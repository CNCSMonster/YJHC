#include "token_reader.h"


//使用模块前的init方法
void init_token_reader(FILE* fin){
  tb_fin=fin;
  head.last=NULL;
  head.next=NULL;
  preKindStack.next=NULL;//初始摆上init块
  
}

//返回一条语句,通过函数指针返回对块的进出情况的判断,以及输出是否要换行的判断
TBNode* token_readsentence(enum BlockAction* blockAction,enum PrintAction* printAction,struct TranslateAction* translateAction ){
  //语法制导翻译-part1:分割+打印+块动作翻译
  







  
}



//释放语句空间
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