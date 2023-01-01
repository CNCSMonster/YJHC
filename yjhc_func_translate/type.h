#ifndef _TYPE_H
#define _TYPE_H

#include "string_index.h"
#include "str_set.h"
#include "../yjhc_preProcess/mystring.h"
#include "vector.h"

//����һ�����͵ĳ�Ա���Բ��ܳ���512
#define MAX_FIELD_NUM_OF_TYPE 512

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
  TYPE_UNION
}TypeKind;

//����֧��enum�Լ���
typedef struct struct_type{
  TypeKind  kind; //��¼�����͵�����
  char* fields[MAX_FIELD_NUM_OF_TYPE][2]; //(��������,������)�Ե���������¼���͵�����
  //funcs�������ٵ��ж�һ���������Ƿ���ڸ�������
  StrSet funcs;
}Type;

//Ҫ��,�ܹ������ַ������ٲ��ҵ���Ӧ������
typedef struct struct_type_table
{
  /* data */
  TypeKind* kinds;  //��̬����洢table������
  int size;   //��¼�Ѿ����ɵ�size������
  //ʹ��һ����ϣ��������ַ������Ӧ��(����,ָ����)�ԡ�ÿ����һ��long������,long��ǰ32λ�������±�,��32λ��ָ����
  StrIdTable strIds;  
}TypeTbl;

//���ļ��ж�ȡ�������ͱ�,ע��,���ͱ��ǰ������Ҫ����Ϊ��������
TypeTbl getTypeTbl(FILE* fin);


//�����������ֲ�ѯһ������
Type findType(TypeTbl* tbl,char* typeName);


//ע��һ������Ϊĳ�����͵ı�������nάָ���������������typedef�����
//����˵��,tbl���õ��ı�,other��Ҫע������͵ı���,target��Ҫע������ͱ���,layer�������˵���ڵĲ��
int assignOtherName(TypeTbl* tbl,char* other,char* target,int layer);

//ɾ�����ͱ�
int delTypeTbl(TypeTbl& tbl);


//Ϊ�˼򵥿���,ȡ���˺����ڽṹ�嶨���Լ�������typedef��ʹ�õ�֧��



#endif
