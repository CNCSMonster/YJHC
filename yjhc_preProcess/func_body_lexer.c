#include "func_body_lexer.h"


#define ERR printf("something wrong in parser")

//第二次遍历，合并运算符,并且合并两个类型
int token_mergeOpAndType(FILE* fin,FILE* code);

//第三次遍历补充流程控制中缺省的界符
int token_addlayer(FILE* fin,FILE* code);

//第四次遍历,猜测指针,函数,类型,使用前缓冲和超前搜索,TODO
int token_guess(FILE* fin,FILE* code);

//第五遍扫描,确定函数指针,给函数指针变量标记为函数
int token_findFuncPointer(FILE* fin,FILE* code);

//判断一个token类型是否是句子开始前的token
int is_preSentence_token(Token token);


//ps四次遍历过后不存在unknown类型的token,所有token都被确定为类型/界符/保留字/函数名/量名


//读取一个括号包裹的逻辑表达式，读取成功返回非0值,否则返回0
int readCase(FILE* fin,FILE* code);

//代码段词法分析,读到做花括号开始,直到遇到对应右花括号结束
int code_parse(FILE* fin,FILE* code);


//对于经过func_split分离出的函数体信息进行词法分析,得到词法单元序列
//基于猜测进行词法分析,可以把id根据位置猜测成类型或者变量等内容
//使用多次遍历完成这个过程
//编译gcc func_body_parsseer.c -o fbp
//使用fbp bodyPath tokensPath
int main(int argc,char* argv[]){
  char* bodyPath=argv[1];
  char* tokensPath=argv[2];
  //下面是调试用的路径
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
  //首先进行第一次遍历,该遍历需要循环进行,每次处理一个函数
  while(jud=code_parse(body,tokens));
  fclose(body);
  fclose(tokens);
  //然后进行第二次遍历,合并运算符
  strcpy(prePath,curPath);
  jud=sprintf(curPath,"%s2nd",tokensPath);
  if(jud<0) exit(-1);
  body=fopen(prePath,"r");
  if(body==NULL) exit(-1);
  tokens=fopen(curPath,"w");
  if(tokens==NULL){
    fclose(body);exit(-1);
  }
  if(!token_mergeOpAndType(body,tokens)) ERR;
  fclose(body);fclose(tokens);

  //进行第三次遍历,补充流程控制中缺省的界符
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

  //进行第四次遍历,对未知token进行猜测
  // strcpy(prePath,curPath);
  // jud=sprintf(curPath,"%s4rd",tokensPath);
  if(jud<0) exit(-1);
  body=fopen(curPath,"r");
  if(body==NULL) exit(-1);
  tokens=fopen(tokensPath,"w");
  if(tokens==NULL){
    fclose(body);exit(-1);
  }
  if(!token_guess(body,tokens)) ERR;
  fclose(body);fclose(tokens);

  //进行第五次遍历,也是最后一次遍历
  // body=fopen(curPath,"r");
  // if(body==NULL) exit(-1);
  // tokens=fopen(tokensPath,"w");
  // if(tokens==NULL){
  //   fclose(body);exit(-1);
  // }
  // if(!token_findFuncPointer(body,tokens)) ERR;
  // fclose(body);fclose(tokens);


  // system("dir");
  // printf("%s,%s",bodyPath,tokensPath);
  //查看中间文件
  // 最后删除中间文件,
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
  // system(ord);
  // sprintf(ord,"del %s4rd",tokensPath);
  // mystrReplace(ord,'/','\\');
  // // printf("\n%s",ord);
  // system(ord);
  return 0;
}

//判断一个token类型是否是句子开始前的token
int is_preSentence_token(Token token){
  if(token.kind==SEMICOLON||token.kind==LEFT_BRACE||token.kind==RIGHT_BRACE||token.kind==COMMA||token.kind==LEFT_PAR){
    return 1;
  }
  return 0;
}


