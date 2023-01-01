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


//导出函数体,直到花括号结束,返回1,说明导出正常,否则说明存在语法错误
int output_funcBody(FILE* fin,FILE* code);
//函数头分析,直到遇到一个左括号结束
int head_analyze(FILE *fin, FILE *head);
//函数文件分析
int func_analyze(FILE *fin, FILE *head, FILE *code);

//字符串输出需要的结果,使用

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


//导出函数体,直到花括号结束,返回1,说明导出正常,否则说明存在语法错误
int output_funcBody(FILE* fin,FILE* code){
  int leftBra=1;  //计数左花括号为1,然后进行换行
  fprintf(code,"{");
  while(leftBra!=0){
    char c=fgetc(fin);
    fputc(c,code);
    if(c==EOF) return 0;
    if(c=='{') leftBra++;
    else if(c=='}') leftBra--;
  }
  if(fgetc(fin)!='\n') return 0;  //最后读取一个换行符
  fprintf(code,"\n");
  return 1;
}


//函数头分析,直到遇到一个左括号结束
//如果到了文件的尽头,返回EOF
int head_analyze(FILE* fin,FILE* head){
  char* stops=" -();,{}}\n";
  //首先读取类型
  char end;
  char tmp[2000];
  char returnType[200];
  char owner[200];
  char funcName[200];
  char args[200]; //保存参数列表,这里面保存参数列表格式为type1 varname1,type2 varname2这样子,不保留前后括号
  //首先,函数头的大小是有限的,而且以{结束,中间可能出现的成分有类型定义符,->,逗号,小括号,id
  end=myfgets(tmp,"\n|{}",fin);
  //如果读取到了文件尽头,返回EOF
  if(end==EOF){
    return EOF;
  }
  if(end!='{') return 0;
  char tmp2[1000];
  end=mysgets(tmp2,">",tmp);
  char* pst=tmp;  //用pst记录读取的起点
  //如果是以>结尾的,该函数是结构体方法
  if(end=='>'){
    int len=strlen(tmp2);
    if(tmp2[len-1]!='-') return 0;
    tmp2[len-1]='\0';  //整理短一位
    if(tmp2[len-2]==' ') tmp2[len-2]='\0';
    strcpy(owner,tmp2);
    pst+=len+1;
    if(*pst==' ') pst++;
  }
  //否则就是全局方法,主人名设置为NULL
  else{
    strcpy(owner,ownerString);
    pst=tmp;
  }
  //然后读取返回值
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
  //否则是基础数据类型或者typedef转的类型别名
  else{
    strcpy(returnType,tmp2);
  }
  //然后读取函数名
  end=mysgets(tmp2,stops,pst);
  pst+=strlen(tmp2)+1;
  if(end==' '||end=='('){
    if(end==' '){
      if(*pst!='(') return 0;
      pst++;
    }
    strcpy(funcName,tmp2);
  }else return 0;

  char* argsp=args;   //指向args开头的指针
  *argsp='\0';
  //然后读取参数列表,读取到逗号或者空格都可以
  while((end=mysgets(tmp2,",) ",pst))!='\0'&&end!=')'){
    pst+=strlen(tmp2)+1;
    //如果内容不为空,写入内容
    if(strcmp(tmp2,"")!=0){
      argsp+=sprintf(argsp,"%s",tmp2);
      //如果是以逗号结尾的,写入逗号,否则写入空格
      if(end==',') argsp+=sprintf(argsp,",");
      else argsp+=sprintf(argsp," ");
      continue;
    }
    if(end==',') argsp+=sprintf(argsp,",");
  }
  if(end!=')') return 0;
  //把括号前最后一次读取到的参数写入文件
  if (strcmp(tmp2, "") != 0)
  {
    sprintf(argsp, "%s", tmp2);
  }
  //最后进行格式化写入
  fprintf(head,"%s|%s|%s|(%s)\n",funcName,owner,returnType,args);
  return 1;
}



//函数文件分析
int func_analyze(FILE* fin,FILE* head,FILE* code){
  //如果文件没有处理完就循环读取处理
  int jud;
  char tmp[2000];
  char end;
  //如果处理到了文件尽头,返回EOF,
  //如果有内容可以正常作为函数头处理,返回1
  //如果内容不符合函数头语法要求,返回0
  int i=0;
  while((jud=head_analyze(fin,head))!=EOF&&jud!=0){
    printf("%d\n",i++);
    //把代码段内容写入文件,分离函数代码段内容
    if(!output_funcBody(fin,code)) return 0;
    printf("@%d\n",i++);
  }
  if(jud==0) return 0;
  return 1;
}



