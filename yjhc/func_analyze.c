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

//读取一个括号包裹的逻辑表达式，读取成功返回非0值,否则返回0
int readCase(FILE* fin,FILE* code);

//代码段词法分析,直到遇到右花括号结束
int code_parse(FILE* fin,FILE* code);
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

  //输出了token序列,但是token序列还可以进行优化，

  //精确化token，对于一些==的运算符,上述处理会处理成两个=的token，现在要把它们合并
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
  Token last;   //上一个token,最多只要保留两个token(因为最多两个token来合成)
  Token cur;
  last=getToken(fin);
  if(last.val==NULL) return 0;
  cur=getToken(fin);
  while(cur.val!=NULL){
    //TODO 比较cur的字符字面值和各种字符串,返回比较结果
    //目前当且仅当为op或者关键字的时候需要考试是否是某个token的部分
    if(cur.kind==OP&&last.kind==OP){
      //运算符组合不用都不用空格
      if(
        (strcmp(cur.val,"=")==0)
      ||(strcmp(cur.val,"+")==0&&strcmp(last.val,"+")==0)
      ||(strcmp(cur.val,"-")==0&&strcmp(last.val,"-")==0)
      ||(strcmp(cur.val,">")==0&&strcmp(last.val,">")==0)
      ||(strcmp(cur.val,"<")==0&&strcmp(last.val,"<")==0)
      ||(strcmp(cur.val,">")==0&&strcmp(last.val,"-")==0)     //如果是指针访问结构体成员符
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
    //打印last,更新last
    fputToken(last,code);
    delToken(last);
    last=cur;
    //更新
    cur=getToken(fin);
  }
  //处理最后一个或者不处理
  if(last.val!=NULL){
    fputToken(last,code);
    delToken(last);
  }
  return 1;  
}

//第三遍处理,给代码补充层次,比如for结构和if结构如果条件语句后面没有花括号的话补充上花括号
int token_addlayer(FILE* fin,FILE* code){
  Token cur;    //记录当前读取到的token
  cur=getToken(fin);
  //读到关键字才进行操作,否则直接写回
  while(cur.val!=NULL){
    fputToken(cur,stdout);
    if(!isKeyForProcessControl(cur.val)){
      fputToken(cur,code);
      delToken(cur);
      cur=getToken(fin);
      continue;
    }
    //后面先跟条件语句,再为块
    if(cur.kind==IF||cur.kind==ELIF||cur.kind==WHILE||cur.kind==FOR){
      //读条件语句的token出来
      fputToken(cur,code);delToken(cur);
      if(!readCase(fin,code)){
        return 0;
      }
      cur=getToken(fin);
      if(cur.val==NULL) return 0;
      //如果后面缺乏块包裹符号的话,添加上层次
      if(cur.kind!=LEFT_BRACE){
        fputToken(leftBraceToken,code);
        //然后输出下一个语句,然后输出右花括号
        fputToken(cur,code);delToken(cur);cur=getToken(fin);
        while(cur.val!=NULL&&cur.kind!=SEMICOLON){
          fputToken(cur,code);delToken(cur);cur=getToken(fin);
        }
        if(cur.val==NULL) return 0;
        fputToken(cur,code);delToken(cur);cur=getToken(fin);
        fputToken(rightBraceToken,code);
      }
      //如果后面为{,则打印然后正常输出
      else{
        fputToken(cur,code);delToken(cur);cur=getToken(fin);
      }
    }
    //直接后面为块
    else if(cur.kind==ELSE||cur.kind==DO){
      TokenKind tmpKind=cur.kind;
      fputToken(cur,code);delToken(cur);cur=getToken(fin);
      if(cur.val==NULL) return 0;
      //如果后面检查不到块花括号,则补充
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
      //否则跳过
      else{
        fputToken(cur,code);delToken(cur);cur=getToken(fin);
      }
    }
    //否则可能是continue,break,return之类的关键字
    else{
      fputToken(cur,code);delToken(cur);cur=getToken(fin);
    }
  }
  return 1;
}


//读取一个括号包裹的逻辑表达式，读取成功返回非0值,否则返回0
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
  cur = getToken(fin); //把第一个左括号写入
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




//代码段词法分析,直到遇到右花括号结束
int code_parse(FILE *fin, FILE *code)
{
  //先往文件中写入一个花括号并换行
  fprintf(code, "%d %c\n", LEFT_BRACE, '{');
  int leftPar = 1;
  //函数token分析
  char *stops = "+-*/^|&;,.><=[]() {}\"\n"; //读到换行之前都是属于这个函数的内容
  char tmp[1000];
  char end;
  int tmpI; //暂存用的int
  end = myfgets(tmp, stops, fin);
  while (end != '}' || leftPar > 1)
  {
    printf("^%s$\n",tmp);
    //对读取到的内容进行词法分析
    //仅仅读到的内容不为的时候才需要进行词法分析
    if (strcmp(tmp, "") != 0)
    {
      //判断是否是基础数据类型
      if (isBaseType(tmp))
      {
        fprintf(code, "%d %s\n", TYPE, tmp);
      }
      //判断是否是类型定义关键字
      //如果是类型定义关键字,则与后面的一个字符串一起组成类型名
      else if (isTypeDefKeyWords(tmp))
      {
        if (end != ' ')
          return 0; //则后面应该是以空格结尾的
        fprintf(code, "%d %s ", TYPE, tmp);
        end = myfgets(tmp, stops, fin);
        fprintf(code, "%s\n", tmp);
      }
      //判断是否是流程控制关键字,这里不会联系后面的token一起判断，对于else会直接判断成else类型,就算后面有if也不会判断成elif类型
      else if ((tmpI=isKeyForProcessControl(tmp))>=0)
      {
        fprintf(code, "%d %s\n", tmpI, tmp);
      }
      //判断是否是数字
      else if (strspn(tmp, "0123456789") == strlen(tmp))
      {
        //判断后面是否是小数点,如果是小数点,就是小数
        if (end == '.')
        {
          fprintf(code, "%d %s.", CONST, tmp);
          end = myfgets(tmp, stops, fin);
          //数字后面跟着小数点,则后面一定是整数序列
          if (strspn(tmp, "0123456789") != strlen(tmp))
          {
            return 0;
          }
          //
          fprintf(code, "%s\n", tmp);
        }
        //如果不是小数点，就是整数
        else
        {
          fprintf(code, "%d %s\n", CONST, tmp);
        }
      }
      //判断是否是名(包括变量名常量名数组名以及函数名,应该不以数字开头,因为不确定是哪种名,定义为UNKOWN类型)
      else if (tmp[0] > 57 || tmp[0] < 48||tmp[0]=='_')
      {
        fprintf(code, "%d %s\n", UNKNOWN, tmp); //还不确定是量名还是函数名
      }
      //如果出现了语法定义外的情况,提示语法错误
      else
      {
        return 0;
      }
    }

    //然后对读到的终结符号进行判断
    if(end==' '){
      end=myfgets(tmp,stops,fin);
      continue;
    }  //空格不用生成token
    //判断是否是字符串的左双引号
    if(end=='\"'){
      //如果是,则读取出字符串剩下部分
      end=freadRestString(fin,tmp);
      if(!end) return 0;  //如果读不出字符串
      //否则
      fprintf(code,"%d \"%s\"\n",CONST,tmp);
    }
    //判断是否是界符
    else if((tmpI=isSep(end))>=0){
      if(end=='{') leftPar++;
      else if(end=='}') leftPar--;
      fprintf(code,"%d %c\n",tmpI,end);
    }
    //判断是否是运算符或者运算符的组成部分
    else if(isOp(end)){
      fprintf(code,"%d %c\n",OP,end);
    }
    else{
      return 0;
    }
    //处理完该次读取内容后获得下一次读取
    end=myfgets(tmp,stops,fin);
  }
  if(end!='}') return 0;
  //最后一行读取的内容应该为空
  if(strcmp(tmp,"")!=0) return 0;
  //循环读取,把后面一个换行符读取出来
  while((end=myfgets(tmp,"\n",fin))!='\n');
  if(end!='\n') return 0; //合理的格式最后面应该是一个换行符
  fprintf(code,"%d %c\n",RIGHT_BRACE,'}');
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
    if(!code_parse(fin,code)){
      return 0;
    }
    printf("@%d\n",i++);
  }
  if(jud==0) return 0;
  return 1;
}



