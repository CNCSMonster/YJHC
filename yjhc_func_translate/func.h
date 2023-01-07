#ifndef _FUNC_H
#define _FUNC_H

#include "stluse.h"
#include "type.h"

//��ʽ������Ϣ
typedef struct struct_func_arg{
  char* name; //�β���
  int isConst;  //�ж��Ƿ��ǳ���
  long long typeId; //�β�������Ϣ����
}Arg;


//��¼��������Ϣ,�����β�
//һ����������Ϣȷ�����ǲ�����ĵ�
//���Һ����Ĳ�������ʹ�õ���Ϣֻ������ȫ����Ϣ
//���Ա�Ҳ��ȷ����
typedef struct struct_func{
  long long retTypeId;  //����������Ϣ�ı���
  char* func_name;  //������
  char* owner;  //������������
  //����������Ӧ�úܶ�,������ʹ�ø���̬���鱣��
  vector args;  //Arg���б�
}Func;



//׼��������,������Ӧ���ܹ����ݺ������������������ٲ��ҵ�
//
typedef struct struct_func_table{
  TypeTbl* globalTypeTbl;  //ȫ�ֱ�����
  hashtbl funcs;  //������
}FuncTbl;

//��ø���̬����ռ���ַ���,��Ϊָ�����ļ�
char* getFuncKey(char* funcName,long long typeId);

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

//չʾһ������
void showFunc(Func* func);

void del_func(Func* func);

//�ͷź�����ռ�
void del_functbl(FuncTbl* funcTbl);





#endif