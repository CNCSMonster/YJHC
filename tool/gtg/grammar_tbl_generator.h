#ifndef _GRAMMAR_TBL_GENERATOR_H
#define _GRAMMAR_TBL_GENERATOR_H

#include "grammar_tbl_generator.h"
#include "hashset.h"
#include "string_id.h"
#include "id_string.h"


//默认工作文件,启动的时候会读取这个文件,退出的时候会写入这个文件
#define DEFAULT_GTG_WORK_FILE "gtg.txt"



/*
映射关系分析:
由token


*/

struct ActionsEachKind{
  HSet actions;
  int defaultAction;  //默认用的action，如果编号为-1的话,说明使用全局未定义符号
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
  //存放不同action的空间,能够根据actionKind来寻找匹配
  struct ActionsEachKind* actions;

  //id分配器
  struct IdAllocator idAllocator; //id分配器

  //存放syntax symbol的空间
  HSet symbols;
  //存放syntax
  struct syntax_line syntaxs; //syntax的头节点,从下一个节点开始才是有效syntax_line语句
}block;

char* ord=NULL;
FILE* fin=NULL;
FILE* fout=NULL;




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

/*
使用说明:
命令格式

action add 增加action,输入该命令后进入增加action选项
默认会读取当行所有action增加直到遇到换行结束,
或者使用action add -n 3 //则会读取后面3个action
actionkind add 增加actionkind   
也可以使用actionkind add -n N，N为一个数字,使用同上
token add  //增加token,输入该命令后进入增加token命令
symbol add //增加symbol
syntax add //增加syntax,也可以使用-n
help  //输出提示

增加型命令:
action add
actionkind add
token add
symbol add
syntax add
增加型命令默认会使用下一行的内容作为所有增加内容
增加型命令后面可以加上附加后缀[-n N]
比如action add -n 4,该命令会读取从下一行开始的4个字符串作为增加的action这些字符串之间用空格类型字符隔开(比如空格或者换行)

提示型命令:
help 提示信息,提示使用信息

查看命令:
check actionkind  //查看actionkind列表
check token //查看所有token信息
check action  <actionkind> 查看某个actionkind下所有action信息
check action  则查看所有action的信息,包括所有actionkind
check symbol  //查看所有symbol信息
check syntax  //查看所有syntax信息
可以在后面加入-o命令把信息输出到-o后面指定的文件中


控制型命令:
exit 退出命令,获取该命令后,把缓冲结果保存,并且退出
gc 垃圾回收命令,回收id,回收空间(garbage collect)

替换型命令:
replace
输入该命令后会使用接下来的两个字符串分别作为要替换字符串，和替换后字符串
也可以加入-n N增加属性,则会提取后面的n对字符串来replace操作

删除命令
del
同样可以后面加入-n来使用,则会使用后面的n个字符串做删除操作


*/

//判断命令的类型
typedef enum ordkind{
  NOT_DEFINE, //未定义命令
  ACTION_ADD, //action命令
  ACTIONKIND_ADD, //actionkind增加
  ACTIONKIND_SETDEFAULT,  //设置actionkind的default
  SET_NOT_DEFINE, //设置未定义字符串
  TOKEN_ADD,
  SYMBOL_ADD,
  SYNTAX_ADD, //syntax增加命令
  DEL,    //删除一个字符串
  REPLACE,    //替换
  GC,     //启动垃圾回收
  EXIT,   //退出
  INIT,    //重启
  HELP,   //提示
  CHECK_ACTION_OFKIND,
  CHECK_ACTION_ALL,
  CHECK_ACTIONKIND,
  CHECK_SYMBOL,
  CHECK_SYNTAX,
  CHECK_TOKEN,
  CHECK_ALL
}OrdKind;





//初始化程序数据
void init();

//命令行读取器,读取一行命令,而且忽略前导空格,忽略注释符后面,而且读到换行停止,命令没有长度限制,动态分配空间
char* fgetOrd(FILE* fin);

//处理命令,如果是不合理的命令则返回0,否则处理成功后返回非0值
int maintainOrd(char* ord);

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