#include "func_body_parser.h"


#define ERR printf("something wrong in parser")

//�ڶ��α������ϲ������
int token_mergeOp(FILE* fin,FILE* code);

//�����α����������̿�����ȱʡ�Ľ��
int token_addlayer(FILE* fin,FILE* code);

//���Ĵα���,�²�ָ��,����,����,ʹ��ǰ����ͳ�ǰ����,TODO
int token_guess(FILE* fin,FILE* code);

//�����ɨ��,ȷ������ָ��,������ָ��������Ϊ����
int token_findFuncPointer();

//ps�Ĵα������󲻴���unknown���͵�token,����token����ȷ��Ϊ����/���/������/������/����


//��ȡһ�����Ű������߼����ʽ����ȡ�ɹ����ط�0ֵ,���򷵻�0
int readCase(FILE* fin,FILE* code);

//����δʷ�����,�����������ſ�ʼ,ֱ��������Ӧ�һ����Ž���
int code_parse(FILE* fin,FILE* code);


//���ھ���func_split������ĺ�������Ϣ���дʷ�����,�õ��ʷ���Ԫ����
//���ڲ²���дʷ�����,���԰�id����λ�ò²�����ͻ��߱���������
//ʹ�ö�α�������������
//����gcc func_body_parsseer.c -o fbp
//ʹ��fbp bodyPath tokensPath
int main(int argc,char* argv[]){
  char* bodyPath=argv[1];
  char* tokensPath=argv[2];
  // char* bodyPath="../out/func_body.txt";
  // char* tokensPath="../out/func_tokens.txt";
  char prePath[300];
  char curPath[300];
  char ord[400];
  int jud=sprintf(curPath,"%s1st",tokensPath);
  if(jud<0) exit(-1);
  FILE* body=fopen(bodyPath,"r");
  if(body==NULL) exit(-1);
  FILE* tokens=fopen(curPath,"w");
  if(tokens==NULL){
    fclose(body);exit(-1);
  }
  //���Ƚ���һ�α���
  while(jud=code_parse(body,tokens));
  fclose(body);
  fclose(tokens);
  if(!jud) ERR;
  //Ȼ����еڶ��α���,�ϲ������
  strcpy(prePath,curPath);
  jud=sprintf(curPath,"%s2nd",tokensPath);
  if(jud<0) exit(-1);
  body=fopen(prePath,"r");
  if(body==NULL) exit(-1);
  tokens=fopen(curPath,"w");
  if(tokens==NULL){
    fclose(body);exit(-1);
  }
  if(!token_mergeOp(body,tokens)) ERR;
  fclose(body);fclose(tokens);

  //���е����α���,�������̿�����ȱʡ�Ľ��
  strcpy(prePath,curPath);
  jud=sprintf(curPath,"%s3rd",tokensPath);
  if(jud<0) exit(-1);
  body=fopen(prePath,"r");
  if(body==NULL) exit(-1);
  tokens=fopen(curPath,"w");
  if(tokens==NULL){
    fclose(body);exit(-1);
  }
  if(!token_addlayer(body,tokens)) ERR;
  fclose(body);fclose(tokens);

  //���е��Ĵα���,��δ֪token���в²�
  if(jud<0) exit(-1);
  body=fopen(curPath,"r");
  if(body==NULL) exit(-1);
  tokens=fopen(tokensPath,"w");
  if(tokens==NULL){
    fclose(body);exit(-1);
  }
  if(!token_guess(body,tokens)) ERR;
  fclose(body);fclose(tokens);

  // system("dir");
  // printf("%s,%s",bodyPath,tokensPath);
  //�鿴�м��ļ�
  //���ɾ���м��ļ�,
  // sprintf(ord,"del %s1st",tokensPath);
  // mystrReplace(ord,'/','\\');
  // // printf("\n%s",ord);
  // system(ord);
  // sprintf(ord,"del %s2nd",tokensPath);
  // // printf("\n%s",ord);
  // mystrReplace(ord,'/','\\');
  // system(ord);
  // sprintf(ord,"del %s3rd",tokensPath);
  // mystrReplace(ord,'/','\\');
  // // printf("\n%s",ord);
  system(ord);
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
  else if(cur.kind!=LEFT_PAR){
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
    if (cur.kind == RIGHT_PAR)
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
    else if (cur.kind == LEFT_PAR)
      leftP++;
    delToken(cur);
    cur = getToken(fin);
  }
  return 1;
}

