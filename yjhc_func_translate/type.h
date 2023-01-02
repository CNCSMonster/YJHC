#ifndef _TYPE_H
#define _TYPE_H

#include "string_index.h"
#include "str_set.h"
#include "../yjhc_preProcess/mystring.h"
#include "vector.h"

//准备类型表数据结构
//类型包括自定义类型以及基础数据类型
//基础数据类型包括int,long,short,double,float
//自定义类型包括,enum,struct,union

typedef enum enum_type_kind{
  TYPE_INT,
  TYPE_SHORT,
  TYPE_LONG,
  TYPE_LONGLONG,
  TYPE_FLOAT,
  TYPE_DOUBLE,
  TYPE_ENUM,
  TYPE_STRUCT,
  TYPE_UNION,
  TYPE_UNKNOW     //未知类型
}TypeKind;

char* baseTypeNames[]={
  [TYPE_INT] "int",
  [TYPE_SHORT] "short",
  [TYPE_LONG] "long",
  [TYPE_LONGLONG] "long long",
  [TYPE_DOUBLE] "double",
  [TYPE_FLOAT] "float",
  [TYPE_STRUCT] NULL,
  [TYPE_UNION] NULL,
  [TYPE_ENUM] NULL,
  [TYPE_UNKNOW] NULL
};


//仅仅支持enum以及非
typedef struct struct_type{
  TypeKind  kind; //记录该类型的类型
  //需要一个哈希表来保存fields,建立<key,val>为<fieldName,fieldTypeName>的哈希表
  hashtbl fields;
  //funcs用来快速地判断一个函数名是否存在该类型中
  StrSet funcs;
}Type;

//要求,能够根据字符串快速查找到对应的类型
typedef struct struct_type_table
{
  /* data */
  Type* types;  //动态数组存储table的数量
  int size;   //记录已经容纳地size的数量
  //使用一个哈希表绑定类型字符串与对应的(类型,指针层次)对。每对用一个long来保存,long的前32位是类型下标,后32位是指针层次
  StrIdTable strIds;  
}TypeTbl;

//从文件中读取建立类型表,注意,类型表的前面内容要设置为基础类型
TypeTbl getTypeTbl(FILE* fin);

//根据从strid中取出的long long id分解出对应的kind下标以及指针维数
//指针维数为0表示就是这个类型本身
//long long前32位为对应基础type的下标,后32位为这个类型对应的指针层次
//long long
void getTypeIndexAndPointerLayer(long long code,int* typeIndex,int* pointerLayer );

//根据kindIndex和pointerLayer获得对应的long long编码
long long getTypeId(int typeIndex,int pointerLayer);

int typeFieldNameHash(const void* name);

int typeFieldEq(const void* name1,const void* name2);


void showType(Type* type);

//extract enum
Type extractEnum(char* str);

//extract union
Type extractUnion(char* str);

//extract struct
Type extractStruct(char* str);


//格式化类型字符串
void refectorTypeName(char* str);

//根据类型名字查询一个类型,返回该类型在类型表中的下标
int findType(TypeTbl* tbl,char* typeName,int* layerRet);

//清空一个type的所有内容
void delType(Type* type);


//删除类型表
int delTypeTbl(TypeTbl* tbl);


//为了简单考虑,取代了函数内结构体定义以及函数内typedef的使用的支持



#endif
