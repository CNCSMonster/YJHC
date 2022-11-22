#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "mystring.c"
#include "mystring.h"



int type_func_extract(FILE* fin,FILE* func_fout,FILE* type_fout);

int main(int argc,char* argv[]){
  //把经过去注释,去宏常量和预编译指令，去全局常量和变量后的文件进行处理
  //输入的文件中这个时候只含有函数和类型定义
  //我们要输出两个文件,一个是函数文件，一个是类型定义文件
  char* inputPath;
  char* funcPath;
  char* typePath;
  if(argc!=4){
    exit(-1);
  }else{
    inputPath=argv[1];
    funcPath=argv[2];
    typePath=argv[3];
  }
  FILE* fin=fopen(inputPath,"r");
  if(!fin){
    printf("fail to open inputPath:%s",inputPath);
    exit(-1);
  }
  FILE* func_fout=fopen(funcPath,"w");
  if(!func_fout){
    fclose(fin);
    printf("fail to open funcPath:%s",funcPath);
    exit(-1);
  }
  FILE* type_fout=fopen(typePath,"w");
  if(!type_fout){
    fclose(fin);fclose(func_fout);
    printf("fail to open typePath:%s",typePath);
    exit(-1);
  }
  //然后才是正式进行类型提取
  if(!type_func_extract(fin,func_fout,type_fout)){
    printf("syntax wrong");
  }
  fclose(fin);fclose(func_fout);fclose(type_fout);
  return 0;
}

//类型，函数提取以及分类
int type_func_extract(FILE* fin,FILE* func_fout,FILE* type_fout){
  //每两个单元之间用换行符结尾
  //提取完之后每个单元的第一个词都是关键字，要么是type,要么是基本数据类型
  char first[500];
  char end;
  while(!feof(fin)){
    myfgets(first," ",fin);
    if(strcmp(first,"")==0){
      continue;
    }
    printf("%s,\n",first);
    //如果这个单元的第一个词是typedef，则这个单元一定是类型定义式
    if(strcmp(first,"typedef")==0){
      myfgets(first,"\n",fin);
      fprintf(type_fout,"typedef %s\n",first);
    }
    //如果这个单元的第一个词是struct,则可能是结构体定义或者是结构体方法定义，又或者是全局方法定义
    else if(strcmp(first,"struct")==0){
      end=myfgets(first,">({",fin);
      //如果先读取到'-'则超前搜索一步，如果下一个字符是'>'则说明是结构体方法单元
      if(end=='>'){
        fprintf(func_fout,"struct %s->",first);
        //读取，把直到换行为止得内容写入方法区区
        while((end=fgetc(fin))!=EOF&&end!='\n'){
          fputc(end,func_fout);
        }
        fputc(end,func_fout);
      }
      //如果先遇到左括号,说明是全局函数,写入方法区
      else if(end=='('){
        fprintf(func_fout,"struct %s(",first);
        while((end=fgetc(fin))!=EOF&&end!='\n'){
          fputc(end,func_fout);
        }
        fputc(end,func_fout);
      }
      //如果先遇到{,说明是结构体定义,写入类型定义区
      else if(end=='{'){
        fprintf(type_fout,"struct %s{",first);
        myfgets(first,"\n",fin);
        fprintf(type_fout,"%s\n",first);
      }

    }
    //如果第一个词是基本数据类型,则一定是函数定义
    else if(isBaseType(first)){
      fprintf(func_fout,"%s ",first);
      myfgets(first,"\n",fin);
      fprintf(func_fout,"%s\n",first);
    }
    //如果是union或者enum，可能是类型定义，或者是方法定义
    else if(isTypeDefKeyWords(first)){
      char term[15];
      strcpy(term,first);
      end=myfgets(first,"{(",fin);
      //如果是类型定义
      if(end=='{'){
        fprintf(type_fout,"%s %s{",term,first);
        myfgets(first,"\n",fin);
        fprintf(type_fout,"%s\n",first);
      }
      //如果是函数定义
      else if(end=='('){
        fprintf(func_fout,"%s(",term);
        while((end=fgetc(fin))!=EOF&&end!='\n'){
          fputc(end,func_fout);
        }
        fputc(end,func_fout);
      } 
      //
      else return 0;
    }
    //否则可能是自定义类型的别名,则属于函数
    else{
      char term[15];
      strcpy(term,first);
      end=myfgets(first,"{(",fin);
      //如果是函数定义
      if(end=='('){
        fprintf(func_fout,"%s(",term);
        while((end=fgetc(fin))!=EOF&&end!='\n'){
          fputc(end,func_fout);
        }
        fputc(end,func_fout);
      } 
      //
      else return 0;
    }
  }
  return 1;
}