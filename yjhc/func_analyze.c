#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mystring.h"
#include "mystring.c"
#include "token_kind.h"






#define ownerString "NULL"

//代码段词法分析,直到遇到右花括号结束
int code_parse(FILE* fin,FILE* code);
//函数头分析,直到遇到一个左括号结束
int head_analyze(FILE* fin,FILE* head);
//函数文件分析
int func_analyze(FILE* fin,FILE* head,FILE* code);

int main(int argc,char* argv[]){
  if(!argc==4){
    printf("func analysis need and only need 3 argument!input path, func head path,and code path!");
    exit(-1);
  }
  char* inputPath=argv[1];
  char* headPath=argv[2];
  char* codePath=argv[3];
  // char* inputPath="../func.txt";
  // char* headPath="../head.txt";
  // char* codePath="../tokens.txt";
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
    printf("fail to open %s",codePath);
    exit(-1);
  }
  if(!func_analyze(fin,head,code)){
    printf("syntax error!,fail to anlyze the function");
  }
  fclose(fin);fclose(head);fclose(code);
  return 0;
}



//代码段词法分析,直到遇到右花括号结束
int code_parse(FILE* fin,FILE* code){
  //先往文件中写入一个花括号并换行
  fprintf(code,"%d %s\n",SEP,'{');
  int leftPar=1;
  //函数token分析
  char* stops="+-*/^|&;,() {}\"\n"; //读到换行之前都是属于这个函数的内容
  char tmp[1000];
  char end;
  while((end=myfgets(tmp,stops,fin))!='}'||leftPar>1){
    //对读取到的内容进行词法分析
    //仅仅读到的内容不为的时候才需要进行词法分析
    if(strcmp(tmp,"")!=0){
      //判断是否是基础数据类型
      if(isBaseType(tmp)){
        fprintf(code,"%d %s\n",TYPE,tmp);
      }
      //判断是否是类型定义关键字
      //如果是类型定义关键字,则与后面的一个字符串一起组成类型名
      else if(isTypeDefKeyWords(tmp)){
        if(end!=' ') return 0;  //则后面应该是以空格结尾的
        fprintf(code,"%d %s ",TYPE,tmp);
        end=myfgets(tmp,stops,fin);
        fprintf(code,"%s\n",end);
      }
      //判断是否是流程控制关键字
      else if(isKeyForProcessControl(tmp)){
        fprintf(code,"%d %s\n",CONTROL,tmp);
      }
      //否则是unknown类型等待以后分析      
      else{
        fprintf(code,"%d %s\n",UNKOWN,tmp);
      }
    }
    //然后对读到的终结符号进行判断
    if(end==' ') continue;  //空格不用生成token
    if(end=='{'){
      leftPar++;
      fprintf(code,"%d %c\n",SEP,end);
    }
    else if(end=='}'){
      leftPar--;
      fprintf(code,"%d %c",SEP,end);
    }
    //判断是否是其他界符
    else if(isSep(end)){
      fprintf(code,"%d %c",SEP,end);
    }
    //判断是否是单独存在的运算符
    else if(isSingleOperation(end)){
      fprintf(code,"%d %c",OP,end);
    }
    //TODO 否则可能是复合的运算符,要超前搜索一位判断

  }
  if(end!='}') return 0;
  //最后一行读取的内容应该为空
  if(strcmp(tmp,"")!=0) return 0;
  //循环读取,把后面一个换行符读取出来
  while((end=myfgets(tmp,"\n",fin))!='\n');
  if(end!='\n') return 0; //合理的格式最后面应该是一个换行符
  fprintf(code,"%d %s\n",SEP,'}');
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
  end=myfgets(tmp,"\n;#@^%%<|{}",fin);
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
  while((jud=head_analyze(fin,head))!=EOF&&jud!=0){
    if(!code_parse(fin,code)){
      return 0;
    }
  }
  if(jud==0) return 0;
  return 1;
}