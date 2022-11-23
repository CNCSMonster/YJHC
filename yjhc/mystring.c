#include "mystring.h"

//��ȡһ���ַ���ֱ��������ֹ���ż��еķ��Ż��߶�ȡ���ļ�ĩβ,����ֵΪ��ֹ��
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

//���ַ�������ָ���ս����ȡһ���ַ���
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


//�ж��Ƿ�һ���ַ����ǻ����������͹ؼ���,�Ƿ��ط�0ֵ�����Ƿ���0
int isBaseType(char* s){
  for(int i=0;i<sizeof(basetypes)/sizeof(basetypes[0]);i++){
    if(!strcmp(s,basetypes[i])) return 1;
  }
  return 0;
}

//�ж��Ƿ����Զ������Ͷ���ؼ���
int isTypeDefKeyWords(char* s){
  for(int i=0;i<sizeof(typeKeyWords)/sizeof(typeKeyWords[0]);i++){
    if(!strcmp(s,typeKeyWords[i])) return 1;
  }
  return 0;
}


//���Ѿ���ȡ���ַ�����˫���ŵ������,
// ���ļ��ж�ȡʣ�µ��ַ���,�������������c�﷨���ַ���,
// �򷵻�0,��ȡ�ɹ��򷵻ط�0ֵ,�Լ�ͨ��ָ�뷵�ض�ȡ�����ַ���,������ǰ��0
int freadRestString(FILE* fin,char* returnString){
  //��ʱ�Ѿ��ӳ����ж�����һ��˫����,�ڴ��ַ�����β��˫����
  char* stops="\\\"";
  char end;
  int i=0;  //ָʾ���浽returnString�еĵ�ǰ���
  char tmp[1000]; //�������屣���ȡ��������
  while((end=myfgets(tmp,stops,fin))!=EOF){
    i+=sprintf(i+returnString,"%s",tmp);
    if(end=='\"') break;
    returnString[i++]=end;
    //�����'\\',Ҫ����ת��Ŀ���
    if(end=='\\'){
      //��ȡ��һ���ַ�,�ж��Ƿ�ת��
      char c=fgetc(fin);
      if(!isEscape(c)) return 0;
      else returnString[i++]=c;
    }
  }
  if(end==EOF) return 0;
  returnString[i]='\0';
  return 1;
}

//�ж��ַ��ܷ�ת��
//�ж��Ƿ��Ǻ����ת���ַ�
int isEscape(char c){
  if (c == 'n' || c == 't' || c == 'b' || c == 'r' || c == '\"' || c == '\\')
  {
    return 1;
  }
  return 0;
}


//�ж��Ƿ������̿��ƹؼ���
int isKeyForProcessControl(char* s){
  for(int i=0;i<sizeof(keyForProcessControl)/sizeof(keyForProcessControl[0]);i++){
    if(strcmp(s,keyForProcessControl[i])==0) return 1;
  }
  return 0;
}

//�ж��Ƿ��ǽ��
int isSep(char c){
  for(int i=0;i<sizeof(sepratorSymbols)/sizeof(sepratorSymbols[0]);i++){
    if(sepratorSymbols[i]==c) return 1;
  }
  return 0;
}
