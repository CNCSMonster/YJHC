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

int token_mergeOp(FILE* fin,FILE* code);

int token_addlayer(FILE* fin,FILE* code);

//��ȡһ�����Ű������߼����ʽ����ȡ�ɹ����ط�0ֵ,���򷵻�0
int readCase(FILE* fin,FILE* code);

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
  char tmpPath[100];
  FILE *code = fopen(strcpy(tmpPath,"tmpTokens.tmp"), "w");
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

  //�����token����,����token���л����Խ����Ż���

  //��ȷ��token������һЩ==�������,��������ᴦ�������=��token������Ҫ�����Ǻϲ�
  fin=fopen(tmpPath,"r");
  char tmpPath2[100];
  code=fopen(strcpy(tmpPath2,"tmppath2.tmp"),"w");

  
  if(!token_mergeOp(fin,code)){
    printf("something wrong happen when merge operation symbol");
  }
  char tord[200];
  strcpy(strcpy(tord,"del ")+strlen(tord),tmpPath);
  fclose(fin);
  fclose(code);
  system(tord);


  fin=fopen(tmpPath2,"r");
  code=fopen(codePath,"w");
  if(!token_addlayer(fin,code)){
    printf("something wrong happen when add the layer");
  }
  strcpy(strcpy(tord,"del ")+strlen(tord),tmpPath2);
  fclose(fin);
  fclose(code);
  system(tord);
  return 0;
}



int token_mergeOp(FILE* fin,FILE* code){
  Token last;   //��һ��token,���ֻҪ��������token(��Ϊ�������token���ϳ�)
  Token cur;
  last=getToken(fin);
  if(last.val==NULL) return 0;
  cur=getToken(fin);
  while(cur.val!=NULL){
    //TODO �Ƚ�cur���ַ�����ֵ�͸����ַ���,���رȽϽ��
    //Ŀǰ���ҽ���Ϊop���߹ؼ��ֵ�ʱ����Ҫ�����Ƿ���ĳ��token�Ĳ���
    if(cur.kind==OP&&last.kind==OP){
      //�������ϲ��ö����ÿո�
      if(
        (strcmp(cur.val,"=")==0)
      ||(strcmp(cur.val,"+")==0&&strcmp(last.val,"+")==0)
      ||(strcmp(cur.val,"-")==0&&strcmp(last.val,"-")==0)
      ||(strcmp(cur.val,">")==0&&strcmp(last.val,">")==0)
      ||(strcmp(cur.val,"<")==0&&strcmp(last.val,"<")==0)
      ||(strcmp(cur.val,">")==0&&strcmp(last.val,"-")==0)     //�����ָ����ʽṹ���Ա��
      ||(strcmp(cur.val,"|")==0&&strcmp(last.val,"|")==0)
      ||(strcmp(cur.val,"&")==0&&strcmp(last.val,"&")==0)
      ){
        Token new=connectToken(last,cur,OP,"");
        delToken(last);
        last=new;
        delToken(cur);
        cur=getToken(fin);
      }
    }
    else if(cur.kind==IF){
      //
      if(last.kind==ELSE){
        Token new=connectToken(last,cur,ELIF," ");
        delToken(last);
        delToken(cur);
        last=new;
        cur=getToken(fin);
      }
    }
    //��ӡlast,����last
    fputToken(last,code);
    delToken(last);
    last=cur;
    //����
    cur=getToken(fin);
  }
  //�������һ�����߲�����
  if(last.val!=NULL){
    fputToken(last,code);
    delToken(last);
  }
  return 1;  
}

