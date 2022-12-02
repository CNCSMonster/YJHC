#include "string_id.h"

//��ʼ����ϣ��
void initStrIdTable(StrIdTable table){
  memset(table->string_id_arr,NULL,sizeof(table->string_id_table));
}

//��ȡ�ַ�����Ӧ�Ĺ�ϣֵ
int string_id_hash(char* str){
  //��С��Ϣ
  int len=strlen(str);
  char hash=0;
  for(int i=0;i<len;i++){
    hash^=str[i]; //ʹ��λ�����Ż��ÿ��λ�õ�����
  }
  return int(hash)+len;
}


int strToId(StrIdTable table,char* str){
  int hash=string_id_hash(str);
  struct string_id_node* target=table->string_id_arr[hash];
  while(target!=NULL&&strcmp(target->str,str)!=0){
    target=target->next;
  }
  if(target==NULL) return -1;
  return target->id;
}

//�����ַ�����Ӧ��id
int putStrId(StrIdTable table,char* str,int id){
  int hash=string_id_hash(str);
  struct string_id_node* pre=NULL;
  struct string_id_node* cur=table->string_id_arr[hash];
  while(cur!=NULL&&strcmp(cur->str,str)!=0){
    pre=cur;
    cur=cur->next;
  }
  //�����Ӧ���ַ����Ѿ�������,�����ʧ��
  if(cur!=NULL) return 0;  
  if(pre==NULL){
    cur=table->string_id_arr[hash]=malloc(sizeof(struct string_id_node));
    cur->next=NULL;
    strcpy(cur->str=malloc(strlen(str)+1),str);
    cur->id=id;
  }else{
    cur=pre->next=malloc(sizeof(struct string_id_node));
    cur->next=NULL;
    strcpy(cur->str=malloc(strlen(str)+1),str);
    cur->id=id;
  }
  return 1;
}

int delStr(StrIdTable table,char* str){
  int hash=string_id_hash(str);
  struct string_id_node* pre=NULL;
  struct string_id_node* cur=table->string_id_arr[hash];
  while(cur!=NULL&&strcmp(cur->str,str)!=0){
    pre=cur;
    cur=cur->next;
  }
  //�����Ӧ�ַ���������,��ɾ��ʧ��
  if(cur==NULL){
    return 0;
  }
  //�����Ӧ�ַ����ǵ�һ��
  if(pre==NULL){
    free(cur->str);
    pre=cur->next;
    free(cur);
    table->string_id_arr[hash]=pre;
  }
  //����
  else{
    pre->next=cur->next;
    free(cur->str);
    free(cur);
  }
  return 1;
}

