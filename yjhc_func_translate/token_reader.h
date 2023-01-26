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

//�����洢syntax���ŵ�ջ�Ľṹ��
struct syntaxSymbolStack{
  struct syntaxSymbolStack* next;
  enum syntax_symbol kind;
};   //ʲô��û�д�����


typedef struct token_buffer_node{
  struct token_buffer_node* last;
  struct token_buffer_node* next;
  struct token token;
}TBNode;


int ifSplitAfterAdd=0;  //����Ƿ����¼���token��ָ�����
ActionSet oldActionSet; //������¼֮ǰ��һЩ����


TBNode head={NULL,NULL,{0,NULL}};  //��������ͷ�ڵ�,ͷ�ڵ㲻����ֵ,ͷ�ڵ����һ���ڵ���Ϊ��Ԫ�ڵ�

TBNode* tail;   //���һ��ֵ�ڵ��λ�ã���û��ֵ�ڵ�ʱ������λ��head�ĵ�ַ

struct syntaxSymbolStack
ssStack={NULL,INIT};   //ջͷ�ڵ����init,����ɾ��

//token��ȡ����ȡtoken��Դ�ļ�
FILE* tb_fin;  


//ʹ��ģ��ǰ��init����
void init_token_reader(FILE* fin);

//����һ�����,ͨ������ָ�뷵�ضԿ�Ľ���������ж�,�Լ�����Ƿ�Ҫ���е��ж�
TBNode* readTokenSentence(ActionSet* actionSet);

//��token��(���ͱ��,�ַ�����ֵ)����ʽд���ļ�
void fput_tokenLine(FILE* fout,TBNode* tokens);

void fshow_tokenLine(FILE* fout,TBNode* tokens);

//�ϲ��ڵ�õ�һ���ڵ�,ָ���м����,���ϲ��Ľڵ�ռ�
TBNode* connect_tokens(TBNode* head,TokenKind kind,const char* sepStr);

//��ӡtokenLine�������Ϣ
void show_tokenLine(TBNode* tokens);

//�ͷ����ռ�,��������ڵ��Լ�����ڵ�
void del_tokenLine(TBNode* tokens);

//�ͷ�ʣ����������,�÷�����������������쳣������ʱ��ʹ��
void release_token_reader();

//����һ����̬����ռ�Ĺ���TBNode*
TBNode* getTBNode(const char* str,TokenKind kind);


#endif