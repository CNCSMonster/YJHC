#ifndef _GRAMMAR_TBL_GENERATOR_H
#define _GRAMMAR_TBL_GENERATOR_H

#include "grammar_tbl_generator.h"
#include "hashset.h"
#include "string_id.h"
#include "id_string.h"


//Ĭ�Ϲ����ļ�,������ʱ����ȡ����ļ�,�˳���ʱ���д������ļ�
#define DEFAULT_GTG_WORK_FILE "gtg.txt"



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
}block;

char* ord=NULL;
FILE* fin=NULL;
FILE* fout=NULL;




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

action add ����action,�����������������actionѡ��
Ĭ�ϻ��ȡ��������action����ֱ���������н���,
����ʹ��action add -n 3 //����ȡ����3��action
actionkind add ����actionkind   
Ҳ����ʹ��actionkind add -n N��NΪһ������,ʹ��ͬ��
token add  //����token,�����������������token����
symbol add //����symbol
syntax add //����syntax,Ҳ����ʹ��-n
help  //�����ʾ

����������:
action add
actionkind add
token add
symbol add
syntax add
����������Ĭ�ϻ�ʹ����һ�е�������Ϊ������������
���������������Լ��ϸ��Ӻ�׺[-n N]
����action add -n 4,��������ȡ����һ�п�ʼ��4���ַ�����Ϊ���ӵ�action��Щ�ַ���֮���ÿո������ַ�����(����ո���߻���)

��ʾ������:
help ��ʾ��Ϣ,��ʾʹ����Ϣ

�鿴����:
check actionkind  //�鿴actionkind�б�
check token //�鿴����token��Ϣ
check action  <actionkind> �鿴ĳ��actionkind������action��Ϣ
check action  ��鿴����action����Ϣ,��������actionkind
check symbol  //�鿴����symbol��Ϣ
check syntax  //�鿴����syntax��Ϣ
�����ں������-o�������Ϣ�����-o����ָ�����ļ���


����������:
exit �˳�����,��ȡ�������,�ѻ���������,�����˳�
gc ������������,����id,���տռ�(garbage collect)

�滻������:
replace
�����������ʹ�ý������������ַ����ֱ���ΪҪ�滻�ַ��������滻���ַ���
Ҳ���Լ���-n N��������,�����ȡ�����n���ַ�����replace����

ɾ������
del
ͬ�����Ժ������-n��ʹ��,���ʹ�ú����n���ַ�����ɾ������


*/

//�ж����������
typedef enum ordkind{
  NOT_DEFINE, //δ��������
  ACTION_ADD, //action����
  ACTIONKIND_ADD, //actionkind����
  ACTIONKIND_SETDEFAULT,  //����actionkind��default
  SET_NOT_DEFINE, //����δ�����ַ���
  TOKEN_ADD,
  SYMBOL_ADD,
  SYNTAX_ADD, //syntax��������
  DEL,    //ɾ��һ���ַ���
  REPLACE,    //�滻
  GC,     //������������
  EXIT,   //�˳�
  INIT,    //����
  HELP,   //��ʾ
  CHECK_ACTION_OFKIND,
  CHECK_ACTION_ALL,
  CHECK_ACTIONKIND,
  CHECK_SYMBOL,
  CHECK_SYNTAX,
  CHECK_TOKEN,
  CHECK_ALL
}OrdKind;





//��ʼ����������
void init();

//�����ж�ȡ��,��ȡһ������,���Һ���ǰ���ո�,����ע�ͷ�����,���Ҷ�������ֹͣ,����û�г�������,��̬����ռ�
char* fgetOrd(FILE* fin);

//��������,����ǲ�����������򷵻�0,������ɹ��󷵻ط�0ֵ
int maintainOrd(char* ord);

//�ж�ĳ���ַ����Ƿ��Ѿ�ʹ�ù���
int ifHasUsed(char* str);

//�ж���������
OrdKind ordKind(char* ord);



/*
���ִ�������ķ�ʽ
һ���������ʽ������
*/

int error();  //������ʾ,������û�ж���������ʱ�����������ʾ

int actionkind_setdefault();
int set_not_define();

int action_add();
int syntax_add();
int symbol_add();
int token_add();
int actionkind_add();

int help();
int del();
int replace();
int gtg_exit();
int gtg_init(); //��ʼ��gtg����


int check_action_ofkind();
int check_action_all();
int check_symbol();
int check_token();
int check_all();
int check_actionkind();
int check_syntax();



int (*executeOrds[])(void)={
  [NOT_DEFINE] error,
  [ACTIONKIND_SETDEFAULT] actionkind_setdefault,
  [SET_NOT_DEFINE] set_not_define,
  [ACTION_ADD] action_add,
  [SYMBOL_ADD] symbol_add,
  [SYNTAX_ADD] syntax_add,
  [TOKEN_ADD] token_add,
  [ACTIONKIND_ADD] actionkind_add,
  [DEL] del,
  [HELP] help,
  [INIT] gtg_init,
  [EXIT]  gtg_exit,
  [REPLACE] replace,
  [CHECK_ACTION_OFKIND] check_action_ofkind,
  [CHECK_ACTION_ALL] check_action_all,
  [CHECK_ACTIONKIND] check_actionkind,
  [CHECK_SYMBOL] check_symbol,
  [CHECK_SYNTAX] check_syntax,
  [CHECK_TOKEN] check_token,
  [CHECK_ALL] check_all
};






#endif