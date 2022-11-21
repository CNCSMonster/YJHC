#ifndef _MYSTRING_H
#define _MYSTRING_H

#include <stdio.h>

//判断是否是自定义类型定义关键字
int isTypeDefKeyWords(char* s);
//判断是否一个字符串是基础数据类型关键字,是返回非0值，不是返回0
int isBaseType(char* s);

int myfgets(char* buf,char* stops,FILE* fin);

//从字符串根据指定终结符读取一个字符串
int mysgets(char* buf,const char* stops,const char* src);

int mystrReplace(char* s,char old,char new);


#endif