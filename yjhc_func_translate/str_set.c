#include "str_set.h"

//����ʹ�õĹ�ϣ����
StrSet getStrSet(int (*hash)(const char*)){
  if(hash==NULL){
    fprintf(stderr,"err");
  }
  StrSet out;
  out.hash=hash;
  for(int i=0;i<STR_SET_DEFAULT_ARR_SIZE;i++) out.arr[i]=NULL;
  out.num=0;
  return out;
}

//�����ַ���
int addStr_StrSet(StrSet* ssp,char* s){
  //��ȡȡ����ssp��hash�İ�ȫ�±�
  int index=((ssp->hash(s))%STR_SET_DEFAULT_ARR_SIZE+STR_SET_DEFAULT_ARR_SIZE)%STR_SET_DEFAULT_ARR_SIZE;
  struct str_set_node* tmp=ssp->arr[index];
  struct str_set_node* pre=NULL;
  while (tmp!=NULL)
  {
    if(strcmp(s,tmp->s)==0) return 0;
    pre=tmp;
    tmp=tmp->next;
  }
  if(pre==NULL){
    ssp->arr[index]=malloc(sizeof(struct str_set_node));
    ssp->arr[index]->next=NULL;
    ssp->arr[index]->s=strcpy(malloc(strlen(s)+1),s);
  }
  else{
    pre->next=malloc(sizeof(struct str_set_node));
    pre->next->next=NULL;
    pre->next->s=strcpy(malloc(strlen(s)+1),s);
  }
  ssp->num++;
  return 1;
}

//�ж��ַ����Ƿ����
int containsStr_StrSet(StrSet* ssp,char* str){
  //��ȡȡ����ssp��hash�İ�ȫ�±�
  int index=((ssp->hash(str))%STR_SET_DEFAULT_ARR_SIZE+STR_SET_DEFAULT_ARR_SIZE)%STR_SET_DEFAULT_ARR_SIZE;
  struct str_set_node* tmp=ssp->arr[index];
  while (tmp!=NULL)
  {
    if(strcmp(str,tmp->s)==0) return 1;
    tmp=tmp->next;
  }
  return 0;
}


//ɾ���ַ���
int delStr_StrSet(StrSet* ssp,char* str){
  //��ȡȡ����ssp��hash�İ�ȫ�±�
  int index=((ssp->hash(str))%STR_SET_DEFAULT_ARR_SIZE+STR_SET_DEFAULT_ARR_SIZE)%STR_SET_DEFAULT_ARR_SIZE;
  struct str_set_node* tmp=ssp->arr[index];
  struct str_set_node* pre=NULL;
  while (tmp!=NULL)
  {
    if(strcmp(str,tmp->s)==0) break;;
    pre=tmp;
    tmp=tmp->next;
  }
  if(tmp==NULL) return 0;
  if(pre==NULL){
    ssp->arr[index]=tmp->next;
  }
  else{
    pre->next=tmp->next;
  }
  free(tmp->s);
  free(tmp);
  ssp->num--;
  return 1;
}


//�ͷű�ռ�
int initStrSet(StrSet* ssp){
  int i=0;
  int index=0;
  while (i<ssp->num)
  {
    struct str_set_node* tmp=ssp->arr[index];
    if(tmp==NULL){
      index++;
      continue;
    }
    while (tmp->next!=NULL)
    {
      struct str_set_node* tmp2=tmp->next;
      tmp->next=tmp2->next;
      free(tmp2->s);
      free(tmp2);
    }
    free(tmp->s);
    free(tmp);
    ssp->arr[index]=NULL;
    i++;
    index++;
  }
  ssp->num=0;
  return 1;
}


