#include "mystring.h"

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
