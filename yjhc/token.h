#ifndef _TOKEN_H
#define _TOKEN_H

#include <stdio.h>
#include <stdlib.h>
#include "token_kind.h"
#include "mystring.h"


typedef struct token{
  TokenKind kind;
  char* val;  //ÿ��token���Ŷ�̬�ڴ��������ַ�����ֵ�Խ�ʡ�ڴ�
}Token;


//���ļ��ж�ȡһ��token,��̬����ռ�
struct token getToken(FILE* fin);

//������token������������µ�token,ʹ��sep�ַ�����Ϊ���ӵ�����
struct token connectToken(Token token1,Token token2,TokenKind newKind,char* sep);


//�ͷŸ�token����Ŀռ�
void delToken(Token token);


//token��ʾ����,�������ͺ��ַ�����ֵ�������token��Ϣ
int printToken(Token token);

#endif