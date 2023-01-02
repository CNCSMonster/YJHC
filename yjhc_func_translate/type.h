#ifndef _TYPE_H
#define _TYPE_H

#include "string_index.h"
#include "str_set.h"
#include "../yjhc_preProcess/mystring.h"
#include "vector.h"

//׼�����ͱ����ݽṹ
//���Ͱ����Զ��������Լ�������������
//�����������Ͱ���int,long,short,double,float
//�Զ������Ͱ���,enum,struct,union

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
  TYPE_UNKNOW     //δ֪����
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


//����֧��enum�Լ���
typedef struct struct_type{
  TypeKind  kind; //��¼�����͵�����
  //��Ҫһ����ϣ��������fields,����<key,val>Ϊ<fieldName,fieldTypeName>�Ĺ�ϣ��
  hashtbl fields;
  //funcs�������ٵ��ж�һ���������Ƿ���ڸ�������
  StrSet funcs;
}Type;

//Ҫ��,�ܹ������ַ������ٲ��ҵ���Ӧ������
typedef struct struct_type_table
{
  /* data */
  Type* types;  //��̬����洢table������
  int size;   //��¼�Ѿ����ɵ�size������
  //ʹ��һ����ϣ��������ַ������Ӧ��(����,ָ����)�ԡ�ÿ����һ��long������,long��ǰ32λ�������±�,��32λ��ָ����
  StrIdTable strIds;  
}TypeTbl;

//���ļ��ж�ȡ�������ͱ�,ע��,���ͱ��ǰ������Ҫ����Ϊ��������
TypeTbl getTypeTbl(FILE* fin);

//���ݴ�strid��ȡ����long long id�ֽ����Ӧ��kind�±��Լ�ָ��ά��
//ָ��ά��Ϊ0��ʾ����������ͱ���
//long longǰ32λΪ��Ӧ����type���±�,��32λΪ������Ͷ�Ӧ��ָ����
//long long
void getTypeIndexAndPointerLayer(long long code,int* typeIndex,int* pointerLayer );

//����kindIndex��pointerLayer��ö�Ӧ��long long����
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


//��ʽ�������ַ���
void refectorTypeName(char* str);

//�����������ֲ�ѯһ������,���ظ����������ͱ��е��±�
int findType(TypeTbl* tbl,char* typeName,int* layerRet);

//���һ��type����������
void delType(Type* type);


//ɾ�����ͱ�
int delTypeTbl(TypeTbl* tbl);


//Ϊ�˼򵥿���,ȡ���˺����ڽṹ�嶨���Լ�������typedef��ʹ�õ�֧��



#endif
