#ifndef _GRAMMAR_TBL_GENERATOR_H
#define _GRAMMAR_TBL_GENERATOR_H


#include "hashset.h"
#include "mstr.h"
#include "string_id.h"
#include "id_string.h"


//默认工作文件,启动的时候会读取这个文件,退出的时候会写入这个文件
#define DEFAULT_GTG_WORK_FILE "gtg.txt"

//默认提示用文件,help命令会输出这个文件里面的文本
#define HELP_FILE "help.txt"   

#define NOT_DEFINE_ID (-1)

#define DEFAULT_NOTDEFINE_STRING "NOT_DEFINE"

/*
映射关系分析:


*/

//语法行
struct syntax_line{
  int symbol;
  int token;
  int action;
  struct syntax_line* next;
};


//表格块
struct tblBlock{
  HSet actions;
  int defaultAction;  //默认用的action，如果编号为-1的话,说明使用全局未定义符号
  int actionKind;
  int syntax_num;     //增加的语法行的数量
  struct syntax_line syntaxs_head;  //该actionkind下的syntaxs们
  struct tblBlock* next;
};


//设置命令器信息块
struct gtgBlock{
  //存放默认值字符串(对应是负数,表示notdefine),必须是全大写的字符串
  char* not_define;
  //字符串id表
  StrIdTable strIds;
  //token表
  HSet tokens;  //记录token用的表,能够用来快速判断token是否存在,其实里面保存的是字符串对应的中间号码
  //存放actionKind的空间
  HSet actionKinds; //记录actionKind对应的中间号码
  //存放不同actionkind对应的表格的链表每个表格内有default_action,actions,syntaxs信息
  struct tblBlock actions_head;

  //id分配器
  struct IdAllocator idAllocator; //id分配器

  //存放syntax symbol的空间
  HSet symbols;
}block;

char* ord=NULL;
FILE* fin=NULL;
FILE* fout=NULL;

int n_each_line_gtg=6;    //每行打印的字符串个数限制

int n_gtg;  //命令-n附加属性提取结果,如果命令附加了-n则，输入的数字属性会保存在这里(如果是个有效的正数)

/*
对gtgBlock的说明:
1.
not_define保存默认的未定义字符串
比如not_define的默认值是NULL,也就是没有经过人定义
结果输出表格的时候,会生成一句:
#define NOT_DEFINE (-1)
我们可以使用not_define ggb 命令设置not_define="ggb"
结果输出表格的时候,会生成一句:
#define NOT_DEFINE (-1)

2.tokens,symbols,actionKinds
tokens保存的是token字符串分配的id,能够快速查找
symbols保存的是syntax symbols字符串分配的id
actionKinds同理保存的是ActionKind字符串分配的id

3.strIdTable
字符串Id表,保存<key,val>=<str,id>
能够快速查找字符串对应的id,用来保存字符串与它分配的id的对应关系

4.idAllocator
id分配器,用来对id进行分配,
能够给新字符串分配id
能够根据id查找对应的字符串
能够自动回收id

5.actions
action链表。
每个节点记录了一个actionKind下的所有action以及该actionKind的默认action
这样删除actionKind的时候能够快速删除掉一个action的所有内容


6.syntax_line
以链表形式保存的语法分析动作命令行
每行保存一个
ActionKind:symbol,token,action四元组
里面的每个值都保存的是对应字符串分配到的id
*/





//判断命令的类型
typedef enum ordkind{
  NOT_DEFINE_ORD, //未定义命令
  ACTION_ADD, //action命令
  ACTIONKIND_ADD, //actionkind增加
  SET_DEFAULT_ACTION,  //设置actionkind的default
  SET_NOT_DEFINE, //设置未定义字符串
  TOKEN_ADD,
  SYMBOL_ADD,
  SYNTAX_ADD, //syntax根据类型输入
  DEL,    //删除一个字符串
  RUN,
  REPLACE,    //替换
  GC,     //启动垃圾回收
  EXIT,   //退出
  CLS,  //clear screen
  INIT,    //重启
  HELP,   //提示
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





//初始化程序数据
void init();

//释放语法列表,释放成功返回释放的语法行的个数,释放失败返回负数
int delSyntaxs(struct syntax_line* syntaxHead);



//释放语法翻译动作指导表,无异常返回非0值,有异常返回0
int delTblBlocks(struct tblBlock* tbls_head);

//命令行读取器,读取一行命令,而且忽略前导空格,忽略注释符后面,而且读到换行停止,命令没有长度限制,动态分配空间
char* fgetOrd(FILE* fin);

//读取一个单词,忽略前导空格和单行注释以及
char* fgetWord(FILE* fin);

//根据ids打印字符串,正常打印返回非0值,打印异常(比如出现未注册的id)返回0
int showStringsByIds(int* ids,int num);


//从指令中提取-n信息(读取-n指定的参数N,一个大于0的整数),读取成功返回非0值,读取失败返回0
int extractN(char* line,int* returnN);

//从指令中读取-o属性,提取成功返回非0值，提取失败返回0
int extractO(char* line,char* returnPath);

//处理命令,如果是不合理的命令则返回0,否则处理成功后返回非0值
int maintainOrd();

//判断某个字符串是否已经使用过了,如果已经使用过了返回非0值,否则返回0
int isUsedStr(char* str);

//判断命令类型
OrdKind ordKind(char* ord);



/*
各种处理命令的方式
一个处理命令方式函数表
*/

int error();  //错误提示,当输入没有定义的命令的时候给出错误提示

int set_defaultAction();
int set_not_define();

int action_add();
int syntax_add();
int symbol_add();
int token_add();
int actionkind_add();

int help();
int run();  //运行指定文件的代码,把运行结果输出到指定位置
int gc();
int cls();
int del();
int replace();
int gtg_exit();
int gtg_init(); //初始化gtg数据


int check_actions_ofkind();
int check_actions_all();
int check_default_action(); //查看指定动作类型的默认动作
int check_symbol();
int check_token();
int check_all();
int check_actionkind();
int check_syntaxs_ofkind();
int check_syntaxs_all();

//输出命令

//输出能够运行的已经执行的数据的gtg脚本文件
int output_orders();

//输出能够用在yjhc中的grammar语法文件
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
  [CLS] cls,    //清空屏幕之前显示的内容
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


//从信息块中删掉字符串,删除成功返回非0值,删除失败返回0
int gtg_delString(char* tmp);

//根据替换输入来完成字符串的替换,替换成功返回非0值,替换异常返回0
int gtg_replaceString(char* input);

//往表块中加入一条语法分析表表项
int syntax_line_add(char* toAdd,struct tblBlock* tmpTbl);

int gtg_showTbl(struct tblBlock *tmpTbl);

int gtg_showSyntaxs(struct tblBlock* tmpTbl,char* actionkind);

//获取对应actionkind的表
struct tblBlock* getTbl(char* actionkind);

#endif