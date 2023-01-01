#include "token_reader.h"


//ʹ��ģ��ǰ��init����
void init_token_reader(FILE* fin){
  tb_fin=fin;
  head.last=NULL;
  head.next=NULL;
  tail=&head;
}

//����һ�����,ͨ������ָ�뷵�ضԿ�Ľ���������ж�,�Լ�����Ƿ�Ҫ���е��ж�
TBNode* readTokenSentence(BlockAction* blockAction,PrintAction* printAction){
  //�﷨�Ƶ�����-part1:�ָ�+��ӡ+�鶯������
  while(1){
    int symbol=ssStack.next==NULL?ssStack.kind:ssStack.next->kind;
    Token newToken=getToken(tb_fin);
    //��ȡ����
    if(newToken.val==NULL){
      if(head.next==NULL) break;
      TBNode* ret=head.next;
      head.next=NULL;
      tail=&head;
      return ret;
    }
    //������������
    //�Ȱ�token�Ž���������
    tail->next=malloc(sizeof(TBNode));
    tail->next->last=tail;
    tail=tail->next;
    tail->next=NULL;
    tail->token=newToken;

    int token=newToken.kind;
    enum BlockAction* m;
    //���и��ֶ�������
    //���ȴ�ӡ��������
    if(PrintAction_Tbl[symbol][token]!=NOT_DEFINE) 
      *printAction=PrintAction_Tbl[symbol][token];
    //Ȼ��鶯������
    if(BlockAction_Tbl[symbol][token]!=NOT_DEFINE)
      *blockAction=BlockAction_Tbl[symbol][token];
    //Ȼ��ջ��������
    //���ж��Ƿ񵯳�
    if(StackPopAction_Tbl[symbol][token]==Pop){
      if(ssStack.next==NULL) printf("err\n");
      else{
        struct syntaxSymbolStack* t=ssStack.next;
        ssStack.next=t->next;
        free(t);
      }
    }
    //���ж��Ƿ�Ҫѹ�룬Ҫѹ��ʲô
    if(StackPushAction_Tbl[symbol][token]!=NOT_DEFINE){
      struct syntaxSymbolStack* add=malloc(sizeof(struct syntaxSymbolStack));
      add->kind=StackPushAction_Tbl[symbol][token];
      add->next=ssStack.next;
      ssStack.next=add;
    }
    //����ж��Ƿ�ָ�,����ָ�ͷ��ز��˳�����
    if(SplitAction_Tbl[symbol][token]==SPLIT){
      TBNode* ret=head.next;
      head.next=NULL;
      tail=&head;
      return ret;
    }
  }
  return NULL;  //����NULL��ʾ��ȡ����
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

//�ͷ�ʣ����������,�÷�����������������쳣������ʱ��ʹ��
void del_rest(){
  if(head.next!=NULL){
    del_tokenLine(head.next);
  }
}