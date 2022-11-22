#include <stdio.h>
#include <stdlib.h>

#include <regex>

//代码段词法分析,直到遇到右花括号结束
int code_parse(FILE* fin,FILE* code);
//函数头分析,直到遇到一个左括号结束
int head_analyze(FILE* fin,FILE* head);
//函数文件分析
int func_analyze(FILE* fin,FILE* head,FILE* code);

//文件函数分信息
int func_analyze(FILE* fin,FILE* fout);

int main(int argc,char* argv[]){
  if(!argc==4){
    printf("func analysis need and only need 3 argument!input path, func head path,and code path!");
    exit(-1);
  }
  char* inputPath=argv[1];
  char* headPath=argv[2];
  char* codePath=argv[3];
  FILE* fin=fopen(inputPath,"r");
  if(fin==NULL){
    printf("fail to open %s",inputPath);
    exit(-1);
  }
  FILE* head=fopen(headPath,"w");
  if(head==NULL){
    fclose(fin);
    printf("fail to open %s",headPath);
    exit(-1);
  }
  FILE* code=fopen(codePath,"w");
  if(code==NULL){
    fclose(fin);
    fclose(head);
    printf("fail to open %s",outputPath);
    exit(-1);
  }
  if(!){

  }

  fclose(fin);fclose(head);fclose(code);
  return 0;
}



//代码段词法分析,直到遇到右花括号结束
int code_parse(FILE* fin,FILE* code){
  //先往文件中写入一个函数
  fputc();

}




//函数头分析,直到遇到一个左括号结束
int head_analyze(FILE* fin,FILE* head){
  
}



//函数文件分析
int func_analyze(FILE* fin,FILE* head,FILE* code){
  //如果文件没有处理完就循环读取处理
  while(!feof(fin)){
    if(!head_analyze(fin,head)) return 0;
    if(!code_parse(fin,code)) return 0;
  }
  return 1;
}