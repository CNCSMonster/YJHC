#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mystring.h"
#include "mystring.c"

#define ownerString "NULL"

//����δʷ�����,ֱ�������һ����Ž���
int code_parse(FILE* fin,FILE* code);
//����ͷ����,ֱ������һ�������Ž���
int head_analyze(FILE* fin,FILE* head);
//�����ļ�����
int func_analyze(FILE* fin,FILE* head,FILE* code);

//�ļ���������Ϣ
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



//����δʷ�����,ֱ�������һ����Ž���
int code_parse(FILE* fin,FILE* code){
  //�����ļ���д��һ�������Ų�����
  fputs("{\n",code);
  int leftPar=1;
  //����token������������





  //����ʱ��д��һ�������Ų�����
  fputs("}\n",code);
}




//����ͷ����,ֱ������һ�������Ž���
int head_analyze(FILE* fin,FILE* head){
  char* stops=" -();,{}}\n";
  //���ȶ�ȡ����
  char end;
  char tmp[2000];
  char returnType[100];
  char owner[100];
  char funcName[100];
  char args[200]; //��������б�,�����汣������б��ʽΪtype1 varname1,type2 varname2������,������ǰ������
  //�ڶ���������֮ǰ���д���
  //���Ƚ��к��������ж�,�Լ���ȡ��������ֵ����
  if((end=myfgets(tmp,stops,fin))==' '){
    //���ȶ�ȡ���Ŀ����Ƿ���ֵҲ�����ǲ����б�
    //�����ͷ�ǻ�������,����ȫ�ֺ���
    if(isBaseType(tmp)){
      strcpy(returnType,tmp);
      strcpy(owner,ownerString);
    }
    //����ǽṹ�����,���ȡ��һλ
    else if(strcmp(tmp,"struct")==0){
      end=myfgets(tmp,stops,fin);
      char tmp2[100];
      if(end!=' ') return 0;
      else{
        sprintf(tmp2,"struct %s",tmp);
      }
      //Ȼ���ж���һλ�Ƿ��ܹ���ȡ�����ʵ��ַ���,���ն���
      while((end=myfgets(tmp," >",fin))&&){

      }
      

    }
    //�Ƿ����������Ͷ���ؼ���
    else if(isTypeDefKeyWords(tmp)){

    }
    //���������typedef�����������ͱ���
    else{
      

    }
  }
  else return 0;
  //Ȼ���ȡ������

  //Ȼ���ȡ�����б�


  //�����и�ʽ��д��
  fprintf(head,"%s|%s|%s|(%s)",funcName,owner,returnType,args);
  return 1;
}



//�����ļ�����
int func_analyze(FILE* fin,FILE* head,FILE* code){
  //����ļ�û�д������ѭ����ȡ����
  while(!feof(fin)){
    if(!head_analyze(fin,head)) return 0;
    if(!code_parse(fin,code)) return 0;
  }
  return 1;
}