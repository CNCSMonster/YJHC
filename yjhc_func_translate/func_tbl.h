#ifndef _FUNC_TBL_H
#define _FUNC_TBL_H

#include "func.h"

//׼��������,������Ӧ���ܹ����ݺ������������������ٲ��ҵ�
//
typedef struct struct_func_table{
  vector funcKeys; //һ��˳����溯��������
  TypeTbl* globalTypeTbl;  //ȫ�ֱ�����
  hashtbl funcs;  //������
}FuncTbl;


//��ø���ʼ������,
FuncTbl getFuncTbl(TypeTbl* typeTbl);

//���غ����ļ�
int loadFile_functbl(FuncTbl* funcTbl,FILE* fin);

//����һ���ַ���
int loadLine_functbl(FuncTbl* funcTbl,char* str);

//��һ���ַ���������ȡ����������Ϣ,ʹ�õ�һ�����ͱ�
int extractArgFromLine(TypeTbl *typeTbl, Arg *retArg, char* argStr);

//���Һ���,���ҵ����غ���ָ��,û�в��ҵ�����NULL
Func* findFunc(FuncTbl* funcTbl,char* funcName,char* owner);


//�ͷź�����ռ�
void del_functbl(FuncTbl* funcTbl);





#endif