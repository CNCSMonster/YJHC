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

//从字符串根据指定终结符读取一个字符串
int mysgets(char* buf,const char* stops,const char* src){
  char c;
  int i=0;
  int k=0;
  while ((c=src[k++])!='\0')
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


//判断是否一个字符串是基础数据类型关键字,是返回非0值，不是返回0
int isBaseType(char* s){
  char* basetypes[]={
    "int",
    "short",
    "double",
    "long",
    "char",
    "void"  //表示无类型
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


//在已经读取出字符串左双引号的请况下,
// 从文件中读取剩下的字符串,如果读不出符合c语法的字符串,
// 则返回0,读取成功则返回非0值
int freadRestString(FILE* fin){

}

//判断字符能否被转义
//判断是否是合理的转义字符
int isEscape(char c){
    if(c=='n'||c=='t'||c=='b'||c=='r'||c=='\"'||c=='\\'){
      return 1;
    }
    return 0;
}
