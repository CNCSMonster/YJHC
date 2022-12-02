#ifndef _GRAMMAR_TBL_GENERATOR_H
#define _GRAMMAR_TBL_GENERATOR_H

#include "grammar_tbl_generator.h"
#include "hashset.h"
#include "string_id.h"
#include "id_string.h"

/*
ӳ���ϵ����:
��token


*/

struct ActionsEachKind{
  HSet actions;
  int defaultAction;  //Ĭ���õ�action��������Ϊ-1�Ļ�,˵��ʹ��ȫ��δ�������
  int actionKind;
  struct ActionsEachKind* next;
};


struct syntax_line{
  int actionKind;
  int symbol;
  int token;
  int action;
  struct syntax_line* next;
};



//������������Ϣ��
struct gtgBlock{
  //���Ĭ��ֵ�ַ���(��Ӧ�Ǹ���,��ʾnotdefine),������ȫ��д���ַ���
  char* not_define;
  //�ַ���id��
  StrIdTable strIds;
  //token��
  HSet tokens;  //��¼token�õı�,�ܹ����������ж�token�Ƿ����,��ʵ���汣������ַ�����Ӧ���м����
  //���actionKind�Ŀռ�
  HSet actionKinds; //��¼actionKind��Ӧ���м����
  //��Ų�ͬaction�Ŀռ�,�ܹ�����actionKind��Ѱ��ƥ��
  struct ActionsEachKind* actions;

  //id������
  struct IdAllocator idAllocator; //id������

  //���syntax symbol�Ŀռ�
  HSet symbols;
  //���syntax
  struct syntax_line syntaxs; //syntax��ͷ�ڵ�,����һ���ڵ㿪ʼ������Чsyntax_line���
};

/*
��gtgBlock��˵��:
1.
not_define����Ĭ�ϵ�δ�����ַ���
����not_define��Ĭ��ֵ��NULL,Ҳ����û�о����˶���
����������ʱ��,������һ��:
#define NOT_DEFINE (-1)
���ǿ���ʹ��not_define ggb ��������not_define="ggb"
����������ʱ��,������һ��:
#define NOT_DEFINE (-1)

2.tokens,symbols,actionKinds
tokens�������token�ַ��������id,�ܹ����ٲ���
symbols�������syntax symbols�ַ��������id
actionKindsͬ�������ActionKind�ַ��������id

3.strIdTable
�ַ���Id��,����<key,val>=<str,id>
�ܹ����ٲ����ַ�����Ӧ��id,���������ַ������������id�Ķ�Ӧ��ϵ

4.idAllocator
id������,������id���з���,
�ܹ������ַ�������id
�ܹ�����id���Ҷ�Ӧ���ַ���
�ܹ��Զ�����id

5.actions
action����
ÿ���ڵ��¼��һ��actionKind�µ�����action�Լ���actionKind��Ĭ��action
����ɾ��actionKind��ʱ���ܹ�����ɾ����һ��action����������


6.syntax_line
��������ʽ������﷨��������������
ÿ�б���һ��
ActionKind:symbol,token,action��Ԫ��
�����ÿ��ֵ��������Ƕ�Ӧ�ַ������䵽��id

*/



/*
ʹ��˵��:
�����ʽ
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