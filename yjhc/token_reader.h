#ifndef _TOKEN_READER_H
#define _TOKEN_READER_H

#include "token_kind.h"
#include "yjhc_grammar.h"
#include "token.h"
#include <stdio.h>
#include <stdlib.h>






//���ع��ڷ����ָ����Ϣ,�����Ƿ�Ҫ���뺯��
//Ϊʲô�õ���struct���Ͷ�����enum����,����Ϊ���ǵ�������չ�﷨��ʱ����ܷ��ص�
//����ָ�������Ƕ෽��Ķ����������Ƿ��뺯��
struct TranslateAction{
  enum PrintAction pa;
  enum FuncTranslateAction fta;
  enum BlockAction ba;
};



struct lastSentenceKindStack{
  struct lastSentenceKindStack* next;
  enum sentence_kind kind;
};   //ʲô��û�д�����


typedef struct token_buffer_node{
  struct token_buffer_node* last;
  struct token_buffer_node* next;
  struct token token;
}TBNode;



TBNode head={NULL,NULL,{0,NULL}};  //��������ͷ�ڵ�,ͷ�ڵ㲻����ֵ,ͷ�ڵ����һ���ڵ���Ϊ��Ԫ�ڵ�


struct lastSentenceKindStack
lskStack={NULL,INIT};   //ջͷ�ڵ����init,����ɾ��

FILE* tb_fin;  








//ʹ��ģ��ǰ��init����
void init_token_reader(FILE* fin);

//����һ�����,ͨ������ָ�뷵�ضԿ�Ľ���������ж�,�Լ�����Ƿ�Ҫ���е��ж�
TBNode* token_readsentence(enum BlockAction* blockAction,enum PrintAction* printAction,struct TranslateAction* translateAction);

//�ͷ�token����ռ�
void del_tokenLine(TBNode* tokens);

//�ͷ�ʣ����������,�÷�����������������쳣������ʱ��ʹ��
void del_rest();


#endif