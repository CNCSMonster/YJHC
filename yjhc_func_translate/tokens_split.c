#include "all.h"


//�ó������ڶ�ĳ���ļ��еĺ���token���Ծ���Ϊ��λ���зָ�
int main(int argc,char* argv[]){
  char* inputPath=argv[1];
  char* outputPath=argv[2];
  // char* inputPath="..\\out\\func_tokens.txt";
  // char* inputPath="..\\out\\after_tokens_split.txt";
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
    fprintf(fout,"\t%d\n",actionSet.blocks);  //��ӡ�����
  }
  release_token_reader();
  return 1;
}