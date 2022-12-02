#ifndef _GRAMMAR_TBL_GENERATOR_H
#define _GRAMMAR_TBL_GENERATOR_H

#include "grammar_tbl_generator.h"
#include "hashset.h"
#include "string_id.h"

/*
ӳ���ϵ����:
��token


*/

struct ActionsEachKind{
  HSet actions;
  int actionKind;
  struct ActionsEachKind* next;
};



//������������Ϣ��
struct gtgBlock{
  //���Ĭ���Ǵ���ֵ���ַ���,������ȫ��д���ַ���
  char* default;
  //�ַ���id��
  StrIdTable strIds;
  //token��
  HSet tokens;  //��¼token�õı�,�ܹ����������ж�token�Ƿ����,��ʵ���汣������ַ�����Ӧ���м����
  //���actionKind�Ŀռ�
  HSet actionKinds; //��¼actionKind��Ӧ���м����
  //��Ų�ͬaction�Ŀռ�,�ܹ�����actionKind��Ѱ��ƥ��
  struct ActionsEachKind* actions;
  //���syntax symbol�Ŀռ�
  HSet symbols;

  
};


/*
Ҫ�����������:
��ʲô����action��?
��ʲô����syntax��?
��ʲô����token��?
Ҫ��action�ܹ����ٵ���ɾ��
syntaxҲҪ�ܹ�������ɾ��
��α���syntax����Ϣ?

��˼:
ʹ�ñ��б��ʽ����syntax��Ϣ
�������token COMMA,�Լ�����ջ��symbol BLOCK_IN,��PrintAction NEWLINE
�򱣴�Ϊ
action_kind:top_symbol,token,action����ʽ
��:
PrintAction:BLOCK_IN,COMMA,NEWLINE����ʽ
Ϊ���ܹ����򻯱��,Ӧ��Ҫ�Ѷ�Ӧ���ַ���תΪ���

����:
Action,����Token,����SyntaxSymbol�����ַ���
Ϊ�˽�ʡ����ռ�õ��ڴ棬�����˸�ÿ���ַ������,Ȼ�󱣴��ŵķ���
���ڴ��еı��
�������ĳ���ַ���,������ַ�������һ�����ñ��,Ȼ����һ����ϣ����
�������<key,val>=(str,id),����str�Ǹ��ַ���,idΪ������ñ���ı��,
ͬʱ��Ҫ����һ����ϣ��,��������
<key,val>=(id,str)
���ڴ��м���syntax�﷨��ʱ��ͱ�����,
��ʵ����output�ķ����ʱ��,�͸��ݱ���ҵ���Ӧ���ַ�����
���ɾ����ĳ���ַ���,�����ö�Ӧ�ı��ΪĬ�ϱ��,Ĭ�Ϻ����ʹ��Ĭ���ַ���

Ҳ���Ǿ�������ӳ��

�ַ���(�û���)-->�м����(�ڴ���ʹ��)-->ʵ���ַ���(�û���)
������ϣ��:
[�ַ���,�м����]
[�м����,ʵ���ַ���]

�����м����Ļ���:
���ɾ����ĳ��token�ַ���,��ɾ����Ӧ������syntax��Ӧ,���һ������м����
���ɾ����ĳ��action,��ɾ����Ӧ��action-�м�����,�������ö�Ӧ���м����ָ��Ĭ��ֵ
���ɾ����ĳ��syntax_symbol,��ɾ����Ӧ������syntax��Ӧ,���һ��ն�Ӧ���м����
����actionʹ�õ��м����Ļ���,ʹ����������ķ�����ͬʱ����action��Ӧ�����ô���,������ô�������0,�����
���м����


���:
ʹ���м���������,IdAllocator������ͻ����м����
ʹ��hashset���������action,token�Լ�syntaxSymbol


*/



/*
ʹ��˵��:


*/










typedef enum ordkind{
  UN_LEGAL,   //���Ϸ�������,Ҳ�����쳣����
  DOC,    //ע������,����˵ע����
  ACTION_ADD,    //action��������
  ACTION_DEL,   //actionɾ������
  ACTION_CLS,   //action�����������
  SYMBOL_ADD, //���������������
  SYMBOL_DEL, //������ż�������
  SYMBOL_CLS, //��������������
  SYNTAX_SET, //�﷨��������������������,Ҳ����˵�﷨����ֻ�ܹ��޸�
  SYNTAX_DEFAULT, //�﷨����ȫ����������
  OUTPUT_ORD,   //�﷨����ȫ������,�Ա��ٴζ�ȡʹ��
  OUTPUT_TBL,   //���yjhc��ʹ�õ�grammar��
  ordkind_num       //ͨ��enum�Ľṹ,���һλ������OrdKindʹ��,�����ܹ���ʾOrdKind������
}OrdKind;

//�����ж�ȡ��,��ȡһ������,���Һ���ǰ���ո�,����ע�ͷ�����,���Ҷ�������ֹͣ,����û�г�������,��̬����ռ�
char* fgetOrd(FILE* fin);

//��������,����ǲ�����������򷵻�0,������ɹ��󷵻ط�0ֵ
int maintainOrd(char* ord);

//�ж���������
OrdKind ordKind(char* ord);



/*
���ִ�������ķ�ʽ
һ���������ʽ������
*/

int doc_run(FILE* fin,char* ord);

int action_add(FILE* fin,char* ord);

int action_del(FILE* fin,char* ord);





#endif