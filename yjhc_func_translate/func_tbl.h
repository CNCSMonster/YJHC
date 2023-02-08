#ifndef _FUNC_TBL_H
#define _FUNC_TBL_H

#include "func.h"
#include "func_rename.h"

//准备函数表,函数表应该能够根据函数名还有主人名快速查找到
//
typedef struct struct_func_table{
  vector funcKeys; //一个顺序表保存函数的名字和主人组合成的键
  TypeTbl* globalTypeTbl;  //全局变量表
  hashtbl funcs;  //函数表
}FuncTbl;


//获得个初始函数表,
FuncTbl getFuncTbl(TypeTbl* typeTbl);

//加载函数文件
int loadFile_functbl(FuncTbl* funcTbl,FILE* fin);

//从一行字符串(以\n结尾)中加载函数
int loadLine_functbl(FuncTbl* funcTbl,char* str);

//获取某个成员函数的改名函数
Func* get_rename_member_func(FuncTbl* funcTbl,Func* func);


//从一个字符串里面提取参数定义信息,使用到一个类型表
int extractArgFromLine(TypeTbl *typeTbl, Arg *retArg, char* argStr);

//查找函数,查找到返回函数指针,没有查找到返回NULL
Func* findFunc(FuncTbl* funcTbl,char* funcName,char* owner);


//释放函数表空间
void del_functbl(FuncTbl* funcTbl);





#endif