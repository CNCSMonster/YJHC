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


//���徭��Ҫʹ�õ��ر�token����,���Ƕ�̬����ռ��token,���ǹ̶�token,����delToken���տռ�
Token leftBraceToken={LEFT_BRACE,"{"};      
Token rightBraceToken={RIGHT_BRACE,"}"};    





//���ļ��ж�ȡһ��token,��̬����ռ�
struct token getToken(FILE* fin);

//���ļ����˻ض�ȡ��һ��token
int ungetToken(FILE* fin,Token token);

//������token������������µ�token,ʹ��sep�ַ�����Ϊ���ӵ�����
struct token connectToken(Token token1,Token token2,TokenKind newKind,char* sep);

//��ȡһ��self���͵�token
Token getSelfToken();


//�ͷŸ�token����Ŀռ�
void delToken(Token token);


//token��ʾ����,�������ͺ��ַ�����ֵ�������token��Ϣ
int printToken(Token token);

//���ļ���д��token�ķ���ֵ���ַ�����ֵ
int fputToken(Token token,FILE* fout);


#endif