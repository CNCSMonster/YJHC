#ifndef _TYPE_H
#define _TYPE_H

#include "stluse.h"
#include "../yjhc_preProcess/mystring.h"

// ��ģ��׼�������͵�������ݽṹ

//׼�����ͱ����ݽṹ
//���Ͱ����Զ��������Լ�������������
//�����������Ͱ���int,long,short,double,float
//�Զ������Ͱ���,enum,struct,union

#define MAX_TYPE_NAME_LENGTH 200

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
  TYPE_FUNC_POINTER,  //���亯��ָ������
  TYPE_UNKNOW     //δ֪����
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
  [TYPE_FUNC_POINTER] "func pointer",
  [TYPE_UNKNOW] "unknown"
};


//׼�������������͵�Ĭ��ֵ
char* defaultValueOfBaseTypes[]={
  [TYPE_INT] "0",
  [TYPE_CHAR] "\'\\0\'",
  [TYPE_LONG] "0L",
  [TYPE_DOUBLE] "0.0L",
  [TYPE_SHORT] "0",
  [TYPE_LONGLONG] "0LL",
  [TYPE_FLOAT] "0.0",
  [TYPE_ENUM] "0",
  [TYPE_STRUCT] "{0}",  //��Ȼ�����еı�������֧��
  [TYPE_UNION] "{0}",
  [TYPE_FUNC_POINTER] "((void*)0)",
  [TYPE_UNKNOW] NULL
};



//����֧��enum�Լ���
typedef struct struct_type{
  //��¼���͵Ļ�������
  char* defaultName;  //��¼��������
  TypeKind  kind; //��¼�����͵�����
  //��Ҫһ����ϣ��������fields,����<key,val>Ϊ<fieldName,fieldTypeName>�Ĺ�ϣ��
  hashtbl fields;
  //funcs�������ٵ��ж�һ���������Ƿ���ڸ�������
  StrSet funcs; //funcs������ǽṹ�巽��,�����нṹ�干�е�
  StrSet funcPointerFields; //funcField����������͵ĺ���ָ�����͵�������,�������������޸�
}Type;

//notice,�涨��0λ���������Ϊunknown����
//Ҫ��,�ܹ������ַ������ٲ��ҵ���Ӧ������
typedef struct struct_type_table
{
  /* data */
  vector types;
  //ʹ��һ����ϣ��������ַ������Ӧ��(����,ָ����)�ԡ�ÿ����һ��long������,long��ǰ32λ�������±�,��32λ��ָ����
  StrIdTable strIds;  
}TypeTbl;

//��ȡȫ�����ͱ�,�����л�����������
TypeTbl getGlobalTypeTbl();

//��ȡһ��ֻ��0λ�õ�unknown�������͵����ͱ�
TypeTbl getTypeTbl();

// ��ȡ���л����������͵�ȫ������

//���ļ��ж�ȡ�������ͱ�,ע��,���ͱ��ǰ������Ҫ����Ϊ��������
//���سɹ����ط�0ֵ,����ʧ�ܷ���0
int loadFile_typeTbl(TypeTbl* tbl,FILE* fin);

//��ȡ������typedefline��Ϣ������
int loadTypedefLine_typetbl(TypeTbl* tbl,char* str);

//���ݴ�strid��ȡ����long long id�ֽ����Ӧ��kind�±��Լ�ָ��ά��
//ָ��ά��Ϊ0��ʾ����������ͱ���
//long longǰ32λΪ��Ӧ����type���±�,��32λΪ������Ͷ�Ӧ��ָ����
//long long
void extractTypeIndexAndPointerLayer(long long code,int* typeIndex,int* pointerLayer );

//����kindIndex��pointerLayer��ö�Ӧ��long long����
long long getTypeId(int typeIndex,int pointerLayer);

int typeFieldNameHash(const void* name);

int typeFieldEq(const void* name1,const void* name2);

//���ͱ�����ַ�����������Ϣ,�ɹ����ط�0ֵ,ʧ�ܷ���0
int loadLine_typetbl(TypeTbl* tbl,char* str);

//����ָ���������
int loadTypedefFuncPointer_typetbl(TypeTbl* tbl,char* str);

//��ù�񻯵ĺ���ָ��������,��ȡ�ɹ����ض�Ӧ����ָ��,��ȡʧ�ܷ���NULL
char* refectorFuncPointerName(char* str);

void showType(Type* type);

//extract enum
Type extractEnum(char* str);

//extract union
Type extractUnion(char* str);

//extract struct
Type extractStruct(char* str);


//��ʽ�������ַ���,��ʽ���ɹ����ط�0ֵ,��ʽ�������쳣����0
int refectorTypeName(char* str);

//�ж�����Ƿ��Ǻ���ָ���������Զ������,�Ƿ��ط�0ֵ,���Ƿ���0
int isFuncPointerFieldDef(char* str);

//�ж��Ƿ��Ǻ���ָ�����������ߺ���ָ�������,����Ƿ��ط�0ֵ,������Ƿ���0
int isFuncPointerType(const char* str);

//���غ���ָ�붨�������Ϣ��������,���سɹ�����1,����ʧ�ܷ���0
int loadFuncPointerFieldDef(Type* typep,char* str);

//�����������ֲ�ѯһ������,���ظ����������ͱ��е��±��Լ�����������Ӧ�ĸ����͵�ָ����
//������ҳɹ�,���ص������±�Ϊ����,�������ʧ��,����Ϊ0,��0λ�ñ���unknown����
int findType(TypeTbl* tbl,char* typeName,int* layerRet);


//���һ��type����������
void delType(Type* type);


//ɾ�����ͱ�
int delTypeTbl(TypeTbl* tbl);


//Ϊ�˼򵥿���,ȡ���˺����ڽṹ�嶨���Լ�������typedef��ʹ�õ�֧��



#endif