//�����鴦��,�����벹����,����for�ṹ��if�ṹ�������������û�л����ŵĻ������ϻ�����
int token_addlayer(FILE* fin,FILE* code){
  Token cur;    //��¼��ǰ��ȡ����token
  cur=getToken(fin);
  //�����ؼ��ֲŽ��в���,����ֱ��д��
  while(cur.val!=NULL){
    fputToken(cur,stdout);
    if(!isKeyForProcessControl(cur.val)){
      fputToken(cur,code);
      delToken(cur);
      cur=getToken(fin);
      continue;
    }
    //�����ȸ��������,��Ϊ��
    if(cur.kind==IF||cur.kind==ELIF||cur.kind==WHILE||cur.kind==FOR){
      //����������token����
      fputToken(cur,code);delToken(cur);
      if(!readCase(fin,code)){
        return 0;
      }
      cur=getToken(fin);
      if(cur.val==NULL) return 0;
      //�������ȱ����������ŵĻ�,����ϲ��
      if(cur.kind!=LEFT_BRACE){
        fputToken(leftBraceToken,code);
        //Ȼ�������һ�����,Ȼ������һ�����
        fputToken(cur,code);delToken(cur);cur=getToken(fin);
        while(cur.val!=NULL&&cur.kind!=SEMICOLON){
          fputToken(cur,code);delToken(cur);cur=getToken(fin);
        }
        if(cur.val==NULL) return 0;
        fputToken(cur,code);delToken(cur);cur=getToken(fin);
        fputToken(rightBraceToken,code);
      }
      //�������Ϊ{,���ӡȻ���������
      else{
        fputToken(cur,code);delToken(cur);cur=getToken(fin);
      }
    }
    //ֱ�Ӻ���Ϊ��
    else if(cur.kind==ELSE||cur.kind==DO){
      TokenKind tmpKind=cur.kind;
      fputToken(cur,code);delToken(cur);cur=getToken(fin);
      if(cur.val==NULL) return 0;
      //��������鲻���黨����,�򲹳�
      if(cur.kind!=LEFT_BRACE){
        fputToken(leftBraceToken,code);
        fputToken(cur,code);delToken(cur);cur=getToken(fin);
        while(cur.val!=NULL&&cur.kind!=SEMICOLON){
          fputToken(cur,code);delToken(cur);cur=getToken(fin);
        }
        if(cur.val==NULL) return 0;
        fputToken(cur,code);delToken(cur);cur=getToken(fin);
        fputToken(rightBraceToken,code);
        if(tmpKind==DO){
          if(cur.val==NULL) return 0;
          if(cur.kind!=WHILE){
            delToken(cur);
            return 0;
          }
          fputToken(cur,code);delToken(cur);
          if(!readCase(fin,code)){
            return 0;
          }
          cur=getToken(fin);
          if(cur.val==NULL) return 0;
          if(cur.kind!=SEMICOLON){
            delToken(cur);
            return 0;
          }
          fputToken(cur,code);delToken(cur);cur=getToken(fin);
        }
      }
      //��������
      else{
        fputToken(cur,code);delToken(cur);cur=getToken(fin);
      }
    }
    //���������continue,break,return֮��Ĺؼ���
    else{
      fputToken(cur,code);delToken(cur);cur=getToken(fin);
    }
  }
  return 1;
}


//��ȡһ�����Ű������߼����ʽ����ȡ�ɹ����ط�0ֵ,���򷵻�0
int readCase(FILE* fin,FILE* code){
  Token cur;
  cur=getToken(fin);
  if(cur.val==NULL) return 0;
  else if(cur.kind!=LEFT_PARENTHESIS){
    delToken(cur);
    return 0;
  }
  int leftP=1;
  fputToken(cur, code);
  delToken(cur);
  cur = getToken(fin); //�ѵ�һ��������д��
  while (cur.val != NULL)
  {
    fputToken(cur, code);
    if (cur.kind == RIGHT_PARENTHESIS)
    {
      if (leftP == 1)
      {
        delToken(cur);
        break;
      }
      else if (leftP < 1)
      {
        delToken(cur);
        return 0;
      }
      else
      {
        leftP--;
      }
    }
    else if (cur.kind == LEFT_PARENTHESIS)
      leftP++;
    delToken(cur);
    cur = getToken(fin);
  }
  return 1;
}




//����δʷ�����,ֱ�������һ����Ž���
int code_parse(FILE *fin, FILE *code)
{
  //�����ļ���д��һ�������Ų�����
  fprintf(code, "%d %c\n", LEFT_BRACE, '{');
  int leftPar = 1;
  //����token����
  char *stops = "+-*/^|&;,.><=[]() {}\"\n"; //��������֮ǰ���������������������
  char tmp[1000];
  char end;
  int tmpI; //�ݴ��õ�int
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
      //�ж��Ƿ������̿��ƹؼ���,���ﲻ����ϵ�����tokenһ���жϣ�����else��ֱ���жϳ�else����,���������ifҲ�����жϳ�elif����
      else if ((tmpI=isKeyForProcessControl(tmp))>=0)
      {
        fprintf(code, "%d %s\n", tmpI, tmp);
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
      else if (tmp[0] > 57 || tmp[0] < 48||tmp[0]=='_')
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
    //�ж��Ƿ��ǽ��
    else if((tmpI=isSep(end))>=0){
      if(end=='{') leftPar++;
      else if(end=='}') leftPar--;
      fprintf(code,"%d %c\n",tmpI,end);
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
  fprintf(code,"%d %c\n",RIGHT_BRACE,'}');
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



