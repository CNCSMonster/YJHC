#include "string_index.h"

StrIdTable getStrIdTable(){
  StrIdTable table=malloc(sizeof(struct string_index_table));
  memset(table->string_id_arr,0,sizeof(table->string_id_arr));
  return table;
}



//获取字符串对应的哈希值
int string_id_hash(char* str){
  //缩小信息
  int len=strlen(str);
  char hash=0;
  for(int i=0;i<len;i++){
    hash^=str[i]; //使用位运算大概获得每个位置的特征
  }
  return (int)hash+len;
}


long long strToId(StrIdTable table,char* str){
  int hash=string_id_hash(str);
  struct string_index_node* target=table->string_id_arr[hash];
  while(target!=NULL&&strcmp(target->str,str)!=0){
    target=target->next;
  }
  if(target==NULL) return -1;
  return target->id;
}

//设置字符串对应的id
int putStrId(StrIdTable table,char* str,long long id){
  int hash=string_id_hash(str);
  struct string_index_node* pre=NULL;
  struct string_index_node* cur=table->string_id_arr[hash];
  while(cur!=NULL&&strcmp(cur->str,str)!=0){
    pre=cur;
    cur=cur->next;
  }
  //如果对应的字符串已经存在了,则添加失败
  if(cur!=NULL) return 0;  
  if(pre==NULL){
    cur=table->string_id_arr[hash]=malloc(sizeof(struct string_index_node));
    cur->next=NULL;
    strcpy(cur->str=malloc(strlen(str)+1),str);
    cur->id=id;
  }else{
    cur=pre->next=malloc(sizeof(struct string_index_node));
    cur->next=NULL;
    strcpy(cur->str=malloc(strlen(str)+1),str);
    cur->id=id;
  }
  return 1;
}

int delStr(StrIdTable table,char* str){
  int hash=string_id_hash(str);
  struct string_index_node* pre=NULL;
  struct string_index_node* cur=table->string_id_arr[hash];
  while(cur!=NULL&&strcmp(cur->str,str)!=0){
    pre=cur;
    cur=cur->next;
  }
  //如果对应字符串不存在,则删除失败
  if(cur==NULL){
    return 0;
  }
  //如果对应字符串是第一个
  if(pre==NULL){
    free(cur->str);
    pre=cur->next;
    free(cur);
    table->string_id_arr[hash]=pre;
  }
  //否则
  else{
    pre->next=cur->next;
    free(cur->str);
    free(cur);
  }
  return 1;
}

//删除整个表
void delStrIdTable(StrIdTable table){
  for(int i=0;i<STRRING_ID_TABLE_ARR_SIZE;i++){
    while(table->string_id_arr[i]!=NULL){
      struct string_index_node* tmp=table->string_id_arr[i];
      table->string_id_arr[i]=tmp->next;
      free(tmp->str);
      free(tmp);
    }
  }
}