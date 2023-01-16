#ifndef _FUNC_H
#define _FUNC_H


#include "stluse.h"
#include "type.h"


//形式参数信息
typedef struct struct_func_arg{
  char* name; //形参名
  int isConst;  //判断是否是常量
  long long typeId; //形参类型信息编码
}Arg;


//记录函数的信息,比如形参
//一个函数的信息确定后是不会更改的
//而且函数的参数里面使用的信息只能允许全局信息
//所以表也是确定的
typedef struct struct_func{
  long long retTypeId;  //返回类型信息的编码
  char* func_name;  //函数名
  char* owner;  //函数主的名字
  //函数参数不应该很多,所以我使用个动态数组保存
  vector args;  //Arg的列表
}Func;

//展示一个函数
void showFunc(Func* func);

void del_func(Func* func);


#endif


