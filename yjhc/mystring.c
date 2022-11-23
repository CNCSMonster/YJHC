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
  for(int i=0;i<sizeof(basetypes)/sizeof(basetypes[0]);i++){
    if(!strcmp(s,basetypes[i])) return 1;
  }
  return 0;
}

//判断是否是自定义类型定义关键字
int isTypeDefKeyWords(char* s){
  for(int i=0;i<sizeof(typeKeyWords)/sizeof(typeKeyWords[0]);i++){
    if(!strcmp(s,typeKeyWords[i])) return 1;
  }
  return 0;
}


//在已经读取出字符串左双引号的请况下,
// 从文件中读取剩下的字符串,如果读不出符合c语法的字符串,
// 则返回0,读取成功则返回非0值,以及通过指针返回读取到的字符串,不包括前后0
int freadRestString(FILE* fin,char* returnString){
  //此时已经从程序中读出了一个双引号,期待字符串结尾的双引号
  char* stops="\\\"";
  char end;
  int i=0;  //指示保存到returnString中的当前起点
  char tmp[1000]; //用来缓冲保存读取到的内容
  while((end=myfgets(tmp,stops,fin))!=EOF){
    i+=sprintf(i+returnString,"%s",tmp);
    if(end=='\"') break;
    returnString[i++]=end;
    //如果是'\\',要考虑转义的可能
    if(end=='\\'){
      //读取下一个字符,判断是否转义
      char c=fgetc(fin);
      if(!isEscape(c)) return 0;
      else returnString[i++]=c;
    }
  }
  if(end==EOF) return 0;
  returnString[i]='\0';
  return 1;
}

//判断字符能否被转义
//判断是否是合理的转义字符
int isEscape(char c){
  if (c == 'n' || c == 't' || c == 'b' || c == 'r' || c == '\"' || c == '\\')
  {
    return 1;
  }
  return 0;
}


//判断是否是流程控制关键字
int isKeyForProcessControl(char* s){
  for(int i=0;i<sizeof(keyForProcessControl)/sizeof(keyForProcessControl[0]);i++){
    if(strcmp(s,keyForProcessControl[i])==0) return 1;
  }
  return 0;
}

//判断是否是界符
int isSep(char c){
  for(int i=0;i<sizeof(sepratorSymbols)/sizeof(sepratorSymbols[0]);i++){
    if(sepratorSymbols[i]==c) return 1;
  }
  return 0;
}
