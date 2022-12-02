#ifndef _MSTR_H
#define _MSTR_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//字符串处理库

//读取一个字符串直到读到截止符号集中的符号或者读取到文件末尾,返回值为终止符
int myfgets(char* buf,char* stops,FILE* fin);
int mystrReplace(char* s,char old,char new);

//从字符串根据指定终结符读取一个字符串
int mysgets(char* buf,const char* stops,const char* src);


#endif