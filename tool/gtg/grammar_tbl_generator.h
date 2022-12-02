#ifndef _GRAMMAR_TBL_GENERATOR_H
#define _GRAMMAR_TBL_GENERATOR_H

#include "grammar_tbl_generator.h"
#include "hashset.h"
#include "string_id.h"

/*
映射关系分析:
由token


*/

struct ActionsEachKind{
  HSet actions;
  int actionKind;
  struct ActionsEachKind* next;
};



//设置命令器信息块
struct gtgBlock{
  //存放默认是错误值的字符串,必须是全大写的字符串
  char* default;
  //字符串id表
  StrIdTable strIds;
  //token表
  HSet tokens;  //记录token用的表,能够用来快速判断token是否存在,其实里面保存的是字符串对应的中间号码
  //存放actionKind的空间
  HSet actionKinds; //记录actionKind对应的中间号码
  //存放不同action的空间,能够根据actionKind来寻找匹配
  struct ActionsEachKind* actions;
  //存放syntax symbol的空间
  HSet symbols;

  
};


/*
要解决的问题有:
用什么保存action们?
用什么保存syntax们?
用什么保存token们?
要求action能够快速地增删改
syntax也要能够快速增删改
如何保存syntax表信息?

构思:
使用边列表道式保存syntax信息
比如对于token COMMA,以及对于栈顶symbol BLOCK_IN,有PrintAction NEWLINE
则保存为
action_kind:top_symbol,token,action的形式
如:
PrintAction:BLOCK_IN,COMMA,NEWLINE的形式
为了能够精简化表达,应该要把对应得字符串转为编号

整理:
Action,或者Token,或者SyntaxSymbol都是字符串
为了节省数据占用的内存，采用了给每个字符串编号,然后保存编号的方法
在内存中的编号
如果加入某个字符串,则给该字符串分配一个可用编号,然后在一个哈希表中
加入表项<key,val>=(str,id),其中str是该字符串,id为分配给该表项的编号,
同时需要另外一个哈希表,加入表项编
<key,val>=(id,str)
在内存中加入syntax语法的时候就保存编号,
在实际上output的法表的时候,就根据编号找到对应的字符串。
如果删除了某个字符串,则设置对应的编号为默认编号,默认号则会使用默认字符串

也就是经过两层映射

字符串(用户层)-->中间号码(内存中使用)-->实际字符串(用户层)
两个哈希表:
[字符串,中间号码]
[中间号码,实际字符串]

关于中间号码的回收:
如果删掉了某个token字符串,则删掉对应的所有syntax反应,并且回收它中间号码
如果删掉了某个action,则删掉对应的action-中间号码对,并且设置对应的中间号码指向默认值
如果删掉了某个syntax_symbol,则删掉对应的所有syntax反应,并且回收对应的中间号码
关于action使用的中间号码的回收,使用引入计数的方法，同时保存action对应的引用次数,如果引用次数等于0,则回收
该中间号码


解答:
使用中间代码分配器,IdAllocator来分配和回收中间代码
使用hashset来保存各种action,token以及syntaxSymbol


*/



/*
使用说明:


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