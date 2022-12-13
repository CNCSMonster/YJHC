#ifndef _GRAMMAR_TBL_GENERATOR_H
#define _GRAMMAR_TBL_GENERATOR_H


#include "hashset.h"
#include "mstr.h"
#include "string_id.h"
#include "id_string.h"


//Ĭ�Ϲ����ļ�,������ʱ����ȡ����ļ�,�˳���ʱ���д������ļ�
#define DEFAULT_GTG_WORK_FILE "gtg.txt"

//Ĭ����ʾ���ļ�,help������������ļ�������ı�
#define HELP_FILE "help.txt"   

#define NOT_DEFINE_ID (-1)

#define DEFAULT_NOTDEFINE_STRING "NOT_DEFINE"

/*
ӳ���ϵ����:


*/

//�﷨��
struct syntax_line{
  int symbol;
  int token;
  int action;
  struct syntax_line* next;
};


//����
struct tblBlock{
  HSet actions;
  int defaultAction;  //Ĭ���õ�action��������Ϊ-1�Ļ�,˵��ʹ��ȫ��δ�������
  int actionKind;
  int syntax_num;     //���ӵ��﷨�е�����
  struct syntax_line syntaxs_head;  //��actionkind�µ�syntaxs��
  struct tblBlock* next;
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
  //��Ų�ͬactionkind��Ӧ�ı�������ÿ���������default_action,actions,syntaxs��Ϣ
  struct tblBlock actions_head;

  //id������
  struct IdAllocator idAllocator; //id������

  //���syntax symbol�Ŀռ�
  HSet symbols;
}block;

char* ord=NULL;
FILE* fin=NULL;
FILE* fout=NULL;

int n_each_line_gtg=6;    //ÿ�д�ӡ���ַ�����������

int n_gtg;  //����-n����������ȡ���,����������-n��������������Իᱣ��������(����Ǹ���Ч������)

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





//�ж����������
typedef enum ordkind{
  NOT_DEFINE_ORD, //δ��������
  ACTION_ADD, //action����
  ACTIONKIND_ADD, //actionkind����
  SET_DEFAULT_ACTION,  //����actionkind��default
  SET_NOT_DEFINE, //����δ�����ַ���
  TOKEN_ADD,
  SYMBOL_ADD,
  SYNTAX_ADD, //syntax������������
  DEL,    //ɾ��һ���ַ���
  RUN,
  REPLACE,    //�滻
  GC,     //������������
  EXIT,   //�˳�
  CLS,  //clear screen
  INIT,    //����
  HELP,   //��ʾ
  CHECK_ACTION_OFKIND,
  CHECK_ACTION_ALL,
  CHECK_DEFAULT_ACTION,
  CHECK_ACTIONKIND,
  CHECK_SYMBOL,
  CHECK_SYNTAX_OFKIND,
  CHECK_SYNTAX_ALL,
  CHECK_TOKEN,
  CHECK_ALL,
  OUTPUT_ORDERS,
  OUTPUT_GRAMMAR
}OrdKind;





//��ʼ����������
void init();

//�ͷ��﷨�б�,�ͷųɹ������ͷŵ��﷨�еĸ���,�ͷ�ʧ�ܷ��ظ���
int delSyntaxs(struct syntax_line* syntaxHead);



//�ͷ��﷨���붯��ָ����,���쳣���ط�0ֵ,���쳣����0
int delTblBlocks(struct tblBlock* tbls_head);

//�����ж�ȡ��,��ȡһ������,���Һ���ǰ���ո�,����ע�ͷ�����,���Ҷ�������ֹͣ,����û�г�������,��̬����ռ�
char* fgetOrd(FILE* fin);

//��ȡһ������,����ǰ���ո�͵���ע���Լ�
char* fgetWord(FILE* fin);

//����ids��ӡ�ַ���,������ӡ���ط�0ֵ,��ӡ�쳣(�������δע���id)����0
int showStringsByIds(int* ids,int num);


