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
  int (*hash)(const char*); //可以设定使用的哈希函数
  int num;  //记录加入的字符串的数量
}StrSet;

//设置使用的哈希函数
StrSet getStrSet(int (*hash)(const char*));

//加入字符串,加入失败返回0,成功返回非0值
int addStr_StrSet(StrSet* ssp,char* s);

//删除字符串,成功返回非0,失败返回0
int delStr_StrSet(StrSet* ssp,char* s);

//判断字符串是否存在
int containsStr_StrSet(StrSet* ssp,char* str);


//释放表空间,释放完后表如同刚创建的一样,没有分配空间,可以重新使用
int initStrSet(StrSet* ssp);

//获得字符串
char** toStrArr_StrSet(StrSet* ssp);


#endif