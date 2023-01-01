#ifndef _STRING_INDEX_H
#define _STRING_INDEX_H

#include <mem.h>
#include <stdlib.h>

#define STRRING_ID_TABLE_ARR_SIZE 200

//�ñ�����
struct string_index_node{
  struct string_index_node* next;
  char* str;
  int id;
};

//�ַ����±��
typedef struct string_index_table{
  struct string_index_node* string_id_arr[STRRING_ID_TABLE_ARR_SIZE];
}*StrIdTable;

//����¹�ϣ��
StrIdTable getStrIdTable();

//��ȡ�ַ�����Ӧ�Ĺ�ϣֵ
int string_id_hash(char* str);

//��ȡ�ַ�����Ӧ��id,�ɹ����طǸ���,���򷵻ظ���
long long strToId(StrIdTable table,char* str);

//�����ַ�����Ӧ��id,�ɹ����ط�0ֵ,ʧ�ܷ���0
int putStrId(StrIdTable table,char* str,long long id);

//ɾ���ַ������Ӧ��id
int delStr(StrIdTable table,char* str);

//ɾ��������
void delStrIdTable(StrIdTable table);



#endif