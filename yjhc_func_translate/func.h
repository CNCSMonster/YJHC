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



//准备函数表,函数表应该能够根据函数名还有主人名快速查找到
//
typedef struct struct_func_table{
  TypeTbl* globalTypeTbl;  //全局变量表
  hashtbl funcs;  //函数表
}FuncTbl;

//获得个动态分配空间的字符串,作为指向函数的键
char* getFuncKey(char* funcName,long long typeId);

//获得个初始函数表,
FuncTbl getFuncTbl(TypeTbl* typeTbl);

//加载函数文件
int loadFile_functbl(FuncTbl* funcTbl,FILE* fin);

//加载一个字符串
int loadLine_functbl(FuncTbl* funcTbl,char* str);

//从一个字符串里面提取参数定义信息,使用到一个类型表
int extractArgFromLine(TypeTbl *typeTbl, Arg *retArg, char* argStr);

//查找函数,查找到返回函数指针,没有查找到返回NULL
Func* findFunc(FuncTbl* funcTbl,char* funcName,char* owner);

//展示一个函数
void showFunc(Func* func);

void del_func(Func* func);

//释放函数表空间
void del_functbl(FuncTbl* funcTbl);





#endif