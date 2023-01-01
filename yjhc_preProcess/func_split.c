#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mystring.h"
#include "mystring.c"
#include "token_kind.h"
#include "string.h"
#include "token.h"
#include "token.c"


#define ownerString "NULL"


//����������,ֱ�������Ž���,����1,˵����������,����˵�������﷨����
int output_funcBody(FILE* fin,FILE* code);
//����ͷ����,ֱ������һ�������Ž���
int head_analyze(FILE *fin, FILE *head);
//�����ļ�����
int func_analyze(FILE *fin, FILE *head, FILE *code);

//�ַ��������Ҫ�Ľ��,ʹ��

int main(int argc, char *argv[])
{
  if (!argc == 4)
  {
    printf("func analysis need and only need 3 argument!input path, func head path,and code path!");
    exit(-1);
  }
  char *inputPath = argv[1];
  char *headPath = argv[2];
  char *codePath = argv[3];
  // char* inputPath="../out/func.txt";
  // char* headPath="../out/func_head.txt";
  // char* codePath="../out/func_tokens.txt";
  FILE *fin = fopen(inputPath, "r");
  if (fin == NULL)
  {
    printf("fail to open %s", inputPath);
    exit(-1);
  }
  FILE *head = fopen(headPath, "w");
  if (head == NULL)
  {
    fclose(fin);
    printf("fail to open %s", headPath);
    exit(-1);
  }
  FILE* code=fopen(codePath,"w");
  if (code == NULL)
  {
    fclose(fin);
    fclose(head);
    printf("fail to open %s", codePath);
    exit(-1);
  }
  if (!func_analyze(fin, head, code))
  {
    printf("syntax error!,fail to anlyze the function");
  }
  fclose(fin);
  fclose(head);
  fclose(code);
  return 0;
}


//����������,ֱ�������Ž���,����1,˵����������,����˵�������﷨����
int output_funcBody(FILE* fin,FILE* code){
  int leftBra=1;  //����������Ϊ1,Ȼ����л���
  fprintf(code,"{");
  while(leftBra!=0){
    char c=fgetc(fin);
    fputc(c,code);
    if(c==EOF) return 0;
    if(c=='{') leftBra++;
    else if(c=='}') leftBra--;
  }
  if(fgetc(fin)!='\n') return 0;  //����ȡһ�����з�
  fprintf(code,"\n");
  return 1;
}


//����ͷ����,ֱ������һ�������Ž���
//��������ļ��ľ�ͷ,����EOF
int head_analyze(FILE* fin,FILE* head){
  char* stops=" -();,{}}\n";
  //���ȶ�ȡ����
  char end;
  char tmp[2000];
  char returnType[200];
  char owner[200];
  char funcName[200];
  char args[200]; //��������б�,�����汣������б��ʽΪtype1 varname1,type2 varname2������,������ǰ������
  //����,����ͷ�Ĵ�С�����޵�,������{����,�м���ܳ��ֵĳɷ������Ͷ����,->,����,С����,id
  end=myfgets(tmp,"\n|{}",fin);
  //�����ȡ�����ļ���ͷ,����EOF
  if(end==EOF){
    return EOF;
  }
  if(end!='{') return 0;
  char tmp2[1000];
  end=mysgets(tmp2,">",tmp);
  char* pst=tmp;  //��pst��¼��ȡ�����
  //�������>��β��,�ú����ǽṹ�巽��
  if(end=='>'){
    int len=strlen(tmp2);
    if(tmp2[len-1]!='-') return 0;
    tmp2[len-1]='\0';  //�����һλ
    if(tmp2[len-2]==' ') tmp2[len-2]='\0';
    strcpy(owner,tmp2);
    pst+=len+1;
    if(*pst==' ') pst++;
  }
  //�������ȫ�ַ���,����������ΪNULL
  else{
    strcpy(owner,ownerString);
    pst=tmp;
  }
  //Ȼ���ȡ����ֵ
  end=mysgets(tmp2,stops,pst);
  pst+=strlen(tmp2)+1;
  if(end!=' ') return 0;
  if(isTypeDefKeyWords(tmp2)){
    int i=0;
    strcpy(returnType,tmp2);
    i+=strlen(tmp2);
    returnType[i++]=' ';
    end=mysgets(tmp2,stops,pst);
    pst+=strlen(tmp2)+1;
    if(end!=' ') return 0;
    strcpy(returnType+i,tmp2);
  }
  //�����ǻ����������ͻ���typedefת�����ͱ���
  else{
    strcpy(returnType,tmp2);
  }
  //Ȼ���ȡ������
  end=mysgets(tmp2,stops,pst);
  pst+=strlen(tmp2)+1;
  if(end==' '||end=='('){
    if(end==' '){
      if(*pst!='(') return 0;
      pst++;
    }
    strcpy(funcName,tmp2);
  }else return 0;

  char* argsp=args;   //ָ��args��ͷ��ָ��
  *argsp='\0';
  //Ȼ���ȡ�����б�,��ȡ�����Ż��߿ո񶼿���
  while((end=mysgets(tmp2,",) ",pst))!='\0'&&end!=')'){
    pst+=strlen(tmp2)+1;
    //������ݲ�Ϊ��,д������
    if(strcmp(tmp2,"")!=0){
      argsp+=sprintf(argsp,"%s",tmp2);
      //������Զ��Ž�β��,д�붺��,����д��ո�
      if(end==',') argsp+=sprintf(argsp,",");
      else argsp+=sprintf(argsp," ");
      continue;
    }
    if(end==',') argsp+=sprintf(argsp,",");
  }
  if(end!=')') return 0;
  //������ǰ���һ�ζ�ȡ���Ĳ���д���ļ�
  if (strcmp(tmp2, "") != 0)
  {
    sprintf(argsp, "%s", tmp2);
  }
  //�����и�ʽ��д��
  fprintf(head,"%s|%s|%s|(%s)\n",funcName,owner,returnType,args);
  return 1;
}



//�����ļ�����
int func_analyze(FILE* fin,FILE* head,FILE* code){
  //����ļ�û�д������ѭ����ȡ����
  int jud;
  char tmp[2000];
  char end;
  //����������ļ���ͷ,����EOF,
  //��������ݿ���������Ϊ����ͷ����,����1
  //������ݲ����Ϻ���ͷ�﷨Ҫ��,����0
  int i=0;
  while((jud=head_analyze(fin,head))!=EOF&&jud!=0){
    printf("%d\n",i++);
    //�Ѵ��������д���ļ�,���뺯�����������
    if(!output_funcBody(fin,code)) return 0;
    printf("@%d\n",i++);
  }
  if(jud==0) return 0;
  return 1;
}



