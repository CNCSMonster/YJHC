#include "token_reader.h"


//使用模块前的init方法
void init_token_reader(FILE* fin){
  tb_fin=fin;
  head.last=NULL;
  head.next=NULL;
  preKindStack.next=NULL
}

//返回一条语句,通过函数指针返回对块的进出情况的判断,以及输出是否要换行的判断
TBNode* token_readsentence(enum BlockAction* blockAction,enum PrintAction* printAction,struct TranslateAction* translateAction ){
  //根据上一个字符不同的状态,使用不同的处理过程
  //如果栈中空空,说明还没有进入函数
  if(preKindStack.next==NULL){
    return preFunc_read(blockAction,printAction,translateAction);
  }
  //如果上一个内容是普通入块
  else if(preKindStack.next->kind==NORMALBLOCK_IN){
    return normalBlockIn_read(blockAction,printAction,translateAction);
  }
  //如果上一个内容是if条件开始标志
  else if(preKindStack.next->kind==IF_CONDITION_START){
    return ifConditionStart_read(blockAction,printAction,translateAction);
  }
  //如果之前类型是if条件
  else if(preKindStack.next->kind==IF_CONDITION){
    return 
  }
  //如果上一个内容是if条件结束符号
  else if(preKindStack.next->kind==IF_CONDITION_END){

  }
  //如果上一个内容是if块开始标志
  else if(preKindStack.next->kind==IF_BLOCK_IN){

  }
  //如果栈顶语义标志是开始块结束标志
  else if(preKindStack.next->kind==IF_BLOCK_OUT){

  }
  //如果栈顶是else if条件开始标志
  else if(preKindStack.next->kind==ELIF_CONDITION_START){

  }
  //如果栈顶是else if条件
  else if(preKindStack.next->kind==ELIF_CONDITION){

  }
  //如果栈顶是else if条件结束标志
  else if(preKindStack.next->kind==ELIF_CONDITION_FINISH){

  }
  //如果栈顶是else if块开始标志
  else if(preKindStack.next->kind==ELSE_BLOCK_IN){

  }
  //如果栈顶是else块结束标志
  else if(preKindStack.next->kind==ELSE_BLOCK_OUT){

  }
  //如果栈顶是for循环开始标志
  else if(preKindStack.next->kind==FOR_START){

  }
  //如果栈顶是forInit块
  else if(preKindStack.next->kind==FOR_INIT){
    
  }
  //如果栈顶是forCondition块
  else if(preKindStack.next->kind==FOR_CONDITION){

  }
  //如果栈顶是forRefresh块
  else if(preKindStack.next->kind==FOR_REFRESH){

  }
  //如果栈顶是forBody开始标志
  else if(preKindStack.next->kind==FOR_BODY_IN){

  }
  //如果栈顶是forBody结束标志
  else if(preKindStack.next->kind==FOR_BODY_OUT){

  }
  //如果栈顶是DoBlock开始块
  else if(preKindStack.next->kind==DO_BLOCK_IN){

  }
  //如果栈顶是DoCondition开始符号
  else if(preKindStack.next->kind==DO_CONDITION_START){

  }
  //如果栈顶是DO_CONDITION结束符号
  else if(preKindStack.next->kind==DO_CONDITION_END){

  }
  //如果栈顶是WHILE条件开始标志
  else if(preKindStack.next->kind==WHILE_CONDITION_START){

  }
  else if(preKindStack.next->kind==WHILE_CONDITION_END){

  }
  else if(preKindStack.next->kind==WHILE_BLOCK_IN){

  }
  else if(preKindStack.next->kind==WHILE_BLOCK_FINISH){

  }
  return NULL;
}

TBNode* preFunc_read(enum BlockAction* blockAction,enum PrintAction* printAction,struct TranslateAction* translateAction);

TBNode* normalBlockIn_read(blockAction,printAction,translateAction);

TBNode* ifConditionStart_read(blockAction,printAction,translateAction);

TBNode* ifCondition_read(blockAction,printAction,translateAction);

TBNode* ifConditionEnd_read(blockAction,printAction,translateAction);

TBNode* ifBlockIn_read(blockAction,printAction,translateAction);

TBNode* ifBlockOut_read(blockAction,printAction,translateAction);

TBNode* elifBlockIn_read(blockAction,printAction,translateAction);

TBNode* elifBlockOut_read(blockAction,printAction,translateAction);

TBNode* elseBlockIn_read(blockAction,printAction,translateAction);

TBNode* elseBlockOut_read(blockAction,printAction,translateAction);

TBNode* doBlockIn_read(blockAction,printAction,translateAction);

TBNode* whileBlockIn_read(blockAction,printAction,translateAction);

TBNode* whileBlockIn_read(blockAction,printAction,translateAction);

TBNode* whileConditionStart_read(blockAction,printAction,translateAction);

TBNode* whileConditionEnd_read(blockAction,printAction,translateAction);

TBNode* doConditionStart_read(blockAction,printAction,translateAction);

TBNode* doCondition_read(blockAction,printAction,translateAction);

TBNode* doConditionEnd_read(blockAction,printAction,translateAction);

TBNode* forStart_read(blockAction,printAction,translateAction);

TBNode* forInit_read(blockAction,printAction,translateAction);

TBNode* forCondition_read(blockAction,printAction,translateAction);

TBNode* forRefresh_read(blockAction,printAction,translateAction);

TBNode* forBodyIn_read(blockAction,printAction,translateAction);

TBNode* forBodyOut_read(blockAction,printAction,translateAction);


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