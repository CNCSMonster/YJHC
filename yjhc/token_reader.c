#include "token_reader.h"


//使用模块前的init方法
void init_token_reader(FILE* fin){
  tb_fin=fin;
  head.last=NULL;
  head.next=NULL;
  tail=&head;
}

//返回一条语句,通过函数指针返回对块的进出情况的判断,以及输出是否要换行的判断
TBNode* readTokenSentence(BlockAction* blockAction,PrintAction* printAction){
  //语法制导翻译-part1:分割+打印+块动作翻译
  while(1){
    int symbol=ssStack.next==NULL?ssStack.kind:ssStack.next->kind;
    Token newToken=getToken(tb_fin);
    //读取结束
    if(newToken.val==NULL){
      if(head.next==NULL) break;
      TBNode* ret=head.next;
      head.next=NULL;
      tail=&head;
      return ret;
    }
    //否则正常进行
    //先把token放进链表里面
    tail->next=malloc(sizeof(TBNode));
    tail->next->last=tail;
    tail=tail->next;
    tail->next=NULL;
    tail->token=newToken;

    int token=newToken.kind;
    enum BlockAction* m;
    //进行各种动作分析
    //首先打印动作分析
    if(PrintAction_Tbl[symbol][token]!=NOT_DEFINE) 
      *printAction=PrintAction_Tbl[symbol][token];
    //然后块动作分析
    if(BlockAction_Tbl[symbol][token]!=NOT_DEFINE)
      *blockAction=BlockAction_Tbl[symbol][token];
    //然后栈动作分析
    //先判断是否弹出
    if(StackPopAction_Tbl[symbol][token]==Pop){
      if(ssStack.next==NULL) printf("err\n");
      else{
        struct syntaxSymbolStack* t=ssStack.next;
        ssStack.next=t->next;
        free(t);
      }
    }
    //再判断是否要压入，要压入什么
    if(StackPushAction_Tbl[symbol][token]!=NOT_DEFINE){
      struct syntaxSymbolStack* add=malloc(sizeof(struct syntaxSymbolStack));
      add->kind=StackPushAction_Tbl[symbol][token];
      add->next=ssStack.next;
      ssStack.next=add;
    }
    //最后判断是否分割,如果分割，就返回并退出函数
    if(SplitAction_Tbl[symbol][token]==SPLIT){
      TBNode* ret=head.next;
      head.next=NULL;
      tail=&head;
      return ret;
    }
  }
  return NULL;  //返回NULL表示读取结束
}



void del_tokenLine(TBNode* tokens){
  TBNode* tmp=tokens->next;
  while(tmp!=NULL){
    tokens->next=tmp->next;
    delToken(tmp->token);
    free(tmp);
    tmp=tokens->next;
  }
  delToken(tokens->token);
  free(tokens);
}

//释放剩余所有内容,该方法可以在语义分析异常结束的时候使用
void del_rest(){
  if(head.next!=NULL){
    del_tokenLine(head.next);
  }
}