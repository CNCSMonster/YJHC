#ifndef _TOKEN_READER_H
#define _TOKEN_READER_H

#include "token_kind.h"
#include "token.h"
#include <stdio.h>
#include <stdlib.h>

//����������ֿ���
enum BlockAction{
  MOVEINTO, //�����µĿ�
  MOVEOUT,  //�뿪��
  STAY  //�����ڿ���
};

//�Ƿ��е����ֿ���
enum PrintAction{
  NEWLINE,  //����
  KEEPLINE  //������
};

//���ع��ڷ����ָ����Ϣ,�����Ƿ�Ҫ���뺯��
//Ϊʲô�õ���struct���Ͷ�����enum����,����Ϊ���ǵ�������չ�﷨��ʱ����ܷ��ص�
//����ָ�������Ƕ෽��Ķ����������Ƿ��뺯��
struct TranslateAction{
  int ifTranslateFunc;      //�Ƿ���к�������
};

/*
��ģ����Ҫ��������ʽ���;ֲ��컮���Լ�����ָ��
���ص���һ���������Լ���Ӧ�Ŀ鶯���ʹ�ӡ����
�Ƿ�����飬�Ƿ�Ҫ���д�ӡ,�Ƿ�Ҫ���������(����for(���÷���,ֱ�Ӵ�ӡ����),����˵
��Ӧ���Ƿ����Ƿ�����µľֲ���ķ���token_reader�������


�ܽ���������:
0. ���뺯��
1. if�������()����ӻ����Ż��߲��ӻ�����(������ӻ����ŵĻ�Ĭ�ϸ÷�ֻ֧����һ������)
2. else if�������()����ӻ����Ż��߲��ӻ�����
3. else�ؼ��ֺ��滨���Ŵ���λ�����û�л�����һ�����
4. for�ؼ����汣��init-condition-refresh�ṹ�Լ�ѭ�����
5. do�ؼ��ֺ��滨���Ŵ���飬����û�л����ŵ�һ�зֺŽ�β����
6. while()���������浥�д�����߻����Ŵ����

�ܽ��뿪������:
0.�����뿪,������Ϊ���
1.if,else if,else���for���while��,�����Ի�����Ϊ���,��������һ������Ϊ���(ʵ���Ϸ�����ʱ����ʽ��,û�зֺŵػ����ֺ�)
2.do���뿪��ʵ����do-while�ṹ��,����while������Ѿ��뿪����,while��������ݲ��ܹ�ʹ�ÿ��ж������


//�����ǰ��ձ��ߵı�����
�ܽ�Ҫ���е����:
1.���̿��ƹؼ���Ҫ����
2.ѡ��ṹif�ؼ���Ҫ����
3.ѭ���ṹ�ؼ���do������while������forҪ����
4.��������{Ҫ����
5.�����Ҫ����
5.ѡ��ṹ

�ܽ᲻�û��е����:
1.���ڵ������������û���
2.for��init-condition-refresh���鲻�û���
3.ѡ��ṹ,��һ���߼����ʽ��ʽ���û���,֮��ÿ����Χ�߼�����ʻ���ÿ�������Ż�һ��
4.���м��������ʽ����ϲ��û���
5.���������Ÿ����ĸ�ֵ��䲻�û���

��β��ǻ������:
1.��;��β�����,����for��init-condition-refresh(���¼��ICR��)���û�������,������Ҫ����

for��ICR���е�init���ֽ��ܱ�������,
����������������Χ�����ⲿ����Χ��ʹ���ⲿ����ֻ���������Ž���ֲ�����

���Կ���ϸ�������﷨�ṹ

1.��ͨ˳��ṹ
�����ֺ�֮ǰ��������,�����ֺź���
������ͨ˳��ṹ���������ֳɷ�,�ֺ�֮ǰ�ɷ�,�Լ��ֺ�֮��ɷ�

2.if�ṹ
if( if������ʼ��(c��������if������߼����ʽҪ��С����������),Ҫ����
�߼�����  �߼�����ͬ�����벻�û���
) �߼�������������,Ҫ����
{ if�鿪ʼ
if������
} if�����,Ҫ�������

3.else if�ṹ,ֻ�ܹ�����if�ṹ����


4.else�ṹֻ�ܹ�����else if�ṹ����

5.{}�ṹ,�������̿��������Ŀ�,���ǵ�����һ����
{�鿪ʼ����,����
�������,����
}��������,����

6.do-while�ṹ
do{ �鿪ʼ�ṹ,����
�������,����
}while(   ����,����ѭ���߼��ж����,����
)     do-whileѭ���ж�������,���û���


7.while�ṹ
while(    while������ʼ��־��
...       while�����߼����ʽ,���û���
)       while����������־
{       while�鿪ʼ��־,���û���
...     while�����,����
}     while�������־,����,����

8.for�ṹ
for(    for�ṹ��ʼ��־,����,�����
init��    �ԷֺŽ�β,���û���
condition�� �ԷֺŽ�β���û���
refresh�ṹ ��С���Ž�β,���û���
)         for��ICR������־,���û���
{       forѭ���忪ʼ��־,���û���
...       forѭ�������,����
}       forѭ���������־,����

9.switch�ṹ(��ʱ�������������)
switch(     switch�ṹ��ʼ��־,����
...       switch�е������ʽ,���û���
){       switch�ж������ʽ��β�Լ��鿪ʼ��
case    case�ؼ���
...     case����
:       case����������


ʹ��ջ��������һ����������,���������ؼ����,�򱣴��ؼ�������͡�
��������ظ�ͬ�����,�򱣴�һ��
��������do{�鿪ʼ��־,���do{���ͷ���ѹ��ջ��
֮����ջ����������ͷ��ż��������������Ƿ�����﷨Ҫ��
���������һ���������,��ѹ��������
��������˵ڶ����������,�����ظ�ѹ��
��������˽������,��ѹ��.
��������˳������,��ȡ��֮ǰѹ�����������ֱ��������һ���������Ϊֹ

����
do{
  s1,g1,g2;
  s2;
  {
    s3;
  }
}while(q1&&q2);
��ȡ����ջ����Ϊ,��Ϊ�����Ϊ�տ�,���Կ��Լ�
do{     doBLockIn
s1    doBlockIn,partSentence,����,
g1;   doBlockIn,partSentence   ��Ȼû����ɾ���,������Ȼ��ȡ�������parSentece
g2;   doBlockIn,
s2;     doBlockIn
{       doBlockIn,normalBlockIn
s3      doBlockIn,normalBlcockIn
}       doBlockIn,normalBlockOut     //����,ȡ��ֱ��֮ǰ�Ľ�ջ����,ѹ���ջ����
}while(   doBlockOut        //��do-while���,ȡ��ջ��ֱ��֮ǰ����do-while��Ϊֹ������,ѹ���doBlock����
q1      doBlockOut halfDoCondition  //һ���doconditon���߼����
&&      connective          //connective����
q2    doBlockOut DoCondtition   //������߼����,��֮ǰһ����߼����ȡ��,�������
);    //doCondition������־,������,���֮ǰ�ĳ����Լ���doCondition�ķ��Ŷ���ջ,����˵������do-while�ṹ�����ݳ�ջ,����ṹ�����������Ѿ���������





*/


