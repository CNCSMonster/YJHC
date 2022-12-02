#ifndef _GRAMMAR_TBL_GENERATOR_H
#define _GRAMMAR_TBL_GENERATOR_H

#include "grammar_tbl_generator.h"
#include "hashset.h"
#include "string_id.h"
#include "id_string.h"

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
};

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
*/

typedef enum ordkind{
  UN_LEGAL,   //不合法的命令,也就是异常命令
  DOC,    //注释命令,或者说注释行
  ACTION_ADD,    //action增加命令
  ACTION_DEL,   //action删除命令
  ACTION_CLS,   //action清除所有命令
  SYMBOL_ADD, //语义符号增加命令
  SYMBOL_DEL, //语义符号减少命令
  SYMBOL_CLS, //语义符号清除命令
  SYNTAX_SET, //语法分析动作个别设置命令,也就是说语法动作只能够修改
  SYNTAX_DEFAULT, //语法动作全局设置命令
  OUTPUT_ORD,   //语法命令全局输入,以便再次读取使用
  OUTPUT_TBL,   //输出yjhc中使用的grammar表
  ordkind_num       //通过enum的结构,最后一位不当作OrdKind使用,而是能够表示OrdKind的数量
}OrdKind;

//命令行读取器,读取一行命令,而且忽略前导空格,忽略注释符后面,而且读到换行停止,命令没有长度限制,动态分配空间
char* fgetOrd(FILE* fin);

//处理命令,如果是不合理的命令则返回0,否则处理成功后返回非0值
int maintainOrd(char* ord);

//判断命令类型
OrdKind ordKind(char* ord);



/*
各种处理命令的方式
一个处理命令方式函数表
*/

int doc_run(FILE* fin,char* ord);

int action_add(FILE* fin,char* ord);

int action_del(FILE* fin,char* ord);





#endif