#include "token_reader.h"


//ʹ��ģ��ǰ��init����
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

//����һ�����,ͨ������ָ�뷵�ضԿ�Ľ���������ж�,�Լ�����Ƿ�Ҫ���е��ж�
TBNode* readTokenSentence(ActionSet* actionSet){
  //ֱ�ӷ��ص����,������ָ�
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
    int token=newToken.kind;
    //���и��ֶ�������
    //���ȴ�ӡ��������
    if(PrintAction_Tbl[symbol][token]!=NOT_DEFINE) 
      oldActionSet.printAction=PrintAction_Tbl[symbol][token];
    //Ȼ��鶯������
    if(BlockAction_Tbl[symbol][token]==BlockIn)
      oldActionSet.blocks++;
    else if(BlockAction_Tbl[symbol][token]==BlockOut) oldActionSet.blocks--;
    //Ȼ������Ƿ��ӡ���ж���
    oldActionSet.printTblAction=PrintTblAction_Tbl[symbol][token];
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
    TBNode* ret=NULL;
    //����ж��Ƿ�ָ�,����ָ�ͷ��ز��˳�����
    if((SplitPreAction_Tbl[symbol][token]==SPLITPRE||ifSplitAfterAdd)&&head.next!=NULL){
      ret=head.next;
      head.next=NULL;
      tail=&head;
      if(ifSplitAfterAdd) ifSplitAfterAdd=0;
    }
    //����splitAfterAction�ж���һ�ֵ�SplitAfterAction
    if(SplitAfterAction_Tbl[symbol][token]==SPLITAFTER){
      ifSplitAfterAdd=1;
    }
    //����tokenǰ�����ݸ��Ƹ�actionset
    memcpy(actionSet,&oldActionSet,sizeof(ActionSet));
    //Ȼ���token�Ž���������
    tail->next=malloc(sizeof(TBNode));
    tail->next->last=tail;
    tail=tail->next;
    tail->next=NULL;
    tail->token=newToken;
    //ѹ�����
    if(BlockAction_Tbl[symbol][token]==BlockInAfter){
      oldActionSet.blocks++;
    }
    //����ж��Ƿ�Ҫ�ָ�
    if(ret!=NULL) return ret;
  }
  return NULL;  //����NULL��ʾ��ȡ����
}


//��ӡtokenLine�������Ϣ
void show_tokenLine(TBNode* tokens){
  //�����Ҵ�ӡ
  TBNode* track=tokens;
  while(track->next!=NULL){
    printf("%s ",track->token.val);
    track=track->next;
  }
  if(track!=NULL) printf("%s",track->token.val);
}

void fshow_tokenLine(FILE* fout,TBNode* tokens){
  //�����Ҵ�ӡ
  TBNode* track=tokens;
  while(track->next!=NULL){
    fprintf(fout,"%s ",track->token.val);
    track=track->next;
  }
  if(track!=NULL) fprintf(fout,"%s",track->token.val);
}


//��token��(���ͱ��,�ַ�����ֵ)����ʽд���ļ�
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

//�ͷ�ʣ����������,�÷�����������������쳣������ʱ��ʹ��
void release_token_reader(){
  if(head.next!=NULL){
    del_tokenLine(head.next);
  }
  //ͬʱҪ�ͷ�ջ�����������
  while(ssStack.next!=NULL){
    struct syntaxSymbolStack* tmp=ssStack.next;
    ssStack.next=tmp->next;
    free(tmp);
  }
  tb_fin=NULL;  //ע������û�л����ļ�ָ��
}