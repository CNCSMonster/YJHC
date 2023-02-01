#ifndef _VAR_TBL_H
#define _VAR_TBL_H

#include "func_tbl.h"
#include "type.h"

#include "stluse.h"
#include "type.h"

//准备量表数据结构,该结构用来保存全局变量名常量名以及对应量的类型


//首先准备量类型
typedef struct struct_val{
  char* name;   //记录量的名字
  int isConst;  //记录是常量还是变量,该值为非0值表示是常量，为0表示是变量
  char* val;  //如果是常量,则保存常量的值
}Val;


//然后准备量表类型
typedef struct struct_val_tbl{
  //用一个可变长数组以及一个从字符串映射到下标的数据结构保存val信息
  vector vals;
  StrIdTable valIds;

  //再保存一个变量名与类型名的映射,这里的变量名可以是函数指针名,绑定的类型则为格式化后的函数指针类型
  hashtbl valToType;
  
  //局部类型表
  TypeTbl typeTbl;  //可以定义一个空的类型表


  //局部类型表,对于局部定义的类型可以在这里查询,TODO,但是笔者不打算实现局部类型
  struct struct_val_tbl* pre;   //上一个量表
  struct struct_val_tbl* next;  //下一个量表
}ValTbl;


//创建一个量表,创建该量表时要指定使用的类型表
ValTbl getValTbl(TypeTbl typeTbl);

//从文件加载全局变量信息表,成功返回非0值，失败返回0
int loadFile_valtbl(ValTbl* valTbl,FILE* fin);

//从一个量定义语句中加载量到量表中，成功返回非0值，失败返回0
int loadLine_valtbl(ValTbl* val_tbl,char* str);

//获取一个量
Val getVal(char* name,int isConst,char* defaultVal);

//从局部变量表开始从局部变量根据变量名查找变量的信息,包括变量类型以及变量的名字,值等信息
//查找成功返回非0值,查找失败返回0
//通过retVal指针返回val的基本信息,通过retType返回type的基本信息,通过typeLayer返回type对应的基本type的指针层次
//比如查找int* a;语句定义的变量a
//返回的基本类型为int,返回的指针层次为0
int findVal(ValTbl* curTbl,char* valName,Val* retVal,Type* retType,int* typeLayer);

//插入新的局部表,返回新的局部表的指针,扩展失败返回NULL
ValTbl* extendValTbl(ValTbl* curTbl);

//回收curTbl后面的下一个局部表,返回新的局部表的指针,
ValTbl* recycleValTbl(ValTbl* partialTbl);


//往量表中加入值
void addVal_valtbl(ValTbl* valTbl,char* valName,char* defaultVal,const int isConst,char* typeName,int typeLayer);




//往量表中加载一个函数的参数
void loadArgs_valtbl(ValTbl* valTbl,FuncTbl* funcTbl,Func* func);

//通过量表查找类型,查找成功返回非0值，查找失败返回0
int findType_valtbl(ValTbl* topValTbl,char* typeName,Type* retType,int* retLayer);

//从量表中查找函数指针名,返回的结果为对应的函数指针的名字和类型



//删除一个量表
void del_valTbl(ValTbl* valTbl);

//展示一个量
void show_val(Val* val);

//删除一个量
void delVal(Val* val);

//获取带指针层次的类型名
char* getTypeName(const char* baseTypeName,int layer);


#endif