//���Ĵα���,�²�ָ��,����,����,ʹ��ǰ����ͳ�ǰ����,ȥ������δ֪token
int token_guess(FILE* fin,FILE* code){
  //һ��δ֪�������Ǳ�����Ҳ�����ǳ�����,Ҳ���������Ͷ���,���߱��������ߺ�����
  //����������Ҫֱ��ǰ��һ�����͵�ֵ
  Token pre=getToken(fin);
  if(pre.val==NULL) return 0;
  Token cur=(Token){
    // .kind=UNKNOWN,
    .val=NULL
  };
  Token next;
  while((cur=getToken(fin)).val!=NULL){
    //�����ǰ������TYPE����,������Ӻ�������п��ܵ�ָ��һ������µ�type����
    if(cur.kind==TYPE){
      while((next=getToken(fin)).val!=NULL&&strcmp(next.val,"*")==0){
        Token tmpCur=connectToken(cur,next,TYPE,"");
        delToken(cur);
        delToken(next);
        cur=tmpCur;
      }
      //����typedef���ܵ����
      if(pre.kind==TYPEDEF_KEYWORD&&next.kind==UNKNOWN&&next.val!=NULL){
        next.kind=TYPE;
      }
      if(next.val!=NULL){
        ungetToken(fin,next);delToken(next);
      }
      fputToken(pre,code);
      delToken(pre);
      pre=cur;
      continue;
    }
    //���������ǰ���Ͳ���unknown����,�򲻲²�,�˳���ǰѭ��
    if(cur.kind!=UNKNOWN){
      fputToken(pre,code);
      delToken(pre);
      pre=cur;
      continue;
    }
    //���ݵ�ǰtoken���ݲ²�����,�����һ��token�ǷֺŻ��߶���,˵������һ�仰�Ŀ�ʼ
    //�����Ǳ��������б�Ĳ���,������������Ƕ������
    //���� int m,int a,b=1;
    next=getToken(fin); //��ǰ������һ��token
    if(next.val==NULL){
      delToken(cur);
      delToken(pre);
      return 0;
    }
    //�����һ��token��������,���next�Ǻ�����,����Ψһһ��������Ǻ������õĺ����������
    if(next.kind==LEFT_PAR){
      cur.kind=FUNC;
    }
    //�����һ������typedef,���������type����һ��������type
    else if(pre.kind==TYPEDEF_KEYWORD){
      cur.kind=TYPE;
      next.kind=TYPE;
    }
    //���curtoken��ĳ�����Ŀ�ͷ������һ��token��δ֪��,�����token�����Ͷ���,��һ��token�Ǳ�����
    else if((pre.kind==SEMICOLON||pre.kind==LEFT_PAR||pre.kind==COMMA||pre.kind==CONST_KEYWORD)&&next.kind==UNKNOWN){
      cur.kind=TYPE;
      next.kind=VAR;
    }
    //���cur token�ǿ�ͷtoken,������һ��token��*,�����token�Ǹ����Ͷ���
    else if((pre.kind==SEMICOLON||pre.kind==LEFT_PAR||pre.kind==COMMA||pre.kind==CONST_KEYWORD)
      &&strcmp(next.val,"*")==0
    ){
      Token tmpCur=connectToken(cur,next,TYPE,"");
      delToken(cur);delToken(next);
      cur=tmpCur;
      while((next=getToken(fin)).val!=NULL&&strcmp(next.val,"*")==0){
        tmpCur=connectToken(cur,next,TYPE,"");
        delToken(cur);delToken(next);
        cur=tmpCur;
      }
    }
    //���򶼲²��Ǳ���
    else{
      cur.kind=VAR;
    }
    if(next.val!=NULL){
      ungetToken(fin,next);
      delToken(next);
    }
    fputToken(pre,code);
    delToken(pre);
    pre=cur;
  }
  // д��pre������
  fputToken(pre,code);
  delToken(pre);
  return 1;
}

//�����ɨ��,ȷ������ָ��,������ָ��������Ϊ����
int token_findFuncPointer(){
  //TODO
}




//����δʷ�����,ֱ�������һ����Ž���,��һ��
int code_parse(FILE *fin, FILE *code)
{
  //��ȡһ�������ſ�ʼ
  if(fgetc(fin)!='{') return 0;
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
      //�ж��Ƿ��ǳ����������η�const
      else if(strcmp(tmp,"const")==0){
        fprintf(code,"%d %s\n",CONST_KEYWORD,tmp);
      }
      //�ж��Ƿ��������������ؼ���typedef
      else if(strcmp(tmp,"typedef")==0){
        fprintf(code,"%d %s\n",TYPEDEF_KEYWORD,tmp);
      }
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

