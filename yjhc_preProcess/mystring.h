#ifndef _MYSTRING_H
#define _MYSTRING_H

#include <stdio.h>
#include <string.h>
#include "token.h"
#include "token_kind.h"





//判断是否是自定义类型定义关键字
int isTypeDefKeyWords(char* s);
//判断是否一个字符串是基础数据类型关键字,是返回非0值，不是返回0
int isBaseType(char* s);

int myfgets(char* buf,char* stops,FILE* fin);

int myStrHash(const char* str);

//过滤去除字符串中的某些字符,返回的是过滤后的字符指针,为了返回链式编程
//该函数会删除字符串中在toRemoves出现的字符,后续字符向前补位
char* myStrFilter(char* str,const char* toRemoves);


//从字符串根据指定终结符读取一个字符串
int mysgets(char* buf,const char* stops,const char* src);

//对字符串去除前后不必要字符
int myStrStrip(char* str,const char* prefexs,const char* suffixs);

//判断字符串中是否有个字符,如果是返回非0值,如果不是返回0
int myIsCharInStr(const char* str,const char c);

int mystrReplace(char* s,char old,char new);

//返回字符传到输入中
int ungets(char* s,FILE* fin);

//在已经读取出字符串左双引号的请况下,从文件中读取剩下的字符串,如果读不出符合c语法的字符串,则返回0,读取成功则返回非0值
int freadRestString(FILE* fin,char* returnString);

//判断字符能否被转义
//判断是否是合理的转义字符
int isEscape(char c);

//判断是否是流程控制关键字,如果不是流程控制关键字,返回负数,如果是,返回对应tokenkind
int isKeyForProcessControl(char* s);

//判断是否是界符,如果是返回界符对应的TokenKind,如果不是,返回非正值
int isSep(char c);

//判断是否是运算符组成或者大运算符的组成部分
int isOp(char c);


#endif