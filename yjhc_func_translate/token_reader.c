#include "token_reader.h"


//使用模块前的init方法
void init_token_reader(FILE* fin){
  tb_fin=fin;
  tb_head.last=NULL;
  tb_head.next=NULL;
  tail=&tb_head;
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
      if(tb_head.next==NULL) break;
      TBNode* ret=tb_head.next;
      tb_head.next=NULL;
      tail=&tb_head;
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
    if((SplitPreAction_Tbl[symbol][token]==SPLITPRE||ifSplitAfterAdd)&&tb_head.next!=NULL){
      ret=tb_head.next;
      tb_head.next=NULL;
      tail=&tb_head;
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

//合并节点得到一个节点,指定中间符号,被合并的节点空间
TBNode* connect_tokens(TBNode* head,TokenKind kind,const char* sepStr){
  if(head==NULL) return NULL;
  TBNode* out=head;
  //使用二次遍历
  int size=1;
  //第一次遍历获取所需要的空间大小
  while(out!=NULL){
    size+=strlen(out->token.val);
    if(out->next!=NULL) size+=strlen(sepStr);
    out=out->next;
  }
  char* name=malloc(size);
  //第二次遍历进行写入
  char* track=name;
  strcpy(track,head->token.val);
  track+=strlen(head->token.val);
  out=head;
  while(out->next!=NULL){
    //加入sepStr
    strcpy(track,sepStr);
    track+=strlen(sepStr);
    //加入待删除节点字符串
    strcpy(track,out->next->token.val);
    track+=strlen(out->next->token.val);
    //删除待删除节点
    TBNode* toDel=out->next;
    out->next=toDel->next;
    if(out->next!=NULL)
      out->next->last=out;
    delToken(toDel->token);
    free(toDel);
  }
  free(out->token.val);
  out->token.val=name;
  out->token.kind=kind;
  if(out!=NULL&&out->last==NULL&&out->next==NULL) return out; //安全返回
  return NULL;
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
  if(tokens==NULL) return;
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
  if(tb_head.next!=NULL){
    del_tokenLine(tb_head.next);
  }
  //同时要释放栈里的所有内容
  while(ssStack.next!=NULL){
    struct syntaxSymbolStack* tmp=ssStack.next;
    ssStack.next=tmp->next;
    free(tmp);
  }
  tb_fin=NULL;  //注意这里没有回收文件指针
}


//产生一个动态分配空间的孤立TBNode*
TBNode* getTBNode(const char* str,TokenKind kind){
  TBNode* out;
  out=malloc(sizeof(TBNode));
  out->last=NULL;
  out->next=NULL;
  out->token.kind=kind;
  out->token.val=strcpy(malloc(strlen(str)+1),str);
  return out;
}




