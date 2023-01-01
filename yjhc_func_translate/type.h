#ifndef _TYPE_H
#define _TYPE_H

#include "string_index.h"
#include "str_set.h"
#include "../yjhc_preProcess/mystring.h"
#include "vector.h"

//限制一个类型的成员属性不能超过512
#define MAX_FIELD_NUM_OF_TYPE 512

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
  TYPE_UNION
}TypeKind;

//仅仅支持enum以及非
typedef struct struct_type{
  TypeKind  kind; //记录该类型的类型
  char* fields[MAX_FIELD_NUM_OF_TYPE][2]; //(属性类型,属性名)对的数组来记录类型的属性
  //funcs用来快速地判断一个函数名是否存在该类型中
  StrSet funcs;
}Type;

//要求,能够根据字符串快速查找到对应的类型
typedef struct struct_type_table
{
  /* data */
  TypeKind* kinds;  //动态数组存储table的数量
  int size;   //记录已经容纳地size的数量
  //使用一个哈希表绑定类型字符串与对应的(类型,指针层次)对。每对用一个long来保存,long的前32位是类型下标,后32位是指针层次
  StrIdTable strIds;  
}TypeTbl;

//从文件中读取建立类型表,注意,类型表的前面内容要设置为基础类型
TypeTbl getTypeTbl(FILE* fin);


//根据类型名字查询一个类型
Type findType(TypeTbl* tbl,char* typeName);


//注册一个类型为某个类型的别名或者n维指针别名，用来处理typedef的情况
//参数说明,tbl是用到的表,other是要注册的类型的别名,target是要注册的类型本名,layer是相对来说属于的层次
int assignOtherName(TypeTbl* tbl,char* other,char* target,int layer);

//删除类型表
int delTypeTbl(TypeTbl& tbl);


//为了简单考虑,取代了函数内结构体定义以及函数内typedef的使用的支持



#endif
