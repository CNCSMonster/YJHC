#include "all.h"


//该程序用于对某个文件中的函数token串以句子为单位进行分割
int main(int argc,char* argv[]){
  char* inputPath=argv[1];
  char* outputPath=argv[2];
  FILE* fin=fopen(inputPath,"r");
  FILE* fout=fopen(outputPath,"w");
  int isRight=1;
  if(fin==NULL||fout==NULL) isRight=0;
  if(!isRight){
    if(fin!=NULL) fclose(fin);
    if(fout!=NULL) fclose(fout);
    return 0;
  }
  init_token_reader(fin);
  TBNode* nodes;
  ActionSet actionSet;
  while((nodes=readTokenSentence(&actionSet))!=NULL){
    fshow_tokenLine(fout,nodes);
    fprintf(fout,"\n");
  }
  release_token_reader();
  return 1;
}