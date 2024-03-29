#ifndef _STRING_ID_H
#define _STRING_ID_H

#include <mem.h>
#include <stdlib.h>

#define STRRING_ID_TABLE_ARR_SIZE 200

//该表用来
struct string_id_node{
  struct string_id_node* next;
  char* str;
  int id;
};

typedef struct string_id_table{
  struct string_id_node* string_id_arr[STRRING_ID_TABLE_ARR_SIZE];
}*StrIdTable;

//获得新哈希表
StrIdTable getStrIdTable();

//获取字符串对应的哈希值
int string_id_hash(char* str);

//获取字符串对应的id,成功返回非负数,否则返回负数
int strToId(StrIdTable table,char* str);

//设置字符串对应的id,成功返回非0值,失败返回0
int putStrId(StrIdTable table,char* str,int id);

//删除字符串与对应的id
int delStr(StrIdTable table,char* str);

//删除整个表
void delStrIdTable(StrIdTable table);



#endif