#ifndef _MYSTRING_H
#define _MYSTRING_H

#include <stdio.h>

//判断是否是自定义类型定义关键字
int isTypeDefKeyWords(char* s);
//判断是否一个字符串是基础数据类型关键字,是返回非0值，不是返回0
int isBaseType(char* s);

int myfgets(char* buf,char* stops,FILE* fin);

int mystrReplace(char* s,char old,char new);


#endif