#include "grammar_tbl_generator.h"

//�ó���Ŀ��,ʵ��һ���ܹ���������yjhc�õ��﷨�ļ��ı༭��

//��ʼ����������,
void init(){
  block.actionKinds=hashset_cre(sizeof(int));
  block.actions=NULL;
  block.tokens=hashset_cre(sizeof(int));
  block.symbols=hashset_cre(sizeof(int));
  block.not_define=NULL;
  block.strIds=getStrIdTable();
  block.idAllocator=getIdAllocator();
  block.syntaxs.next=NULL;
  
  ord=NULL;
  fin=fopen(DEFAULT_GTG_WORK_FILE,"r");
  if(fin!=NULL){
    while((ord=fgetOrd(fin))!=NULL){
      OrdKind kind=ordkind(ord);
      executeOrds[kind]();
    }
    fclose(fin);
  }
  fin=NULL;
  fin=stdin;
  fout=NULL;
}

//�����ж�ȡ��,��ȡһ������,���Һ���ǰ���ո�,����ע�ͷ�����,���Ҷ�������ֹͣ,����û�г�������,��̬����ռ�
char* fgetOrd(FILE* fin){

}

//��������,����ǲ�����������򷵻�0,������ɹ��󷵻ط�0ֵ
int maintainOrd(char* ord){
  
}

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