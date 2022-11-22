#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mystring.h"
#include "mystring.c"

#define ownerString "NULL"

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
  if(!func_analyze(fin,head,code)){
    printf("syntax error!,fail to anlyze the function");
  }
  fclose(fin);fclose(head);fclose(code);
  return 0;
}



//代码段词法分析,直到遇到右花括号结束
int code_parse(FILE* fin,FILE* code){
  //先往文件中写入一个花括号并换行
  fputs("{\n",code);
  int leftPar=1;
  //进行token分析并且描述





  //结束时再写入一个花括号并换行
  fputs("}\n",code);
}




//函数头分析,直到遇到一个左括号结束
int head_analyze(FILE* fin,FILE* head){
  char* stops=" -();,{}}\n";
  //首先读取类型
  char end;
  char tmp[2000];
  char returnType[100];
  char owner[100];
  char funcName[100];
  char args[200]; //保存参数列表,这里面保存参数列表格式为type1 varname1,type2 varname2这样子,不保留前后括号
  //在读到函数体之前进行处理
  //首先进行函数主人判定,以及获取函数返回值类型
  if((end=myfgets(tmp,stops,fin))==' '){
    //首先读取到的可能是返回值也可能是参数列表
    //如果打头是基本类型,则是全局函数
    if(isBaseType(tmp)){
      strcpy(returnType,tmp);
      strcpy(owner,ownerString);
    }
    //如果是结构体符号,则读取下一位
    else if(strcmp(tmp,"struct")==0){
      end=myfgets(tmp,stops,fin);
      char tmp2[100];
      if(end!=' ') return 0;
      else{
        sprintf(tmp2,"struct %s",tmp);
      }
      //然后判断下一位是否能够读取到合适的字符串,按照定义
      while((end=myfgets(tmp," >",fin))&&){

      }
      

    }
    //是否是其他类型定义关键字
    else if(isTypeDefKeyWords(tmp)){

    }
    //否则可能是typedef重命名的类型别名
    else{
      

    }
  }
  else return 0;
  //然后获取函数名

  //然后获取参数列表


  //最后进行格式化写入
  fprintf(head,"%s|%s|%s|(%s)",funcName,owner,returnType,args);
  return 1;
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