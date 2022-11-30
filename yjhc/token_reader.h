#ifndef _TOKEN_READER_H
#define _TOKEN_READER_H

#include "token_kind.h"
#include "yjhc_grammar.h"
#include "token.h"
#include <stdio.h>
#include <stdlib.h>






//返回关于翻译的指导信息,比如是否要翻译函数
//为什么用的是struct类型而不是enum类型,是因为考虑到后面扩展语法的时候可能返回的
//翻译指导动作是多方面的而不仅仅是是否翻译函数
struct TranslateAction{
  enum PrintAction pa;
  enum FuncTranslateAction fta;
  enum BlockAction ba;
};



struct lastSentenceKindStack{
  struct lastSentenceKindStack* next;
  enum sentence_kind kind;
};   //什么都没有存的情况


typedef struct token_buffer_node{
  struct token_buffer_node* last;
  struct token_buffer_node* next;
  struct token token;
}TBNode;



TBNode head={NULL,NULL,{0,NULL}};  //缓冲区地头节点,头节点不保存值,头节点的下一个节点作为首元节点


struct lastSentenceKindStack
lskStack={NULL,INIT};   //栈头节点就是init,不可删除

FILE* tb_fin;  








//使用模块前的init方法
void init_token_reader(FILE* fin);

//返回一条语句,通过函数指针返回对块的进出情况的判断,以及输出是否要换行的判断
TBNode* token_readsentence(enum BlockAction* blockAction,enum PrintAction* printAction,struct TranslateAction* translateAction);

//释放token链表空间
void del_tokenLine(TBNode* tokens);

//释放剩余所有内容,该方法可以在语义分析异常结束的时候使用
void del_rest();


#endif