#include "token.h"

//从文件中读取一个token,动态分配空间,如果读取到尽头,则返回的token的字符串属性为NULL
struct token getToken(FILE* fin){
  struct token out;
  int jud=fscanf(fin,"%d",&out.kind); //读数直到遇到空格
  if(jud!=1){
    out.val=NULL;
    return out;
  }
  fgetc(fin); //读出空格
  char tmp[1000];
  char end=myfgets(tmp,"\n",fin); //读字符串直到遇到换行符
  if(end=='\n'){
    out.val=(char*)malloc(strlen(tmp)+1);
    strcpy(out.val,tmp);
  }
  else
    out.val==NULL;
  return out;
}

//往文件中退回读取的一个token
int ungetToken(FILE* fin,Token token){
  char re[200];
  if(sprintf(re,"%d %s\n",token.kind,token.val)<0) return 0;
  //退回之前读取的字符串
  int i=strlen(re)-1;
  while (i>=0)
  {
    ungetc(re[i--],fin);
  }
  return 1;
}


//把两个token连接起来组成新的token,使用sep字符串作为连接点内容
struct token connectToken(Token token1,Token token2,TokenKind newKind,char* sep){
  //计算合并需要的空间
  struct token out;
  int newSize=strlen(token1.val)+strlen(token2.val)+strlen(sep)+1;
  out.val=malloc(newSize);
  //使用链式编码
  strcpy(strcpy(strcpy(out.val,token1.val)+strlen(token1.val),sep)+strlen(sep),token2.val);
  out.kind=newKind;
  return out;
}


//获取一个self类型的token
Token getSelfToken(){
  Token out={
    .kind=SELF_KEYWORD,
    .val=strcpy(malloc(1+strlen(SELF_STRING_VALUE)),SELF_STRING_VALUE)
  };
  return out;
}

//释放给token分配的空间,释放空间之后就不应该再使用了
void delToken(Token token){
  free(token.val);
}

//token显示工具,根据类型和字符字面值换行输出token信息
int printToken(Token token){
  printf("(%s,%s)\n",tokenStrings[token.kind],token.val);
  return 1;
}

//往文件中写回token的符号值和字符字面值
int fputToken(Token token,FILE* fout){
  fprintf(fout,"%d %s\n",token.kind,token.val);
  fprintf(stdout,"%d %s\n",token.kind,token.val); //测试用
  return 1;
}