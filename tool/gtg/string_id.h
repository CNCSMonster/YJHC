#ifndef _STRING_ID_H
#define _STRING_ID_H

#include <mem.h>
#include <stdlib.h>

#define STRRING_ID_TABLE_ARR_SIZE 200

//�ñ�����
struct string_id_node{
  struct string_id_node* next;
  char* str;
  int id;
};

typedef struct string_id_table{
  struct string_id_node* string_id_arr[STRRING_ID_TABLE_ARR_SIZE];
}*StrIdTable;

//��ʼ����ϣ��
void initStrIdTable(StrIdTable table);

//��ȡ�ַ�����Ӧ�Ĺ�ϣֵ
int string_id_hash(char* str);

//��ȡ�ַ�����Ӧ��id,�ɹ����طǸ���,���򷵻ظ���
int strToId(StrIdTable table,char* str);

//�����ַ�����Ӧ��id,�ɹ����ط�0ֵ,ʧ�ܷ���0
int putStrId(StrIdTable table,char* str,int id);

//ɾ���ַ������Ӧ��id
int delStr(StrIdTable table,char* str);





#endif