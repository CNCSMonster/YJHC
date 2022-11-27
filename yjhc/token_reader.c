#include "token_reader.h"


//ʹ��ģ��ǰ��init����
void init_token_reader(FILE* fin){
  tb_fin=fin;
  head.last=NULL;
  head.next=NULL;
  preKindStack.next=NULL
}

//����һ�����,ͨ������ָ�뷵�ضԿ�Ľ���������ж�,�Լ�����Ƿ�Ҫ���е��ж�
TBNode* token_readsentence(enum BlockAction* blockAction,enum PrintAction* printAction,struct TranslateAction* translateAction ){
  //������һ���ַ���ͬ��״̬,ʹ�ò�ͬ�Ĵ������
  //���ջ�пտ�,˵����û�н��뺯��
  if(preKindStack.next==NULL){
    return preFunc_read(blockAction,printAction,translateAction);
  }
  //�����һ����������ͨ���
  else if(preKindStack.next->kind==NORMALBLOCK_IN){
    return normalBlockIn_read(blockAction,printAction,translateAction);
  }
  //�����һ��������if������ʼ��־
  else if(preKindStack.next->kind==IF_CONDITION_START){
    return ifConditionStart_read(blockAction,printAction,translateAction);
  }
  //���֮ǰ������if����
  else if(preKindStack.next->kind==IF_CONDITION){
    return 
  }
  //�����һ��������if������������
  else if(preKindStack.next->kind==IF_CONDITION_END){

  }
  //�����һ��������if�鿪ʼ��־
  else if(preKindStack.next->kind==IF_BLOCK_IN){

  }
  //���ջ�������־�ǿ�ʼ�������־
  else if(preKindStack.next->kind==IF_BLOCK_OUT){

  }
  //���ջ����else if������ʼ��־
  else if(preKindStack.next->kind==ELIF_CONDITION_START){

  }
  //���ջ����else if����
  else if(preKindStack.next->kind==ELIF_CONDITION){

  }
  //���ջ����else if����������־
  else if(preKindStack.next->kind==ELIF_CONDITION_FINISH){

  }
  //���ջ����else if�鿪ʼ��־
  else if(preKindStack.next->kind==ELSE_BLOCK_IN){

  }
  //���ջ����else�������־
  else if(preKindStack.next->kind==ELSE_BLOCK_OUT){

  }
  //���ջ����forѭ����ʼ��־
  else if(preKindStack.next->kind==FOR_START){

  }
  //���ջ����forInit��
  else if(preKindStack.next->kind==FOR_INIT){
    
  }
  //���ջ����forCondition��
  else if(preKindStack.next->kind==FOR_CONDITION){

  }
  //���ջ����forRefresh��
  else if(preKindStack.next->kind==FOR_REFRESH){

  }
  //���ջ����forBody��ʼ��־
  else if(preKindStack.next->kind==FOR_BODY_IN){

  }
  //���ջ����forBody������־
  else if(preKindStack.next->kind==FOR_BODY_OUT){

  }
  //���ջ����DoBlock��ʼ��
  else if(preKindStack.next->kind==DO_BLOCK_IN){

  }
  //���ջ����DoCondition��ʼ����
  else if(preKindStack.next->kind==DO_CONDITION_START){

  }
  //���ջ����DO_CONDITION��������
  else if(preKindStack.next->kind==DO_CONDITION_END){

  }
  //���ջ����WHILE������ʼ��־
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


//�ͷ����ռ�
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

//�ͷ�ʣ����������,�÷�����������������쳣������ʱ��ʹ��
void del_rest(){
  if(head.next!=NULL){
    del_tokenLine(head.next);
  }
}