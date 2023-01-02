#include "func_translate.h"

//�����ж��Ƿ���Ҫ���д�ӡ

int main(){
  //��ȡ��Ȼ���ӡÿ��������
  char* tokensPath="..\\out\\func_tokens.txt";
  char* typesPath="..\\out\\type.txt";
  FILE* fin=fopen(tokensPath,"r");
  FILE* typeFin=fopen(typesPath,"r");
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

  //��ȡ����,Ȼ���ӡ
  TypeTbl typeTbl=getTypeTbl(typeFin);
  //��ӡÿ������
  for(int i=0;i<typeTbl.size;i++){
    printf("%d\n\n",i);
    showType(&typeTbl.types[i]);
  }
  delTypeTbl(&typeTbl);
  fclose(typeFin);

  return 0;
}












