#include "token_reader.h"


//ʹ��ģ��ǰ��init����
void init_token_reader(FILE* fin){
  tb_fin=fin;
  head.last=NULL;
  head.next=NULL;
  preKindStack.next=NULL;//��ʼ����init��
  
}

//����һ�����,ͨ������ָ�뷵�ضԿ�Ľ���������ж�,�Լ�����Ƿ�Ҫ���е��ж�
TBNode* token_readsentence(enum BlockAction* blockAction,enum PrintAction* printAction,struct TranslateAction* translateAction ){
  //�﷨�Ƶ�����-part1:�ָ�+��ӡ+�鶯������
  







  
}



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