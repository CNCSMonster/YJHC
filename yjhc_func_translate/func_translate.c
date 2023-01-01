#include "func_translate.h"

//首先判断是否需要进行打印

int main(){
  //读取，然后打印每个块的语句
  char* tokensPath="..\\out\\func_tokens.txt";
  FILE* fin=fopen(tokensPath,"r");
  
  init_token_reader(fin);
  TBNode* tmp;
  BlockAction ba;
  PrintAction pa;
  while((tmp=readTokenSentence(&ba,&pa))!=NULL){
    //打印一下
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












