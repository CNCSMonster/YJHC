#include "grammar_tbl_generator.h"

//该程序目的,实现一个能够用来生成yjhc用的语法文件的编辑器

//初始化程序数据,
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

//命令行读取器,读取一行命令,而且忽略前导空格,忽略注释符后面,而且读到换行停止,命令没有长度限制,动态分配空间
char* fgetOrd(FILE* fin){

}

//处理命令,如果是不合理的命令则返回0,否则处理成功后返回非0值
int maintainOrd(char* ord){
  
}

//判断某个字符串是否已经使用过了
int ifHasUsed(char* str);

//判断命令类型
OrdKind ordKind(char* ord);



/*
各种处理命令的方式
一个处理命令方式函数表
*/

int error();  //错误提示,当输入没有定义的命令的时候给出错误提示

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
int gtg_init(); //初始化gtg数据


int check_action_ofkind();
int check_action_all();
int check_symbol();
int check_token();
int check_all();
int check_actionkind();
int check_syntax();