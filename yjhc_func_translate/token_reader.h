#ifndef _TOKEN_READER_H
#define _TOKEN_READER_H

#include "../yjhc_preProcess/token_kind.h"
#include "yjhc_grammar.h"
#include "../yjhc_preProcess/token.h"
#include "action.h"
#include "syntax_symbol.h"
#include <stdio.h>
#include <stdlib.h>

int token_reader_blocks=0;

//用来存储syntax符号的栈的结构体
struct syntaxSymbolStack{
  struct syntaxSymbolStack* next;
  enum syntax_symbol kind;
};   //什么都没有存的情况


typedef struct token_buffer_node{
  struct token_buffer_node* last;
  struct token_buffer_node* next;
  struct token token;
}TBNode;


int ifSplitAfterAdd=0;  //标记是否在新加入token后分割的情况
ActionSet oldActionSet; //用来记录之前的一些动作


TBNode head={NULL,NULL,{0,NULL}};  //缓冲区地头节点,头节点不保存值,头节点的下一个节点作为首元节点

TBNode* tail;   //最后一个值节点的位置，当没有值节点时，它定位在head的地址

struct syntaxSymbolStack
ssStack={NULL,INIT};   //栈头节点就是init,不可删除

//token读取器读取token的源文件
FILE* tb_fin;  


//使用模块前的init方法
void init_token_reader(FILE* fin);

//返回一条语句,通过函数指针返回对块的进出情况的判断,以及输出是否要换行的判断
TBNode* readTokenSentence(ActionSet* actionSet);

//以token的(类型编号,字符字面值)的形式写入文件
void fput_tokenLine(FILE* fout,TBNode* tokens);

void fshow_tokenLine(FILE* fout,TBNode* tokens);

//合并节点得到一个节点,指定中间符号,被合并的节点空间
TBNode* connect_tokens(TBNode* head,TokenKind kind,const char* sepStr);

//打印tokenLine的语句信息
void show_tokenLine(TBNode* tokens);

//释放语句空间,包括这个节点以及后面节点
void del_tokenLine(TBNode* tokens);

//释放剩余所有内容,该方法可以在语义分析异常结束的时候使用
void release_token_reader();

//产生一个动态分配空间的孤立TBNode*
TBNode* getTBNode(const char* str,TokenKind kind);


#endif