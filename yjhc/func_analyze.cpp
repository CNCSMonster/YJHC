#include <stdio.h>
#include <stdlib.h>

#include <regex>

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
  if(!){

  }

  fclose(fin);fclose(head);fclose(code);
  return 0;
}



//����δʷ�����,ֱ�������һ����Ž���
int code_parse(FILE* fin,FILE* code){
  //�����ļ���д��һ������
  fputc();

}




//����ͷ����,ֱ������һ�������Ž���
int head_analyze(FILE* fin,FILE* head){
  
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