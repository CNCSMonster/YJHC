#include <stdio.h>
#include <stdlib.h>

#include "../yjhc/mystring.h"
#include "../yjhc/mystring.c"

//输入token编码以及字符字面值文件,然后输出对应格式的字符分析结果到控制台中
int main(int argc, char* argv[]){
  if(argc!=2){
    fputc('e',stdout);
  }
  FILE* fin=fopen(argv[1],"r");
  char tmp[1000];
  TokenKind kind;
  char end;
  while(!feof(fin)){
    int jud=fscanf(fin,"%d",&kind);
    char c=fgetc(fin);
    if(jud!=1) break;
    end=myfgets(tmp,"\n",fin);
    if(end!='\n') break;
    printf("(%s, %s)\n",tokenStrings[kind],tmp);
  }
  fclose(fin);
  if(end!='\n') exit(-1);
  return 0;

}