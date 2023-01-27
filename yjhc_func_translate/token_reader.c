#include "token_reader.h"


//ʹ��ģ��ǰ��init����
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

//����һ�����,ͨ������ָ�뷵�ضԿ�Ľ���������ж�,�Լ�����Ƿ�Ҫ���е��ж�
TBNode* readTokenSentence(ActionSet* actionSet){
  //ֱ�ӷ��ص����,������ָ�
  //�﷨�Ƶ�����-part1:�ָ�+��ӡ+�鶯������
  while(1){
    int symbol=ssStack.next==NULL?ssStack.kind:ssStack.next->kind;
    Token newToken=getToken(tb_fin);
    //��ȡ����
    if(newToken.val==NULL){
      if(tb_head.next==NULL) break;
      TBNode* ret=tb_head.next;
      tb_head.next=NULL;
      tail=&tb_head;
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
    if((SplitPreAction_Tbl[symbol][token]==SPLITPRE||ifSplitAfterAdd)&&tb_head.next!=NULL){
      ret=tb_head.next;
      tb_head.next=NULL;
      tail=&tb_head;
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

//�ϲ��ڵ�õ�һ���ڵ�,ָ���м����,���ϲ��Ľڵ�ռ�
TBNode* connect_tokens(TBNode* head,TokenKind kind,const char* sepStr){
  if(head==NULL) return NULL;
  TBNode* out=head;
  //ʹ�ö��α���
  int size=1;
  //��һ�α�����ȡ����Ҫ�Ŀռ��С
  while(out!=NULL){
    size+=strlen(out->token.val);
    if(out->next!=NULL) size+=strlen(sepStr);
    out=out->next;
  }
  char* name=malloc(size);
  //�ڶ��α�������д��
  char* track=name;
  strcpy(track,head->token.val);
  track+=strlen(head->token.val);
  out=head;
  while(out->next!=NULL){
    //����sepStr
    strcpy(track,sepStr);
    track+=strlen(sepStr);
    //�����ɾ���ڵ��ַ���
    strcpy(track,out->next->token.val);
    track+=strlen(out->next->token.val);
    //ɾ����ɾ���ڵ�
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
  if(out!=NULL&&out->last==NULL&&out->next==NULL) return out; //��ȫ����
  return NULL;
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

//�ͷ�ʣ����������,�÷�����������������쳣������ʱ��ʹ��
void release_token_reader(){
  if(tb_head.next!=NULL){
    del_tokenLine(tb_head.next);
  }
  //ͬʱҪ�ͷ�ջ�����������
  while(ssStack.next!=NULL){
    struct syntaxSymbolStack* tmp=ssStack.next;
    ssStack.next=tmp->next;
    free(tmp);
  }
  tb_fin=NULL;  //ע������û�л����ļ�ָ��
}


//����һ����̬����ռ�Ĺ���TBNode*
TBNode* getTBNode(const char* str,TokenKind kind){
  TBNode* out;
  out=malloc(sizeof(TBNode));
  out->last=NULL;
  out->next=NULL;
  out->token.kind=kind;
  out->token.val=strcpy(malloc(strlen(str)+1),str);
  return out;
}




