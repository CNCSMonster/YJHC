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


/*
ӳ���ϵ����:


*/

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
  CHECK_SYNTAX_OFKIND,
  CHECK_SYNTAXS,
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
int maintainOrd(char* ord);

//�ж�ĳ���ַ����Ƿ��Ѿ�ʹ�ù���,����Ѿ�ʹ�ù��˷��ط�0ֵ,���򷵻�0
int isUsedStr(char* str);

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
int gc();
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
int check_syntax_ofkind();
int check_syntaxs();

//�������

//����ܹ����е��Ѿ�ִ�е����ݵ�gtg�ű��ļ�
int output_orders();

//����ܹ�����yjhc�е�grammar�﷨�ļ�
int output_grammar();



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
  [GC] gc,
  [EXIT]  gtg_exit,
  [REPLACE] replace,
  [CHECK_ACTION_OFKIND] check_action_ofkind,
  [CHECK_ACTION_ALL] check_action_all,
  [CHECK_ACTIONKIND] check_actionkind,
  [CHECK_SYMBOL] check_symbol,
  [CHECK_SYNTAX_OFKIND] check_syntax_ofkind,
  [CHECK_SYNTAXS] check_syntaxs,
  [CHECK_TOKEN] check_token,
  [CHECK_ALL] check_all,
  [OUTPUT_GRAMMAR] output_grammar,
  [OUTPUT_ORDERS] output_orders
};






#endif