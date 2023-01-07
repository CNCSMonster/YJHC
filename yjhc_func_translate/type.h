#ifndef _TYPE_H
#define _TYPE_H

#include "stluse.h"
#include "../yjhc_preProcess/mystring.h"

//准备类型表数据结构
//类型包括自定义类型以及基础数据类型
//基础数据类型包括int,long,short,double,float
//自定义类型包括,enum,struct,union

typedef enum enum_type_kind{
  TYPE_INT,
  TYPE_SHORT,
  TYPE_LONG,
  TYPE_CHAR,
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
  [TYPE_CHAR] "char",
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

char* typeKindName[]={
  [TYPE_INT] "int",
  [TYPE_SHORT] "short",
  [TYPE_CHAR] "char",
  [TYPE_LONG] "long",
  [TYPE_LONGLONG] "long long",
  [TYPE_FLOAT] "float",
  [TYPE_DOUBLE] "double",
  [TYPE_ENUM] "enum",
  [TYPE_STRUCT] "struct",
  [TYPE_UNION] "union",
  [TYPE_UNKNOW] "unknown"
};


//准备基础数据类型的默认值
char* defaultValueOfBaseTypes[]={
  [TYPE_INT] "0",
  [TYPE_CHAR] "\'\\0\'",
  [TYPE_LONG] "0L",
  [TYPE_DOUBLE] "0.0L",
  [TYPE_SHORT] "0",
  [TYPE_LONGLONG] "0LL",
  [TYPE_FLOAT] "0.0",
  [TYPE_ENUM] "0",
  [TYPE_STRUCT] "{0}",  //虽然但是有的编译器不支持
  [TYPE_UNION] "{0}",
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

//notice,规定第0位保存的内容为unknown类型
//要求,能够根据字符串快速查找到对应的类型
typedef struct struct_type_table
{
  /* data */
  vector types;
  //使用一个哈希表绑定类型字符串与对应的(类型,指针层次)对。每对用一个long来保存,long的前32位是类型下标,后32位是指针层次
  StrIdTable strIds;  
}TypeTbl;

//获取全局类型表,里面有基础数据类型
TypeTbl getGlobalTypeTbl();

//获取一个只有0位置的unknown数据类型的类型表
TypeTbl getTypeTbl();

// 获取含有基础数据类型的全局量表

//从文件中读取建立类型表,注意,类型表的前面内容要设置为基础类型
//加载成功返回非0值,加载失败返回0
int loadFile_typeTbl(TypeTbl* tbl,FILE* fin);

//根据从strid中取出的long long id分解出对应的kind下标以及指针维数
//指针维数为0表示就是这个类型本身
//long long前32位为对应基础type的下标,后32位为这个类型对应的指针层次
//long long
void getTypeIndexAndPointerLayer(long long code,int* typeIndex,int* pointerLayer );

//根据kindIndex和pointerLayer获得对应的long long编码
long long getTypeId(int typeIndex,int pointerLayer);

int typeFieldNameHash(const void* name);

int typeFieldEq(const void* name1,const void* name2);

//类型表加载字符串的类型信息,成功返回非0值,失败返回0
int loadLine_typetbl(TypeTbl* tbl,char* str);


void showType(Type* type);

//extract enum
Type extractEnum(char* str);

//extract union
Type extractUnion(char* str);

//extract struct
Type extractStruct(char* str);


//格式化类型字符串
void refectorTypeName(char* str);

//根据类型名字查询一个类型,返回该类型在类型表中的下标以及该类型名对应的该类型的指针层次
//如果查找成功,返回的类型下标为正数,如果查找失败,返回为0,而0位置保存unknown类型
int findType(TypeTbl* tbl,char* typeName,int* layerRet);

//清空一个type的所有内容
void delType(Type* type);


//删除类型表
int delTypeTbl(TypeTbl* tbl);


//为了简单考虑,取代了函数内结构体定义以及函数内typedef的使用的支持



#endif
