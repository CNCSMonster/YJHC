#ifndef _MSTR_H
#define _MSTR_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//�ַ��������

//��ȡһ���ַ���ֱ��������ֹ���ż��еķ��Ż��߶�ȡ���ļ�ĩβ,����ֵΪ��ֹ��
int myfgets(char* buf,char* stops,FILE* fin);
int mystrReplace(char* s,char old,char new);

//���ַ�������ָ���ս����ȡһ���ַ���
int mysgets(char* buf,const char* stops,const char* src);


#endif