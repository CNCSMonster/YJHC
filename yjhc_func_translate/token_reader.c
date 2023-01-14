#include "token_reader.h"


//使用模块前的init方法
void init_token_reader(FILE* fin){
  tb_fin=fin;
  head.last=NULL;
  head.next=NULL;
  tail=&head;
  ifSplitAfterAdd=0;
  token_reader_blocks=0;
  oldActionSet.blocks=0;
  oldActionSet.printAction=PRINTSPACE;
  oldActionSet.printTblAction=NOT_DEFINE;
}

//返回一条语句,通过函数指针返回对块的进出情况的判断,以及输出是否要换行的判断
TBNode* readTokenSentence(ActionSet* actionSet){
  //直接返回的情况,结束后分割
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
    int token=newToken.kind;
    //进行各种动作分析
    //首先打印动作分析
    if(PrintAction_Tbl[symbol][token]!=NOT_DEFINE) 
      oldActionSet.printAction=PrintAction_Tbl[symbol][token];
    //然后块动作分析
    if(BlockAction_Tbl[symbol][token]==BlockIn)
      oldActionSet.blocks++;
    else if(BlockAction_Tbl[symbol][token]==BlockOut) oldActionSet.blocks--;
    //然后更新是否打印换行动作
    oldActionSet.printTblAction=PrintTblAction_Tbl[symbol][token];
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
    TBNode* ret=NULL;
    //最后判断是否分割,如果分割，就返回并退出函数
    if((SplitPreAction_Tbl[symbol][token]==SPLITPRE||ifSplitAfterAdd)&&head.next!=NULL){
      ret=head.next;
      head.next=NULL;
      tail=&head;
      if(ifSplitAfterAdd) ifSplitAfterAdd=0;
    }
    //根据splitAfterAction判断下一轮的SplitAfterAction
    if(SplitAfterAction_Tbl[symbol][token]==SPLITAFTER){
      ifSplitAfterAdd=1;
    }
    //加入token前把内容复制给actionset
    memcpy(actionSet,&oldActionSet,sizeof(ActionSet));
    //然后把token放进链表里面
    tail->next=malloc(sizeof(TBNode));
    tail->next->last=tail;
    tail=tail->next;
    tail->next=NULL;
    tail->token=newToken;
    //压入后动作
    if(BlockAction_Tbl[symbol][token]==BlockInAfter){
      oldActionSet.blocks++;
    }
    //最后判断是否要分割
    if(ret!=NULL) return ret;
  }
  return NULL;  //返回NULL表示读取结束
}


//打印tokenLine的语句信息
void show_tokenLine(TBNode* tokens){
  //从左到右打印
  TBNode* track=tokens;
  while(track->next!=NULL){
    printf("%s ",track->token.val);
    track=track->next;
  }
  if(track!=NULL) printf("%s",track->token.val);
}

void fshow_tokenLine(FILE* fout,TBNode* tokens){
  //从左到右打印
  TBNode* track=tokens;
  while(track->next!=NULL){
    fprintf(fout,"%s ",track->token.val);
    track=track->next;
  }
  if(track!=NULL) fprintf(fout,"%s",track->token.val);
}


//以token的(类型编号,字符字面值)的形式写入文件
void fput_tokenLine(FILE* fout,TBNode* tokens){
  TBNode* track=tokens; 
  while(track!=NULL){
    fprintf(fout,"%d %s\n",track->token.kind,track->token.val);
    track=track->next;
  }
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
void release_token_reader(){
  if(head.next!=NULL){
    del_tokenLine(head.next);
  }
  //同时要释放栈里的所有内容
  while(ssStack.next!=NULL){
    struct syntaxSymbolStack* tmp=ssStack.next;
    ssStack.next=tmp->next;
    free(tmp);
  }
  tb_fin=NULL;  //注意这里没有回收文件指针
}