int token_mergeOpAndType(FILE* fin,FILE* code){
  Token last;   //上一个token,最多只要保留两个token(因为最多两个token来合成)
  Token cur;
  last=getToken(fin);
  if(last.val==NULL) return 0;
  cur=getToken(fin);
  while(cur.val!=NULL){
    //TODO 比较cur的字符字面值和各种字符串,返回比较结果
    
    //如果当前是类型而且上一个字符也是类型,两个都是long,则可以合并成long long
    if(cur.kind==TYPE&&last.kind==TYPE&&strcmp(cur.val,"long")==0&&strcmp(last.val,"long")==0){
      Token new=connectToken(last,cur,TYPE," ");
      delToken(last);
      delToken(cur);
      last=new;
      cur=getToken(fin);
      continue;
    }
    else if(last.kind==TYPE&&cur.kind==OP&&strcmp(cur.val,"*")==0){

    }
    //目前当且仅当为op或者关键字的时候需要考试是否是某个token的部分
    else if(cur.kind==OP&&last.kind==OP){
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
  else if(cur.kind!=LEFT_PAR){
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

//第四次遍历,猜测指针,函数,类型,使用前缓冲和超前搜索,去除所有未知token
int token_guess(FILE* fin,FILE* code){
  //一个未知名可能是变量名也可能是常量名,也可能是类型定义,或者变量名或者函数名
  //我们起码需要直到前面一个类型的值
  Token pre=getToken(fin);
  if(pre.val==NULL) return 0;
  Token cur=(Token){
    // .kind=UNKNOWN,
    .val=NULL
  };
  Token next;
  while((cur=getToken(fin)).val!=NULL){
    //如果当前类型是TYPE类型,则会连接后面的所有可能的指针一起组成新的type类型
    if(cur.kind==TYPE){
      while((next=getToken(fin)).val!=NULL&&strcmp(next.val,"*")==0){
        Token tmpCur=connectToken(cur,next,TYPE,"");
        delToken(cur);
        delToken(next);
        cur=tmpCur;
      }
      //处理typedef可能的情况
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
    //否则如果当前类型不是unknown类型,则不猜测,退出当前循环
    if(cur.kind!=UNKNOWN){
      fputToken(pre,code);
      delToken(pre);
      pre=cur;
      continue;
    }
    //根据当前token内容猜测属性,如果上一个token是分号或者逗号,说明这是一句话的开始
    //或者是变量定义列表的部分,所以这里可能是定义语句
    //比如 int m,int a,b=1;
    next=getToken(fin); //超前搜索下一个token
    if(next.val==NULL){
      delToken(cur);
      delToken(pre);
      return 0;
    }
    //如果下一个token是左括号,则该next是函数名,这是唯一一种这个量是函数调用的函数名的情况
    if(next.kind==LEFT_PAR){
      cur.kind=FUNC;
    }
    //如果上一个词是typedef,则该类型是type，下一个类型是type
    else if(pre.kind==TYPEDEF_KEYWORD){
      cur.kind=TYPE;
      next.kind=TYPE;
    }
    //如果curtoken是某个语句的的开头或者const的下一个而且下一个token是未知名,则这个token是类型定义,下一个token是变量名
    else if((is_preSentence_token(pre)||pre.kind==CONST_KEYWORD)&&next.kind==UNKNOWN){
      cur.kind=TYPE;
      next.kind=VAR;
    }
    //如果cur token是开头token,而且下一个token是*,则这个token是个类型定义
    else if((is_preSentence_token(pre)||pre.kind==CONST_KEYWORD)
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
    //否则都猜测是变量
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
  // 写入pre的内容
  fputToken(pre,code);
  delToken(pre);
  return 1;
}

//失败的做法,试图在词法分析步骤去分析函数指针定义的成分,做了太多不该做的事情
//测试第五遍扫描
//第五遍扫描,确定函数指针,给函数指针变量标记为函数
//debug发现.第四次扫描后可能猜测函数指针定义种的返回位置类型token猜测为unknown
int token_findFuncPointer(FILE* fin,FILE* code){
  //首先找到每个Type开头的句子开头
  Token cur;
  Token pre={
    .val=NULL
  };
  Token tmpToken;
  int isRight=1;
  vector vec=getVector(sizeof(Token));
  pre=getToken(fin);
  if(pre.val==NULL){
    return 0;
  }
  while((cur=getToken(fin)).val!=NULL){
    //如果前面是句子开头,则进行判断,
    //可能是do结构
    if(cur.kind==DO){


    }
    //可能是while结构
    if(cur.kind==WHILE){

    }

    //可能是花括号结尾
    if(cur.kind==RIGHT_BRACE){

    }
    //可能是if结构
    if(cur.kind==IF){

    }
    //可能是elif结构
    if(cur.kind==ELIF){

    }

    //可能是else结构
    if(cur.kind==ELSE){

    }
    //可能是for结构
    if(cur.kind==FOR){

    }
    if(is_preSentence_token(pre)){
      //首先读取到分号
      while((tmpToken=getToken(fin)).val!=NULL&&tmpToken.kind!=SEMICOLON){
        vector_push_back(&vec,&tmpToken);
      }
      if(tmpToken.val==NULL){
        isRight=0;
        break;
      }
      Token* tokens=vector_toArr(&vec);
      int tokensSize=vec.size;
      int isFuncPointerDef=1;   //判断是否是定义函数指针的句子
      //然后判断这个句子是否是合适的句子
      if(!isFuncPointerDef||tokens[0].kind!=LEFT_PAR){
        isFuncPointerDef=0;
      }
      if(!isFuncPointerDef||tokens[1].kind!=OP){
        isFuncPointerDef=0;
      }
      if(!isFuncPointerDef||tokens[2].kind!=VAR){
        isFuncPointerDef=0;
      }
      if(!isFuncPointerDef||tokens[3].kind!=RIGHT_PAR){
        isFuncPointerDef=0;
      }
      if(!isFuncPointerDef||tokens[4].kind!=LEFT_PAR){
        isFuncPointerDef=0;
      }
      if(!isFuncPointerDef||tokens[tokensSize-1].kind!=RIGHT_PAR){
        isFuncPointerDef=0;
      }
      //如果有不合理的逗号,则说明格式不是函数指针定义语句
      if(!isFuncPointerDef||tokens[tokensSize-2].kind==COMMA){
        isFuncPointerDef=0;
      }
      //如果不是函数定义语句,则打印该句子并释放空间
      if(!isFuncPointerDef){
        fputToken(pre,code);
        fputToken(cur,code);
        delToken(pre);
        delToken(cur);
        for(int i=0;i<tokensSize;i++){
          fputToken(tokens[i],code);
          delToken(tokens[i]);
        }
        free(tokens);
        vector_clear(&vec);
        pre=tmpToken;
        continue;
      }
      //否则要么是函数指针定义语句,要么是其他符合要求的语句
      //使用循环的结构判断后面的参数列表是否符合要求
      for(int i=5;i<tokensSize-1;i+=2){
        Token t1;
        Token t2;
        vector_get(&vec,i,&t1);
        vector_get(&vec,i+1,&t2);
        //如果里面是
        if(t1.kind!=TYPE||(t2.kind!=COMMA&&i!=tokensSize-2)){
          isFuncPointerDef=0;
          break;
        }
      }
      if(!isFuncPointerDef){
        isRight=0;
        free(tokens);
        break;
      }
      cur.kind=TYPE;
      tokens[2].kind=FUNC;
      //打印以及释放空间
      fputToken(pre,code);delToken(pre);pre.val=NULL;
      fputToken(cur,code);delToken(cur);cur.val=NULL;
      //打印剩下的token并删除
      for(int i=0;i<tokensSize;i++){
        fputToken(tokens[i],code);
        delToken(tokens[i]);
      }
      free(tokens);
      vector_clear(&vec);
      pre=tmpToken;
    }
    else{
      fputToken(pre,code);
      delToken(pre);
      pre=cur;
    }
  }
  if(!isRight){
    //删除vector
    Token* tokens=vector_toArr(&vec);
    for(int i=0;i<vec.size;i++) delToken(tokens[i]);
    free(tokens);
    if(cur.val!=NULL) delToken(cur);
    if(pre.val!=NULL) delToken(pre);
    return 0;
  }
  if(pre.val!=NULL){
    fputToken(pre,code);delToken(pre);
  }
  if(cur.val!=NULL){
    fputToken(cur,code);delToken(cur);
  }
  return 1;
}




//代码段词法分析,直到遇到右花括号结束,第一遍
int code_parse(FILE *fin, FILE *code)
{
  char end;
  //读取一个左花括号开始
  if((end=fgetc(fin))!='{') return 0;
  //先往文件中写入一个花括号并换行
  fprintf(code, "%d %c\n", LEFT_BRACE, '{');
  int leftPar = 1;

  //函数token分析
  char *stops = "+-*/^|&;,.><=[]() {}\"\n"; //读到换行之前都是属于这个函数的内容
  char tmp[1000];
  int tmpI; //暂存用的int
  end = myfgets(tmp, stops, fin);
  while (end != '}' || leftPar > 1)
  {
    printf("^%s$\n",tmp);
    //对读取到的内容进行词法分析
    //仅仅读到的内容不为空的时候才需要进行词法分析
    if (strcmp(tmp, "") != 0)
    {
      //判断是否是基础数据类型
      if (isBaseType(tmp))
      {
        //如果是基础数据类型
        fprintf(code, "%d %s\n", TYPE, tmp);
      }
      //判断是否是常量类型修饰符const
      else if(strcmp(tmp,"const")==0){
        fprintf(code,"%d %s\n",CONST_KEYWORD,tmp);
      }
      //判断是否是类型重命名关键字typedef
      else if(strcmp(tmp,"typedef")==0){
        fprintf(code,"%d %s\n",TYPEDEF_KEYWORD,tmp);
      }
      //判断是否是self关键字(不允许用来命名自定义函数/成员方法/成员属性/其他量)
      else if(strcmp(tmp,SELF_STRING_VALUE)==0){
        fprintf(code,"%d %s\n",SELF_KEYWORD,tmp);
      }
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

