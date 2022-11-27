#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mystring.h"
#include "mystring.c"
#include "token_kind.h"
#include "string.h"





#define ownerString "NULL"

//����δʷ�����,ֱ�������һ����Ž���
int code_parse(FILE* fin,FILE* code);
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
  // char* inputPath="../func.txt";
  // char* headPath="../head.txt";
  // char* codePath="../tokens.txt";
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
  FILE *code = fopen(codePath, "w");
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

//����δʷ�����,ֱ�������һ����Ž���
int code_parse(FILE *fin, FILE *code)
{
  //�����ļ���д��һ�������Ų�����
  fprintf(code, "%d %c\n", SEP, '{');
  int leftPar = 1;
  //����token����
  char *stops = "+-*/^|&;,.><=[]() {}\"\n"; //��������֮ǰ���������������������
  char tmp[1000];
  char end;
  end = myfgets(tmp, stops, fin);
  while (end != '}' || leftPar > 1)
  {
    printf("^%s$\n",tmp);
    //�Զ�ȡ�������ݽ��дʷ�����
    //�������������ݲ�Ϊ��ʱ�����Ҫ���дʷ�����
    if (strcmp(tmp, "") != 0)
    {
      //�ж��Ƿ��ǻ�����������
      if (isBaseType(tmp))
      {
        fprintf(code, "%d %s\n", TYPE, tmp);
      }
      //�ж��Ƿ������Ͷ���ؼ���
      //��������Ͷ���ؼ���,��������һ���ַ���һ�����������
      else if (isTypeDefKeyWords(tmp))
      {
        if (end != ' ')
          return 0; //�����Ӧ�����Կո��β��
        fprintf(code, "%d %s ", TYPE, tmp);
        end = myfgets(tmp, stops, fin);
        fprintf(code, "%s\n", tmp);
      }
      //�ж��Ƿ������̿��ƹؼ���
      else if (isKeyForProcessControl(tmp))
      {
        fprintf(code, "%d %s\n", CONTROL, tmp);
      }
      //�ж��Ƿ�������
      else if (strspn(tmp, "0123456789") == strlen(tmp))
      {
        //�жϺ����Ƿ���С����,�����С����,����С��
        if (end == '.')
        {
          fprintf(code, "%d %s.", CONST, tmp);
          end = myfgets(tmp, stops, fin);
          //���ֺ������С����,�����һ������������
          if (strspn(tmp, "0123456789") != strlen(tmp))
          {
            return 0;
          }
          //
          fprintf(code, "%s\n", tmp);
        }
        //�������С���㣬��������
        else
        {
          fprintf(code, "%d %s\n", CONST, tmp);
        }
      }
      //�ж��Ƿ�����(�����������������������Լ�������,Ӧ�ò������ֿ�ͷ,��Ϊ��ȷ����������,����ΪUNKOWN����)
      else if (tmp[0] > 57 || tmp[0] < 48)
      {
        fprintf(code, "%d %s\n", UNKNOWN, tmp); //����ȷ�����������Ǻ�����
      }
      //����������﷨����������,��ʾ�﷨����
      else
      {
        return 0;
      }
    }

    //Ȼ��Զ������ս���Ž����ж�
    if(end==' '){
      end=myfgets(tmp,stops,fin);
      continue;
    }  //�ո�������token
    //�ж��Ƿ����ַ�������˫����
    if(end=='\"'){
      //�����,���ȡ���ַ���ʣ�²���
      end=freadRestString(fin,tmp);
      if(!end) return 0;  //����������ַ���
      //����
      fprintf(code,"%d \"%s\"\n",CONST,tmp);
    }
    else if(end=='{'){
      leftPar++;
      fprintf(code,"%d %c\n",SEP,end);
    }
    else if(end=='}'){
      leftPar--;
      fprintf(code,"%d %c\n",SEP,end);
    }
    //�ж��Ƿ����������
    else if(isSep(end)){
      fprintf(code,"%d %c\n",SEP,end);
    }
    //�ж��Ƿ���������������������ɲ���
    else if(isOp(end)){
      fprintf(code,"%d %c\n",OP,end);
    }
    else{
      return 0;
    }
    //������ôζ�ȡ���ݺ�����һ�ζ�ȡ
    end=myfgets(tmp,stops,fin);
  }
  if(end!='}') return 0;
  //���һ�ж�ȡ������Ӧ��Ϊ��
  if(strcmp(tmp,"")!=0) return 0;
  //ѭ����ȡ,�Ѻ���һ�����з���ȡ����
  while((end=myfgets(tmp,"\n",fin))!='\n');
  if(end!='\n') return 0; //����ĸ�ʽ�����Ӧ����һ�����з�
  fprintf(code,"%d %c\n",SEP,'}');
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
    if(!code_parse(fin,code)){
      return 0;
    }
    printf("@%d\n",i++);
  }
  if(jud==0) return 0;
  return 1;
}



