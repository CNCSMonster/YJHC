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

//返回字符传到输入中
int ungets(char* s,FILE* fin){
  for(int i=0;i<strlen(s);i++){
    ungetc(s[strlen(s)-1-i],fin);
  }
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

int isBaseType(char* s){
  for(int i=0;i<sizeof(basetypes)/sizeof(basetypes[0]);i++){
    if(!strcmp(s,basetypes[i])) return 1;
  }
  return 0;
}

int isTypeDefKeyWords(char* s){
  for(int i=0;i<sizeof(typeKeyWords)/sizeof(typeKeyWords[0]);i++){
    if(!strcmp(s,typeKeyWords[i])) return 1;
  }
  return 0;
}


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

int isEscape(char c){
  if (c == 'n' || c == 't' || c == 'b' || c == 'r' || c == '\"' || c == '\\')
  {
    return 1;
  }
  return 0;
}


int isKeyForProcessControl(char* s){
  for(int i=0;i<sizeof(keyForProcessControl)/sizeof(keyForProcessControl[0]);i++){
    if(strcmp(s,keyForProcessControl[i])==0) return controlTokens[i];
  }
  return -1;
}

int isSep(char c){
  for(int i=0;i<sizeof(seps)/sizeof(seps[0]);i++){
    if(seps[i]==c){
      return sepTokens[i];
    }
  }
  return -1;
}

int isOp(char c){
  for(int i=0;i<sizeof(operationElems)/sizeof(operationElems[0]);i++){
    if(operationElems[i]==c) return 1;
  }
  return 0;
}

//对字符串去除前后不必要字符
int myStrStrip(char* str,const char* prefexs,const char* suffixs){
  if(str==NULL) return 0;
  char* st=str;
  char* end=str+strlen(str)-1;
  for (int j = 0;prefexs!=NULL&& j < strlen(prefexs)&&st<=end; j++)
  {
    if (*st != prefexs[j])
      continue;
    st++;
    j=-1;
  }
  for (int j = 0;suffixs!=NULL&& j < strlen(suffixs)&&st<=end; j++)
  {
    if (*end != suffixs[j])
      continue;
    *end = '\0';
    end--;
    j=-1;
  }
  strcpy(str,st);
  return 1;
}

//过滤去除字符串中的某些字符,返回的是过滤后的字符指针,为了返回链式编程
//该函数会删除字符串中在toRemoves出现的字符,后续字符向前补位
char* myStrFilter(char* str,const char* toRemoves){
  //,试图给参数减少一些前缀,保存参数本身的时候
  int i=0;
  for(int j=0;j<strlen(str);j++){
    int isToRemove=0;
    for(int k=0;k<strlen(toRemoves);k++){
      if(str[j]==toRemoves[k]){
        isToRemove=1;
        break;
      }
    }
    if(isToRemove) continue;
    str[i++]=str[j];
  }
  str[i]='\0';
  return str;
}


//判断字符串中是否有个字符,如果是返回非0值,如果不是返回0
int myIsCharInStr(const char* str,const char c){
  for(int i=0;i<strlen(str);i++) if(c==str[i]) return 1;
  return 0;
}


//进行一个普普通通的哈希
int myStrHash(const char* str){
  int out=0;
  int size=strlen(str)/sizeof(out);
  int* m=(int*) str;
  for(int i=0;i<size;i++) out^=m[i];
  out+=(strlen(str)-size*sizeof(out))*size;
  return out;
}