//��һ�����,����Ƕ�����һ�����,����һ�����������init
enum token_reader_state{
  NORMALBLOCK_IN,    //��ͨ�����
  NORMALBLOCK_OUT, //��ͨ������
  IF_CONDITION_START,    //if�����鿪ʼ��־if(
  IF_CONDITION,      //if����ʽ
  IF_CONDITION_END,     //if�����������־,���֮ǰѹ��ջ�е�����
  IF_BLOCK_IN,       //if������
  IF_BLOCK_OUT,      //if���������
  ELIF_CONDITION_START,   //else if(,Ҳ����else if������ʼ,����ǰ����if����else if�������������Ч
  ELIF_CONDITION,       //else if������
  ELIF_CONDITION_FINISH,      //else if��������
  ELIF_BLOCK_IN,     //else if��֧�鿪ʼ
  ELIF_BLOCK_OUT,     //else if��֧�����
  ELSE_BLOCK_IN,   //else{ �鿪ʼ
  ELSE_BLOCK_OUT,   //} else�����,������if-elseif-else����if-else�ṹ�������ݳ�ջ
  WHILE_CONDITION_START,  //while(  while������ʼ
  WHILE_CONDITION_END,    //)while��������
  WHILE_BLOCK_IN,    //{ whileѭ�������鿪ʼ
  WHILE_BLOCK_OUT,    //} whileѭ�����������
  DO_BLOCK_IN,   //do{ doѭ�������鿪
  DO_CONDITION_START,     //}while( mͬʱ��DoBLockOut
  DO_CONDITION,
  DO_CONDITION_END,     //)while��������
  FOR_START,     //for(  for��init���ֿ�ʼ��־,Ҳ��for��Ľ��뷽��
  FOR_INIT,       //for��ʼ��־�󵽵�һ���ֺ�֮ǰΪinit����
  FOR_CONDITION,    //FORINIT����һ���ֺ�ǰ
  FOR_REFRESH,    //FORCONDITION����һ������ǰ
  FOR_BODY_IN,   //{ forѭ���忪ʼ
  FOR_BODY_OUT  //} forѭ�������
};

struct lastSentenceKindStack{
  struct lastSentenceKindStack* next;
  enum token_reader_state kind;
};   //ʲô��û�д�����


typedef struct token_buffer_node{
  struct token_buffer_node* last;
  struct token_buffer_node* next;
  struct token token;
}TBNode;



TBNode head={NULL,NULL,{0,NULL}};  //��������ͷ�ڵ�,ͷ�ڵ㲻����ֵ,ͷ�ڵ����һ���ڵ���Ϊ��Ԫ�ڵ�

struct lastSentenceKindStack
preKindStack={NULL,0};

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