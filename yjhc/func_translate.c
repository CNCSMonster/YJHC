#include <stdio.h>
#include <stdlib.h>

#include "token_reader.c"
#include "token_reader.h"

#include "token.h"
#include "token.c"

#include "mystring.h"
#include "mystring.c"


//�����ж��Ƿ���Ҫ���к�������


//Ȼ����з���


//��������Ҫ���ø��ֱ����Ϣ,Ҫȫ������,���������ͱ���,
//Ҫ���Զ���������Ϣ��(�������Ҫ�޸�),Ҫ�ܹ����ٲ������Ͷ���ĳ�Ա���ͳ�Ա����
//��Ҫ�оֲ�������,��������Ҫ�ܹ���չ�Լ����п������
//ʹ�÷���ft <��������token�ļ�> <����ͷ�ļ�> <ȫ�����ļ�> <���Ͷ����ļ�> <���������ļ�> <��������ļ�>
int main(){
  //��ȡ��Ȼ���ӡÿ��������
  char* tokensPath="..\\out\\func_tokens.txt";
  FILE* fin=fopen(tokensPath,"r");
  
  init_token_reader(fin);
  TBNode* tmp;
  BlockAction ba;
  PrintAction pa;
  while((tmp=readTokenSentence(&ba,&pa))!=NULL){
    //��ӡһ��
    TBNode* tr=tmp;
    do{
      printf("%s ",tr->token.val);
      tr=tr->next;
    }while(tr!=NULL);
    // if(pa==PRINTENTER) printf("\tenter");
    // else printf("\tspace");
    if(pa==PRINTENTER) printf("\n");
    del_tokenLine(tmp);
  }
  del_rest();
  fclose(fin);

  return 0;
}