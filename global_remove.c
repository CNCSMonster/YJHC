#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int myfgets(char* buf,char* stops,FILE* fin);
int global_remove(FILE* fin,FILE* fout,FILE* global);
//判断是否是自定义类型定义关键字
int isTypeDefKeyWords(char* s);
//判断是否一个字符串是基础数据类型关键字,是返回非0值，不是返回0
int isBaseType(char* s);
int mystrReplace(char* s,char old,char new);
/*
fin为去掉宏定义后的文件的指针,fout为写入去掉全局量后的文件的指针,global为全局变量的位置
把全局变量和全局常量(非宏定义常量)提取到一个文件之中
全局常量包括:
const int m;
const int arr[2]={1,2};

全局常量变量 包括:
int m;
int arr[2]={1,2};

c语言中全局常量的定义以const开头，以分号结尾，而且可以赋值，否则给与一个默认的初始值
全局变量定义以类型名开头，以分号结尾，初始化一定要用常量赋值，或者不初始化，给予一个默认的初始值


*/



int global_remove(FILE* fin,FILE* fout,FILE* global){
  //如果没有读到文件尽头,则继续读取
  //每个语句的开头一定是一个关键字声明,要么是数据类型,要么是结构体符号或者共用体符号
  //或者是重命名符号,或者是常量声明关键字
  char first[400]; //读取第一个关键字
  while(!feof(fin)){
    char end;
    end=myfgets(first,"; \n",fin);
    if(strcmp(first,"")==0) continue;   //排除可能读到空字符的情况
    printf("^%s$\n",first);
    //判断是否是重命名符号
    if(strcmp(first,"typedef")==0){
      printf("c1\n");
      //如果是重名名符号,则一定不是全局常量和全局变量定义，
      //只可能是类型定义
      fprintf(fout,"%s%c",first,end);
      end=myfgets(first,";",fin);
      if(end!=';') return 0;    //应该能够读到以;结尾
      fprintf(fout,"%s%c",first,end);
    }
    //判断是否是常量声明符号
    else if(strcmp(first,"const")==0){
      printf("c2\n");
      //说明是常量声明
      end=myfgets(first,";",fin); //读取到下一个分号为止
      //常量声明写入文件，去掉分号,而且把常量声明语句中所有换行替换成空格
      mystrReplace(first,'\n',' ');
      fprintf(global,"const %s\n",first);
    }
    //判断是否是结构体关键字,如果是结构体类型,可能是结构体方法定义语句块,也可能是结构体定义语块
    else if(isTypeDefKeyWords(first)||isBaseType(first)){
      printf("c3\n");
      char last[100];
      strcpy(last,first);
      end=myfgets(first,";{=",fin);
      //如果先遇到分号结束,则说明是全局变量定义语句
      //如果先遇到等号结束，说明是全局变量定义语句且进行了初始化
      if(end==';'){
        mystrReplace(first,'\n',' ');
        fprintf(global,"%s %s\n",last,first);
      }
      else if(end=='='){
        mystrReplace(first,'\n',' ');
        fprintf(global,"%s %s =",last,first);
        end=myfgets(first,";",fin);
        printf("after=:%s\n",first);
        mystrReplace(first,'\n',' ');
        fprintf(global,"%s\n",first);
        // printf("1\n");  //调试语句
      }
      //否则是函数
      else if(end=='{'){
        mystrReplace(first,'\n',' ');
        fprintf(fout,"%s %s{",last,first);
        while((end=myfgets(first,";}",fin))!=EOF){
          if(end=='}') break;
          mystrReplace(first,'\n',' ');
          fprintf(fout,"%s;",first);
        }
        if(end=='}') fprintf(fout," }\n");
        else return 0;
      }
      //异常
      else return 0;
    }
    //否则是异常情况
    else{
      return 0;
    }
  }
  return 1;
}

//判断是否一个字符串是基础数据类型关键字,是返回非0值，不是返回0
int isBaseType(char* s){
  char* basetypes[]={
    "int",
    "short",
    "double",
    "long",
    "char"
  };
  for(int i=0;i<sizeof(basetypes)/sizeof(basetypes[0]);i++){
    if(!strcmp(s,basetypes[i])) return 1;
  }
  return 0;
}

//判断是否是自定义类型定义关键字
int isTypeDefKeyWords(char* s){
  char* typeKeyWords[]={
    "struct",
    "union",
    "enum"
  };
  for(int i=0;i<sizeof(typeKeyWords)/sizeof(typeKeyWords[0]);i++){
    if(!strcmp(s,typeKeyWords[i])) return 1;
  }
  return 0;
}

//读取一个字符串直到读到截止符号集中的符号或者读取到文件末尾,返回值为终止符
int myfgets(char* buf,char* stops,FILE* fin){
  char c;
  int i=0;
  while ((c=fgetc(fin))!=EOF)
  {
    int ifstop=0;
    for(int j=0;j<strlen(stops);j++){
      if(stops[j]==c){
        ifstop=1;
        break;
      }
    }
    if(ifstop){
      break;
    }
    buf[i++]=c;
  }
  buf[i]='\0';
  return c;
}
int mystrReplace(char* s,char old,char new){
  for(int i=0;i<strlen(s);i++) if(s[i]==old) s[i]=new;
  return 1;
}



int main(int argc,char* argv[]){
  //把含有井的预编译命令和宏常量抵挡一提取出来
  if(argc!=4){
    printf("wrong!we need and only need 3 argument,pleas check your input");
    exit(-1);
  }
  FILE* fin=fopen(argv[1],"r"); //打开要读取文件，获取句柄
  // FILE* fin=fopen("code.yjhc","r"); //打开要读取文件，获取句柄
  if(!fin){
    //如果打开文件失败
    printf("fail to open %s",argv[1]);
    exit(-1);
  }
  FILE* fout=fopen(argv[2],"w");
  // FILE* fout=fopen("out.txt","w"); //打开要读取文件，获取句柄
  if(!fout){
    fclose(fin);
    printf("fail to open %s for write",argv[2]);
    exit(-1);
  }
  FILE* global_fout=fopen(argv[3],"w");  //打开要写入提取出的宏定义的文件
  if(!global_fout){
    fclose(fin);
    printf("fail to open %s for write",argv[23]);
    exit(-1);
  }
  if(!global_remove(fin,fout,global_fout)){
    printf("fail to pick up global sentence from input");
  }
  fclose(fin);fclose(fout);fclose(global_fout);
  return 0;
}