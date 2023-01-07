#ifndef _STR_SET_H
#define _STR_SET_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STR_SET_DEFAULT_ARR_SIZE 200

struct str_set_node{
  char* s;
  struct str_set_node* next;
};

typedef struct str_set{
  struct str_set_node* arr[STR_SET_DEFAULT_ARR_SIZE];
  int (*hash)(const char*); //�����趨ʹ�õĹ�ϣ����
  int num;  //��¼������ַ���������
}StrSet;

//����ʹ�õĹ�ϣ����
StrSet getStrSet(int (*hash)(const char*));

//�����ַ���,����ʧ�ܷ���0,�ɹ����ط�0ֵ
int addStr_StrSet(StrSet* ssp,char* s);

//ɾ���ַ���,�ɹ����ط�0,ʧ�ܷ���0
int delStr_StrSet(StrSet* ssp,char* s);

//�ж��ַ����Ƿ����
int containsStr_StrSet(StrSet* ssp,char* str);


//�ͷű�ռ�,�ͷ�������ͬ�մ�����һ��,û�з���ռ�,��������ʹ��
int initStrSet(StrSet* ssp);

//����ַ���
char** toStrArr_StrSet(StrSet* ssp);


#endif