#ifndef _MYSTRING_H
#define _MYSTRING_H

#include <stdio.h>

//�ж��Ƿ����Զ������Ͷ���ؼ���
int isTypeDefKeyWords(char* s);
//�ж��Ƿ�һ���ַ����ǻ����������͹ؼ���,�Ƿ��ط�0ֵ�����Ƿ���0
int isBaseType(char* s);

int myfgets(char* buf,char* stops,FILE* fin);

//���ַ�������ָ���ս����ȡһ���ַ���
int mysgets(char* buf,const char* stops,const char* src);

int mystrReplace(char* s,char old,char new);


#endif