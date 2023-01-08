#include "all.h"

int main(int argc,char* argv[]){
  if(argc<3) fprintf(stderr,"miss arg!we need inputPath and outputPath!\n");
  FILE* fin=fopen(argv[1],"r");
  FILE* fout=fopen(argv[2],"w");
  init_token_reader(fin);
  TBNode* nodes;
  ActionSet actionSet;
  int preblocks=0;
  while((nodes=readTokenSentence(&actionSet))!=NULL){
    //如果是进入函数,先换行
    if(strcmp(nodes->token.val,"{")==0&&nodes->next==NULL) ;
    else if(actionSet.printTblAction==PRINT_TBL_PRE)
      for(int i=0;i<actionSet.blocks;i++) fprintf(fout,"\t");
    fshow_tokenLine(fout,nodes);
    if(actionSet.printAction==PRINTENTER){
      fprintf(fout,"\n");
      //如果打印enter了,打印和块数相当的缩进符号
      int n=actionSet.blocks;
      // if(strcmp(nodes->token.val,"}")==0) n--;
      // for(int i=0;i<n;i++) printf("\t");
    }
    else fprintf(fout," ");
    if(actionSet.printTblAction==PRINT_TBL_AFTER) 
      for(int i=0;i<actionSet.blocks;i++) fprintf(fout,"\t");
    preblocks=actionSet.blocks;
    del_tokenLine(nodes);
  }
  fclose(fin);
  fclose(fout);
  del_rest_token_reader();
  return 1;
}