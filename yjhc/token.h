#ifndef _TOKEN_H
#define _TOKEN_H

#include <stdio.h>
#include <stdlib.h>
#include "token_kind.h"
#include "mystring.h"


typedef struct token{
  TokenKind kind;
  char* val;  //每个token安排动态内存来保存字符字面值以节省内存
}Token;


//从文件中读取一个token,动态分配空间
struct token getToken(FILE* fin);

//把两个token连接起来组成新的token,使用sep字符串作为连接点内容
struct token connectToken(Token token1,Token token2,TokenKind newKind,char* sep);


//释放给token分配的空间
void delToken(Token token);


//token显示工具,根据类型和字符字面值换行输出token信息
int printToken(Token token);

#endif