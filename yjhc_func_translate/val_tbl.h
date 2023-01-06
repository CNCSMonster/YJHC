#ifndef _VAR_TBL_H
#define _VAR_TBL_H

#include "vector.h"
#include "hashtable.h"
#include "type.h"

//׼���������ݽṹ,�ýṹ��������ȫ�ֱ������������Լ���Ӧ��������


//����׼��������
typedef struct struct_val{
  char* name;   //��¼��������
  int isConst;  //��¼�ǳ������Ǳ���,��ֵΪ��0ֵ��ʾ�ǳ�����Ϊ0��ʾ�Ǳ���
  char* val;  //����ǳ���,�򱣴泣����ֵ
}Val;


//Ȼ��׼����������
typedef struct struct_val_tbl{
  //��һ���ɱ䳤�����Լ�һ�����ַ���ӳ�䵽�±�����ݽṹ����val��Ϣ
  vector vals;
  StrIdTable valIds;

  //�ٱ���һ������������������ӳ��
  hashtbl valToType;
  
  //�ֲ����ͱ�
  TypeTbl typeTbl;  //���Զ���һ���յ����ͱ�

  //�ֲ����ͱ�,���ھֲ���������Ϳ����������ѯ,TODO,���Ǳ��߲�����ʵ�־ֲ�����
  struct struct_val_tbl* pre;   //��һ������
  struct struct_val_tbl* next;  //��һ������
}ValTbl;


//����һ������,����������ʱҪָ��ʹ�õ����ͱ�
ValTbl getValTbl(TypeTbl typeTbl);

//���ļ�����ȫ�ֱ�����Ϣ��,�ɹ����ط�0ֵ��ʧ�ܷ���0
int loadFromFile_valtbl(ValTbl* valTbl,FILE* fin);

//��һ������������м������������У��ɹ����ط�0ֵ��ʧ�ܷ���0
int loadLine_valtbl(ValTbl* val_tbl,char* str);

//��ȡһ����
Val getVal(char* name,int isConst,char* defaultVal);

//�Ӿֲ�������ʼ�Ӿֲ��������ݱ��������ұ�������Ϣ,�������������Լ�����������,ֵ����Ϣ
//���ҳɹ����ط�0ֵ,����ʧ�ܷ���0
//ͨ��retValָ�뷵��val�Ļ�����Ϣ,ͨ��retType����type�Ļ�����Ϣ,ͨ��typeLayer����type��Ӧ�Ļ���type��ָ����
//�������int* a;��䶨��ı���a
//���صĻ�������Ϊint,���ص�ָ����Ϊ0
int findVal(ValTbl* curTbl,char* valName,Val* retVal,Type* retType,int* typeLayer);



//�������м���ֵ
void addVal_valtbl(ValTbl* valTbl,char* valName,char* defaultVal,const int isConst,char* typeName);

//ɾ��һ������
void del_valTbl(ValTbl* valTbl);

//ɾ��һ����
void delVal(Val* val);


#endif
