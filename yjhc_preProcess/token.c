#include "token.h"

//���ļ��ж�ȡһ��token,��̬����ռ�,�����ȡ����ͷ,�򷵻ص�token���ַ�������ΪNULL
struct token getToken(FILE* fin){
  struct token out;
  int jud=fscanf(fin,"%d",&out.kind); //����ֱ�������ո�
  if(jud!=1){
    out.val=NULL;
    return out;
  }
  fgetc(fin); //�����ո�
  char tmp[1000];
  char end=myfgets(tmp,"\n",fin); //���ַ���ֱ���������з�
  if(end=='\n'){
    out.val=(char*)malloc(strlen(tmp)+1);
    strcpy(out.val,tmp);
  }
  else
    out.val==NULL;
  return out;
}

//���ļ����˻ض�ȡ��һ��token
int ungetToken(FILE* fin,Token token){
  char re[200];
  if(sprintf(re,"%d %s\n",token.kind,token.val)<0) return 0;
  //�˻�֮ǰ��ȡ���ַ���
  int i=strlen(re)-1;
  while (i>=0)
  {
    ungetc(re[i--],fin);
  }
  return 1;
}


//������token������������µ�token,ʹ��sep�ַ�����Ϊ���ӵ�����
struct token connectToken(Token token1,Token token2,TokenKind newKind,char* sep){
  //����ϲ���Ҫ�Ŀռ�
  struct token out;
  int newSize=strlen(token1.val)+strlen(token2.val)+strlen(sep)+1;
  out.val=malloc(newSize);
  //ʹ����ʽ����
  strcpy(strcpy(strcpy(out.val,token1.val)+strlen(token1.val),sep)+strlen(sep),token2.val);
  out.kind=newKind;
  return out;
}


//��ȡһ��self���͵�token
Token getSelfToken(){
  Token out={
    .kind=SELF_KEYWORD,
    .val=strcpy(malloc(1+strlen(SELF_STRING_VALUE)),SELF_STRING_VALUE)
  };
  return out;
}

//�ͷŸ�token����Ŀռ�,�ͷſռ�֮��Ͳ�Ӧ����ʹ����
void delToken(Token token){
  free(token.val);
}

//token��ʾ����,�������ͺ��ַ�����ֵ�������token��Ϣ
int printToken(Token token){
  printf("(%s,%s)\n",tokenStrings[token.kind],token.val);
  return 1;
}

//���ļ���д��token�ķ���ֵ���ַ�����ֵ
int fputToken(Token token,FILE* fout){
  fprintf(fout,"%d %s\n",token.kind,token.val);
  fprintf(stdout,"%d %s\n",token.kind,token.val); //������
  return 1;
}