//��ָ������ȡ-n��Ϣ(��ȡ-nָ���Ĳ���N,һ������0������),��ȡ�ɹ����ط�0ֵ,��ȡʧ�ܷ���0
int extractN(char* line,int* returnN);

//��ָ���ж�ȡ-o����,��ȡ�ɹ����ط�0ֵ����ȡʧ�ܷ���0
int extractO(char* line,char* returnPath);

//��������,����ǲ�����������򷵻�0,������ɹ��󷵻ط�0ֵ
int maintainOrd();

//�ж�ĳ���ַ����Ƿ��Ѿ�ʹ�ù���,����Ѿ�ʹ�ù��˷��ط�0ֵ,���򷵻�0
int isUsedStr(char* str);

//�ж���������
OrdKind ordKind(char* ord);



/*
���ִ�������ķ�ʽ
һ���������ʽ������
*/

int error();  //������ʾ,������û�ж���������ʱ�����������ʾ

int set_defaultAction();
int set_not_define();

int action_add();
int syntax_add();
int symbol_add();
int token_add();
int actionkind_add();

int help();
int run();  //����ָ���ļ��Ĵ���,�����н�������ָ��λ��
int gc();
int cls();
int del();
int replace();
int gtg_exit();
int gtg_init(); //��ʼ��gtg����


int check_actions_ofkind();
int check_actions_all();
int check_default_action(); //�鿴ָ���������͵�Ĭ�϶���
int check_symbol();
int check_token();
int check_all();
int check_actionkind();
int check_syntaxs_ofkind();
int check_syntaxs_all();

//�������

//����ܹ����е��Ѿ�ִ�е����ݵ�gtg�ű��ļ�
int output_orders();

//����ܹ�����yjhc�е�grammar�﷨�ļ�
int output_grammar();



int (*executeOrds[])(void)={
  [NOT_DEFINE_ORD] error,
  [SET_DEFAULT_ACTION] set_defaultAction,
  [SET_NOT_DEFINE] set_not_define,
  [ACTION_ADD] action_add,
  [SYMBOL_ADD] symbol_add,
  [SYNTAX_ADD] syntax_add,
  [TOKEN_ADD] token_add,
  [ACTIONKIND_ADD] actionkind_add,
  [DEL] del,
  [RUN] run,  //TODO
  [CLS] cls,    //�����Ļ֮ǰ��ʾ������
  [HELP] help,
  [INIT] gtg_init,
  [GC] gc,
  [EXIT]  gtg_exit,
  [REPLACE] replace,
  [CHECK_ACTION_OFKIND] check_actions_ofkind,
  [CHECK_ACTION_ALL] check_actions_all,
  [CHECK_ACTIONKIND] check_actionkind,
  [CHECK_DEFAULT_ACTION] check_default_action,
  [CHECK_SYMBOL] check_symbol,
  [CHECK_SYNTAX_OFKIND] check_syntaxs_ofkind,
  [CHECK_SYNTAX_ALL] check_syntaxs_all,
  [CHECK_TOKEN] check_token,
  [CHECK_ALL] check_all,
  [OUTPUT_GRAMMAR] output_grammar,
  [OUTPUT_ORDERS] output_orders
};


//����Ϣ����ɾ���ַ���,ɾ���ɹ����ط�0ֵ,ɾ��ʧ�ܷ���0
int gtg_delString(char* tmp);

//�����滻����������ַ������滻,�滻�ɹ����ط�0ֵ,�滻�쳣����0
int gtg_replaceString(char* input);

//������м���һ���﷨���������
int syntax_line_add(char* toAdd,struct tblBlock* tmpTbl);

int gtg_showTbl(struct tblBlock *tmpTbl);

int gtg_showSyntaxs(struct tblBlock* tmpTbl,char* actionkind);

//��ȡ��Ӧactionkind�ı�
struct tblBlock* getTbl(char* actionkind);

#endif