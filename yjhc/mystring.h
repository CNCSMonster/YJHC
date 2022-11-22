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


//在已经读取出字符串左双引号的请况下,从文件中读取剩下的字符串,如果读不出符合c语法的字符串,则返回0,读取成功则返回非0值
int freadRestString(FILE* fin);

//判断字符能否被转义
//判断是否是合理的转义字符
int isEscape(char c);


#endif