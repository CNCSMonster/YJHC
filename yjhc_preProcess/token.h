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


//定义经常要使用的特别token集合,不是动态分配空间的token,而是固定token,不用delToken回收空间
Token leftBraceToken={LEFT_BRACE,"{"};      
Token rightBraceToken={RIGHT_BRACE,"}"};    





//从文件中读取一个token,动态分配空间
struct token getToken(FILE* fin);

//往文件中退回读取的一个token
int ungetToken(FILE* fin,Token token);

//把两个token连接起来组成新的token,使用sep字符串作为连接点内容
struct token connectToken(Token token1,Token token2,TokenKind newKind,char* sep);

//获取一个self类型的token
Token getSelfToken();


//释放给token分配的空间
void delToken(Token token);


//token显示工具,根据类型和字符字面值换行输出token信息
int printToken(Token token);

//往文件中写回token的符号值和字符字面值
int fputToken(Token token,FILE* fout);

//判断token是数字常量token还是字符常量token
int isConstStrToken(Token token);

//判断是否是整数token
int isConstIntToken(Token token);

//判断是否是常小鼠token
int isConstDecimalToken(Token token);


#endif