#include "grammar_tbl_generator.h"

//该程序目的,实现一个能够用来生成yjhc用的语法文件的编辑器

//初始化程序数据,
void init(){
  block.actionKinds=hashset_cre(sizeof(int));
  block.tbls_head.next=NULL;
  block.tokens=hashset_cre(sizeof(int));
  block.symbols=hashset_cre(sizeof(int));
  block.not_define=NULL;
  block.strIds=getStrIdTable();
  block.idAllocator=getIdAllocator();
  ord=NULL;
  fin=fopen(DEFAULT_GTG_WORK_FILE,"r");
  if(fin!=NULL){
    while((ord=fgetOrd(fin))!=NULL){
      maintainOrd(ord);
    }
    fclose(fin);
  }
  fin=stdin;
  fout=NULL;
}


int delSyntaxs(struct syntax_line* syntaxHead){
  int out=0;
  //注意,删除语法行的时候,要清楚对应的idAllocator里面的引用次数统计
  struct syntax_line* tmp=syntaxHead->next;
  //
  while(tmp!=NULL){
    out++;
    syntaxHead->next=tmp->next;
    int symbol=tmp->symbol;
    int token=tmp->token;
    int action=tmp->action;
    dropIdUseTimes(&block.idAllocator,symbol,1);
    dropIdUseTimes(&block.idAllocator,token,1);
    dropIdUseTimes(&block.idAllocator,action,1);
    free(tmp);
    tmp=syntaxHead->next;
  }
  return out;
}


int delTblBlocks(struct TblBlock* tbls_head){
  while (tbls_head->next!=NULL)
  {
    struct TblBlock* tmp=tbls_head->next;
    tbls_head->next=tmp->next;
    int actionkind=tmp->actionKind;
    free_hashset(&tmp->actions);
    int dropTimes=delSyntaxs(&tmp->syntaxs_head);
    free(tmp);
    if(dropTimes<0) return 0;
  }
  return 1;
}


//命令行读取器,读取一行命令,而且忽略前导空格,忽略注释符后面,而且读到换行停止,命令没有长度限制,动态分配空间
char* fgetOrd(FILE* fin){
  int baseSize=100;
  int size=baseSize;
  char* out=malloc(sizeof(char)*size);
  int i=0;
  char c;
  //忽略所有有效命令前的空格和注释
  while((c=fgetc(fin))!=EOF){
    if(c==' ') continue;
    if(c=='#'){
      while((c=fgetc(fin))!=EOF&&!(c=='\n'||c=='\r'));
      if(c==EOF) break;
    }else if(c=='\n'||c=='\r') return strcpy(malloc(1),"");
    else{
      break;
    }
  }

  if(c==EOF){
    free(out);
    return NULL;
  }else{
    out[i++]=c;
  }
  while((c=fgetc(fin))!=EOF&&c!='\n'){
    out[i++]=c;
    if(i>=size-2){
      size+=baseSize;
      char* tmp=malloc(sizeof(char)*size);
      strcpy(tmp,out);
      free(out);
      out=tmp;
    }
  }
  if(c==EOF&&strlen(out)==0){
    free(out);
    return NULL;
  }
  out[i]='\0';
  size=strlen(out);
  char* tmp=strcpy(malloc(sizeof(char)*(size+1)),out);
  free(out);
  return tmp;
}



int showStringsByIds(int* ids,int num){
  int i=0;
  while(i<num){
    char* str=NULL;
    str=getIdString(&block.idAllocator,ids[i++]);
    if(str==NULL) return 0;
    fprintf(fout,"%s",str);
    free(str);
    for(int j=1;j<n_each_line_gtg&&i<num;j++){
      str=getIdString(&block.idAllocator,ids[i++]);
      if(str==NULL) return 0;
      fprintf(fout," %s",str);
      free(str);
    }
    fprintf(fout,"\n");
  }
  return 1;
}


//处理命令,如果是不合理的命令则返回0,否则处理成功后返回非0值
int maintainOrd(){
  if(ord==NULL) return 0;
  OrdKind kind=ordKind(ord);
  //在执行前进行-o和-n信息的提取
  //TODO,提取-o属性和-n属性
  extractN(ord,&n_gtg);
  char tmp[1000];
  if(extractO(ord,tmp)){
    fout=fopen(tmp,"w");
    if(fout==NULL) fout=stdout;
  }else{
    fout=stdout;
  }
  int jud=executeOrds[kind]();
  if(!jud){
    printf("fail to execute such line\n");
  }
  if(fout!=stdout) fclose(fout);
  if(ord!=NULL)  free(ord);
  ord=NULL;
  n_gtg=0;    //清除之前读出的数字大小
  return 1;
}

int isUsedStr(char* str){
  //首先判断是否和默认字符串重合
  if(block.not_define==NULL&&strcmp(str,DEFAULT_NOTDEFINE_STRING)==0) return 1;
  else if(block.not_define!=NULL&&strcmp(str,block.not_define)==0) return 1;
  //首先查找字符串id表中查找该字符串的编号,如果查找到了说明已经使用过了
  int jud=strToId(block.strIds,str);
  if(jud<0) return 0;
  return -1;
}

//读取一个单词,忽略前导空格和单行注释以及
char* fgetWord(FILE* fin){
  int i=0;
  char* stops="\n #\r";
  char tmp[1000];
  char end;
  do{
    end=myfgets(tmp,stops,fin);
    if(end=='#'){
      char tmp2[1000];
      end=myfgets(tmp2,"\n",fin);
    }
    if(strlen(tmp)==0) continue;
    return strcpy(malloc((strlen(tmp)+1)*sizeof(char)),tmp);
  }while(end!=EOF);
  return strcpy(malloc(1),"");
}

//判断命令类型
OrdKind ordKind(char* ord){
  char* stops=" \n\r";
  //判断是否是控制型命令
  char end;
  char tmp[100];
  end=mysgets(tmp,stops,ord);
  if(strcmp(tmp,"exit")==0){
    return EXIT;
  }else if(strcmp(tmp,"init")==0){
    return INIT;
  }else if(strcmp(tmp,"gc")==0){
    return GC;
  }
  else if(strcmp(tmp,"cls")==0){
    return CLS;
  }
  else if(strcmp(tmp,"del")==0){
    return DEL;
  }
  else if(strcmp(tmp,"run")==0){
    return RUN;
  }
  //判断是否是提示命令
  else if(strcmp(tmp,"help")==0){
    return HELP;
  }
  //判断是否是替换命令/删除命令
  else if(strcmp(tmp,"replace")==0){
    return REPLACE;
  }
  //判断是否是增加命令
  else if(strcmp(tmp,"action")==0){
    end=mysgets(tmp,stops,ord+strlen(tmp)+1);
    if(strcmp(tmp,"add")==0) return ACTION_ADD;
  }
  else if(strcmp(tmp,"actionkind")==0){
    end=mysgets(tmp,stops,ord+strlen(tmp)+1);
    if(strcmp(tmp,"add")==0) return ACTIONKIND_ADD;
  }
  else if(strcmp(tmp,"symbol")==0){
    end=mysgets(tmp,stops,ord+strlen(tmp)+1);
    if(strcmp(tmp,"add")==0) return SYMBOL_ADD;
  }
  else if(strcmp(tmp,"syntax")==0){
    end=mysgets(tmp,stops,ord+strlen(tmp)+1);
    if(strcmp(tmp,"add")==0) return SYNTAX_ADD;
  }
  else if(strcmp(tmp,"token")==0){
    end=mysgets(tmp,stops,ord+strlen(tmp)+1);
    if(strcmp(tmp,"add")==0) return TOKEN_ADD;
  }
  else if(strcmp(tmp,"set")==0){
    end=mysgets(tmp,stops,ord+strlen(tmp)+1);
    if(strcmp(tmp,"defaultAction")==0) return SET_DEFAULT_ACTION;
    else if(strcmp(tmp,"notDefine")==0) return SET_NOT_DEFINE;
  }
  //判断是否是查看命令
  else if(strcmp(tmp,"check")==0){
    end=mysgets(tmp,stops,ord+strlen(tmp)+1);
    if(strcmp(tmp,"actions")==0){
      return CHECK_ACTION_ALL;
    }
    if(strcmp(tmp,"notDefine")==0) return CHECK_NOTDEFINE;
    if(strcmp(tmp,"defaultAction")==0){
      return CHECK_DEFAULT_ACTION;
    }
    else if(strcmp(tmp,"action")==0){
      return CHECK_ACTION_OFKIND;
    }
    else if(strcmp(tmp,"actionkind")==0){
      return CHECK_ACTIONKIND;
    }
    else if(strcmp(tmp,"token")==0){
      return CHECK_TOKEN;
    }
    else if(strcmp(tmp,"syntax")==0){
      return CHECK_SYNTAX_OFKIND;
    }
    else if(strcmp(tmp,"syntaxs")==0){
      return CHECK_SYNTAX_ALL;
    }
    else if(strcmp(tmp,"symbol")==0){
      return CHECK_SYMBOL;
    }
    else if(strcmp(tmp,"all")==0){
      return CHECK_ALL;
    }
  }
  //判断是否是输出命令
  else if(strcmp(tmp,"output")==0){
    end=mysgets(tmp,stops,ord+strlen(tmp)+1);
    if(strcmp(tmp,"-o")==0) return OUTPUT_ORDERS;
    else if(strcmp(tmp,"-g")==0) return OUTPUT_GRAMMAR;
  }

  //否则是未定义命令
  return NOT_DEFINE_ORD;
}



/*
各种处理命令的方式
一个处理命令方式函数表
*/

int error() //错误提示,当输入没有定义的命令的时候给出错误提示
{
  printf("your input is not defined in gtg grammar!\n");
  printf("you could enter \"help\" to watch help messages\n");
  return 1;
}

int gc(){
  //垃圾回收,对于重复的语句进行回收

  //需要维护一个表,记录已经访问过的信息
  //每个symbol->tokens表格
  HSet hset=hashset_cre(sizeof(long long));
  //从每个表的开始访问
  struct TblBlock* tbl=block.tbls_head.next;
  while(tbl!=NULL){
    //为每个表初始一次空间
    free_hashset(&hset);
    //要删除掉的语句有,symbol或者token已经失效的语句,或者(symbol,token)已经在前面出现过的语句
    struct syntax_line* next=tbl->syntaxs_head.next;
    struct syntax_line* pre=&(tbl->syntaxs_head);
    while(next!=NULL){
      //首先判断该句是否是合理的
      int symbol=next->symbol;
      int token=next->token;
      int action=next->action;
      if(!hashset_contains(&block.symbols,&symbol)||!hashset_contains(&block.tokens,&token))
      {
        //减少token，symbol，action的引用次数并且退出
        dropIdUseTimes(&block.idAllocator,symbol,1);
        dropIdUseTimes(&block.idAllocator,token,1);
        dropIdUseTimes(&block.idAllocator,action,1);
        pre->next=next->next;
        free(next);
        next=pre->next;
        tbl->syntax_num--;
        continue;
      }
      //否则判断是否已经出现过
      long long code=((long long)symbol)*INT_MAX+(long long)token;
      if(hashset_contains(&hset,&code)){
        pre->next=next->next;
        free(next);
        next=pre->next;
        tbl->syntax_num--;
        continue;
      }
      hashset_add(&hset,&code);
      pre=next;
      next=next->next;
    }
    tbl=tbl->next;
  }
  
  //对于用完的hset表要回收空间
  free_hashset(&hset);
  return 1;
}

int cls(){
  system("cls");
  return 1;
}

int set_defaultAction(){
  //首先读取actionkind
  char* track=ord;
  char tmp[1000];
  char end;
  //读取actionkind属性
  end=mysgets(tmp," ",track);
  if(end==' ') track+=strlen(tmp)+1;
  else return error();
  end=mysgets(tmp," ",track);
  if(end==' ') track+=strlen(tmp)+1;
  else return error();
  //开始读取actionkind作为输入
  char actionkind[200];
  end=mysgets(actionkind," ",track);
  struct TblBlock* targetTbl=getTbl(actionkind);
  if(targetTbl==NULL) return 0;
  char* action=fgetWord(fin);
  //如果是要恢复到默认设置
  if(strcmp(action,DEFAULT_NOTDEFINE_STRING)==0){
    free(action);
    targetTbl->defaultAction=NOT_DEFINE_ID;
    return 1;
  }
  int id=strToId(block.strIds,action);
  free(action);
  if(id<0||!hashset_contains(&targetTbl->actions,&id)) return 0;
  targetTbl->defaultAction=id;
  return 1;
}

int set_not_define(){
  char* newNotDefineString=fgetWord(fin);
  //如果这个新要设定得字符串是已经在系统中得字符串,则设定失败
  if(isUsedStr(newNotDefineString)){
    free(newNotDefineString);
    return 0;
  }
  if(block.not_define!=NULL){
    free(block.not_define);
    block.not_define=NULL;
  }
  if(strcmp(newNotDefineString,DEFAULT_NOTDEFINE_STRING)==0) free(newNotDefineString);
  else if(block.not_define==NULL) block.not_define=newNotDefineString;
  return 1;
}

int action_add(){
  //TODO fix
  char* track=ord;
  char tmp[1000];
  char end;
  int id;
  //读取actionkind属性
  end=mysgets(tmp," ",track);
  if(end==' ') track+=strlen(tmp)+1;
  else return error();
  end=mysgets(tmp," ",track);
  if(end==' ') track+=strlen(tmp)+1;
  else return error();
  //开始读取actionkind作为输入
  char actionkind[500];
  end=mysgets(actionkind," ",track);
  struct TblBlock* tbl=getTbl(actionkind);
  if(tbl==NULL) return 0;
  //如果没有附加-n属性,则读取下一行所有内容作为输入
  if(n_gtg==0){
    char* tord=fgetOrd(fin);
    //然后把ord中所有内容加入到该actionkind的action中
    track=tord;
    int islast=1;
    while ((end = mysgets(tmp, " ", track)) != '\0'||islast)
    {
      track+=strlen(tmp)+1;
      if(end=='\0') islast=0;
      if (strlen(tmp) == 0)
        continue;
      //再判断这个名字有没有用过
      if (isUsedStr(tmp))
      {
        fprintf(fout,"%s has been input!\n",tmp);
        continue;
      }
      id = allocateId(&block.idAllocator, tmp);
      if(id<0) return 0;
      putStrId(block.strIds, tmp, id);
      //往该actions块中加入内容
      hashset_add(&tbl->actions, &id);
    }
    free(tord);
  }
  //否则读取接下来n个有效字符串作为该action的输入
  else{
    for(int i=0;i<n_gtg;i++){
      char* tmp=fgetWord(fin);
      if(strlen(tmp)==0){
        free(tmp);
        continue;
      }
      //再判断这个名字有没有用过
      if(isUsedStr(tmp)){
        fprintf(fout,"%s has been input!\n",tmp);
        fprintf(fout,"you can't add %s as action for actionkind\"%s\"\n",tmp,actionkind);
        free(tmp);
        continue;
      }
      id=allocateId(&block.idAllocator,tmp);
      if(id<0) return 0;
      putStrId(block.strIds,tmp,id);
      hashset_add(&tbl->actions,&id);
      free(tmp);
    }
  }
  return 1;
}

int syntax_add(){
  char* track=ord;
  char tmp[1000];
  char end;
  int ifGet=1;
  //读取actionkind属性
  end=mysgets(tmp," ",track);
  if(end==' ') track+=strlen(tmp)+1;
  else ifGet=0;
  end=mysgets(tmp," ",track);
  if(end==' ') track+=strlen(tmp)+1;
  else ifGet=0;
  struct TblBlock* kindof=NULL;
  int id;
  char actionkind[200];
  if(ifGet){
    //开始读取actionkind作为输入
    end=mysgets(actionkind," ",track);
    id=strToId(block.strIds,actionkind);
    if(id>=0){
      kindof=block.tbls_head.next;
      while (kindof!=NULL&&kindof->actionKind!=id) kindof=kindof->next;
    }
  }
  //如果没有-n属性,读取该行的n个
  if(n_gtg==0){
    char* line=fgetOrd(fin);
    char* tline=line;
    int ifHasEnd=0;
    end=' ';
    while((end!='\0')&&((end=mysgets(tmp," ",tline))!='\0'||!ifHasEnd)){
      if(end=='\0') ifHasEnd=1; 
      else tline+=strlen(tmp)+1;
      char tmp2[200];
      char tend=mysgets(tmp2,":",tmp);
      if(tend!=':'){
        int jud;
        if(kindof==NULL){
          fprintf(fout,"miss <actionkind> for add syntax line \"%s\"\n",tmp);
          jud=0;
        }
        else jud=syntax_line_add(tmp,kindof);
        if(!jud) fprintf(fout,"fail to add syntax line \"%s\"\n",tmp);
        continue;
      }
      int tid=strToId(block.strIds,tmp2);
      if(tid<0){
        fprintf(fout,"fail to add syntax line \"%s\"\n",tmp);
        continue;
      }
      struct TblBlock* tmpTbl=block.tbls_head.next;
      while(tmpTbl!=NULL&&tmpTbl->actionKind!=tid) tmpTbl=tmpTbl->next;
      if(tmpTbl==NULL){
        fprintf(fout,"table missed for actionkind %s\n",actionkind);
        continue;
      }
      if(!syntax_line_add(tmp+strlen(tmp2)+1,tmpTbl)){
        fprintf(fout,"fail to add syntax line \"%s\"\n",tmp);
      }
    }
    free(line);
  }
  //如果有-n属性,则读取接下来n个信息作为输入
  else
  {
    for (int i = 0; i < n_gtg; i++)
    {
      char *toAdd = fgetWord(fin);
      char tmp2[200];
      end = mysgets(tmp2, ":", toAdd);
      if (end != ':')
      {
        int jud;
        if (kindof == NULL)
        {
          fprintf(fout, "miss <actionkind> for add syntax line \"%s\"\n", toAdd);
          jud = 0;
        }
        else
          jud = syntax_line_add(toAdd, kindof);
        if (!jud)
          fprintf(fout, "fail to add syntax line \"%s\"\n", toAdd);
        free(toAdd);
        continue;
      }
      int tid = strToId(block.strIds, tmp2);
      if (tid < 0)
      {
        fprintf(fout, "fail to add syntax line \"%s\"\n", toAdd);
        free(toAdd);
        continue;
      }
      struct TblBlock *tmpTbl = block.tbls_head.next;
      while (tmpTbl != NULL && tmpTbl->actionKind != tid)
        tmpTbl = tmpTbl->next;
      if (tmpTbl == NULL)
      {
        fprintf(fout, "table missed for actionkind %s\n", actionkind);
        free(toAdd);
        continue;
      }
      if (!syntax_line_add(toAdd + strlen(tmp2) + 1, tmpTbl))
      {
        fprintf(fout, "fail to add syntax line %s\n", toAdd);
      }
      free(toAdd);
    }
  }
  return 1;
}

int extractN(char* line,int* returnN){
  char tmp[1000];
  char end;
  //读取-n属性
  while((end=mysgets(tmp,"-",line))!='\0'){
    line+=strlen(tmp)+1;
    end=mysgets(tmp," ",line);
    if(end!=' '){
      return 0;
    }else if(strcmp("n",tmp)==0){
      line+=strlen(tmp)+1;
      end=mysgets(tmp," ",line);
      int n=atoi(tmp);
      if(n<=0){
        printf("input error,please input possitive number after -n\n");
        return 0;
      }else{
        if(returnN!=NULL) *returnN=n;
        return n;
      }
    }else{
      return 0;
    }
  }
  return 0;
}


int extractO(char* line,char* returnPath){
  char end;
  //读取-o属性
  while((end=mysgets(returnPath,"-",line))!='\0'){
    line+=strlen(returnPath)+1;
    end=mysgets(returnPath," ",line);
    if(end!=' '){
      return 0;
    }else if(strcmp("o",returnPath)==0||strcmp("g",returnPath)==0){
      line+=strlen(returnPath)+1;
      end=mysgets(returnPath," ",line);
      return 1;
    }else{
      return 0;
    }
  }
  return 0;
}


int symbol_add(){
  //
  HSetp symbols=&block.symbols;
  //如果没有附加-n属性,使用接下来一行作为输入
  if(n_gtg==0){
    char* line=fgetOrd(fin);
    char* tline=line;
    char tmp[200];
    char end;
    do{
      end=mysgets(tmp," \n\r#",tline);
      tline+=strlen(tmp)+1;
      if(isUsedStr(tmp)){
        fprintf(fout,"\nerror!this word %s has been added!\n",tmp);
        continue;
      }
      else if(strlen(tmp)==0){
        fprintf(fout,"\nerror!you can't use empty string as symbol!\n",tmp);
        continue;
      }
      //首先,给该字符串分配id
      int id=allocateId(&block.idAllocator,tmp);
      //然后把id加入到字符串id表中,同时把id加入到symbols表中
      if(id>=0){
        putStrId(block.strIds,tmp,id);
        hashset_add(&block.symbols,&id);
      }
      else fprintf(fout,"\nerror!fail to allocate id for %s\n",tmp);
      
    }while(end!='\0'&&end!='#');
    free(line);
  }
  //否则使用接下来n个单词作为输入
  else{
    for(int i=0;i<n_gtg;i++){
      char* tmp=fgetWord(fin);
      //判断是否是已经加入的字符串
      if(isUsedStr(tmp)){
        fprintf(fout,"\nerror!this word %s has been added!\n",tmp);
        free(tmp);
        continue;
      }
      else if(strlen(tmp)==0){
        fprintf(fout,"\nerror!you can't use empty string as symbol!\n");
        free(tmp);
        continue;
      }
      //首先,给该字符串分配id
      int id=allocateId(&block.idAllocator,tmp);
      //然后把id加入到字符串id表中,同时把id加入到symbols表中
      if(id>=0){
        putStrId(block.strIds,tmp,id);
        hashset_add(&block.symbols,&id);
      }
      else fprintf(fout,"\nerror!fail to allocate id for %s\n",tmp);
      free(tmp);
    }
  }
  return 1;
}
int token_add(){
  HSetp tokens=&block.tokens;
  //如果没有附加-n属性,使用接下来一行作为输入
  if(n_gtg==0){
    char* line=fgetOrd(fin);
    char* tline=line;
    char tmp[200];
    char end;
    do{
      end=mysgets(tmp," \n\r#",tline);
      tline+=strlen(tmp)+1;
      if(isUsedStr(tmp)){
        fprintf(fout,"\nerror!this word %s has been added!\n",tmp);
        continue;
      }
      else if(strlen(tmp)==0){
        fprintf(fout,"\nerror!you can't use empty string as token!\n");
        continue;
      }
      //首先,给该字符串分配id
      int id=allocateId(&block.idAllocator,tmp);
      //然后把id加入到字符串id表中,同时把id加入到symbols表中
      if(id>=0){
        putStrId(block.strIds,tmp,id);
        hashset_add(&block.tokens,&id);
      }
      else fprintf(fout,"\nerror!fail to allocate id for %s\n",tmp);
      
    }while(end!='\0'&&end!='#');
    free(line);
  }
  //否则使用接下来n个单词作为输入
  else{
    for(int i=0;i<n_gtg;i++){
      char* tmp=fgetWord(fin);
      //判断是否是已经加入的字符串
      if(isUsedStr(tmp)){
        fprintf(fout,"\nerror!this word %s has been added!\n",tmp);
        free(tmp);
        continue;
      }
      else if(strlen(tmp)==0){
        fprintf(fout,"\nerror!you can't use empty string as symbol!\n");
        free(tmp);
        continue;
      }
      //首先,给该字符串分配id
      int id=allocateId(&block.idAllocator,tmp);
      //然后把id加入到字符串id表中,同时把id加入到symbols表中
      if(id>=0){
        putStrId(block.strIds,tmp,id);
        hashset_add(&block.tokens,&id);
      }
      else fprintf(fout,"\nerror!fail to allocate id for %s\n",tmp);
      free(tmp);
    }
  }
  return 1;
}

int actionkind_add(){
  HSetp actionkinds=&block.actionKinds;
  //如果没有附加-n属性,使用接下来一行作为输入
  if(n_gtg==0){
    char* line=fgetOrd(fin);
    char* tline=line;
    char tmp[200];
    char end;
    do{
      end=mysgets(tmp," \n\r#",tline);
      tline+=strlen(tmp)+1;
      if(isUsedStr(tmp)){
        fprintf(fout,"\nerror!this word %s has been added!\n",tmp);
        continue;
      }
      else if(strlen(tmp)==0){
        fprintf(fout,"\nerror!you can't use empty string as symbol!\n");
        continue;
      }
      //首先,给该字符串分配id
      int id=allocateId(&block.idAllocator,tmp);
      //然后把id加入到字符串id表中,同时把id加入到symbols表中
      if(id>=0){
        putStrId(block.strIds,tmp,id);
        hashset_add(&block.actionKinds,&id);
        //加入新的actionkind块
        struct TblBlock* addTbl=malloc(sizeof(struct TblBlock));
        addTbl->next=block.tbls_head.next;
        block.tbls_head.next=addTbl;
        addTbl->actionKind=id;
        addTbl->syntax_num=0;
        addTbl->actions=hashset_cre(sizeof(int));
        addTbl->defaultAction=NOT_DEFINE_ID; //负数表示使用默认的id
        addTbl->syntaxs_head.next=NULL;
      }
      else fprintf(fout,"\nerror!fail to allocate id for %s\n",tmp);
      
    }while(end!='\0'&&end!='#');
    free(line);
  }
  //否则使用接下来n个单词作为输入
  else{
    for(int i=0;i<n_gtg;i++){
      char* tmp=fgetWord(fin);
      //判断是否是已经加入的字符串
      if(isUsedStr(tmp)){
        fprintf(fout,"\nerror!this word %s has been added!\n",tmp);
        free(tmp);
        continue;
      }
      else if(strlen(tmp)==0){
        fprintf(fout,"\nerror!you can't use empty string as symbol!\n");
        free(tmp);
        continue;
      }
      //首先,给该字符串分配id
      int id=allocateId(&block.idAllocator,tmp);
      //然后把id加入到字符串id表中,同时把id加入到symbols表中
      if(id>=0){
        putStrId(block.strIds,tmp,id);
        hashset_add(&block.actionKinds,&id);
        //加入新的actionkind块
        struct TblBlock* addTbl=malloc(sizeof(struct TblBlock));
        addTbl->next=block.tbls_head.next;
        block.tbls_head.next=addTbl;
        addTbl->actionKind=id;
        addTbl->actions=hashset_cre(sizeof(int));
        addTbl->syntax_num=0;
        addTbl->defaultAction=-1; //负数表示使用默认的id
        addTbl->syntaxs_head.next=NULL;
      }
      else fprintf(fout,"\nerror!fail to allocate id for %s\n",tmp);
      free(tmp);
    }
  }
  return 1;
}

int help(){
  FILE* fh=fopen(HELP_FILE,"r");
  //然后展示在控制台上
  char c;
  while((c=fgetc(fh))!=EOF) fputc(c,stdout);
  fclose(fh);
  fputc('\n',stdout);
  return 1;
}

//in this way you will del a string.
int del(){
  //如果没有-n属性指定,则删除下一行输入的所有字符串
  if(n_gtg==0){
    char* line=fgetOrd(fin);
    char* tline=line;
    char tmp[200];
    char end;
    //TODO
    do{
      end=mysgets(tmp," \n\r#",tline);
      tline+=strlen(tmp)+1;
      if(!gtg_delString(tmp)) fprintf(fout,"fail to del string \"%s\"\n",tmp);
    }while(end!='\0'&&end!='#');
    free(line);
  }
  else{
    //，完成多行时del处理
    for(int i=0;i<n_gtg;i++){
      char* tmp=fgetWord(fin);
      //判断是否是已经加入的字符串
      if(!isUsedStr(tmp)){
        fprintf(fout,"\nerror!you can't del string %s,which has not been added!\n",tmp);
        free(tmp);
        continue;
      }
      if(!gtg_delString(tmp)) fprintf(fout,"fail to del string \"%s\"\n",tmp);
    }
  }
  return 1;
}

int run(){
  //获取输入的文件名
  char tmp[1000];
  char end=mysgets(tmp," ",ord);
  if(end!=' ') return 0;
  end=mysgets(tmp," ",ord+strlen(tmp)+1);
  FILE* tfin=fin;
  fin=fopen(tmp,"r");
  if(fin==NULL){
    fin=tfin;
    return 0;
  }
  if(ord!=NULL) free(ord);
  ord=NULL;
  while ((ord=fgetOrd(fin))!=NULL)
  {
    maintainOrd();
    ord=NULL;
  }
  fclose(fin);
  fin=tfin;
  return 1;
}


int replace(){
  if(n_gtg==0){
    char* line=fgetOrd(fin);
    char* tline=line;
    char tmp[200];
    char end;
    //TODO
    do{
      end=mysgets(tmp," \n\r#",tline);
      tline+=strlen(tmp)+1;
      if(!gtg_replaceString(tmp)) fprintf(fout,"fail to execute replace \"%s\"\n",tmp);
    }while(end!='\0'&&end!='#');
    free(line);
  }
  else{
    //，完成多行时del处理
    for(int i=0;i<n_gtg;i++){
      char* tmp=fgetWord(fin);
      if(!gtg_replaceString(tmp)) fprintf(fout,"fail to execute replace \"%s\"\n",tmp);
    }
  }
  return 1;
}

int gtg_exit(){
  //把所有内容先写入文件
  fout=fopen(DEFAULT_GTG_WORK_FILE,"w");

  if(fout==NULL) return 0;
  if(ord!=NULL) free(ord);
  ord=NULL; 
  output_orders();  
  fclose(fout);
  //释放空间
  //清空symbol表并释放空间
  free_hashset(&block.symbols);
  //清除token表释放
  free_hashset(&block.tokens);
  //清除actionkind表并释放空间
  free_hashset(&block.actionKinds);
  //清除语法块
  delTblBlocks(&block.tbls_head);
  //释放并更换id分配器
  freeIdAllocator(&block.idAllocator);
  //释放字符串-id哈希表
  delStrIdTable(block.strIds);
  free(block.strIds);
  //重置默认字符串
  if(block.not_define!=NULL){
    free(block.not_define);
    block.not_define=NULL;
  }
  exit(0);
}

int gtg_init(){

  //清空symbol表并释放空间
  free_hashset(&block.symbols);
  //清除token表释放
  free_hashset(&block.tokens);
  //清除actionkind表并释放空间
  free_hashset(&block.actionKinds);
  //清除语法块
  delTblBlocks(&block.tbls_head);
  //释放并更换id分配器
  freeIdAllocator(&block.idAllocator);
  block.idAllocator=getIdAllocator();
  //释放字符串-id哈希表
  delStrIdTable(block.strIds);
  //重置默认字符串
  if(block.not_define!=NULL){
    free(block.not_define);
    block.not_define=NULL;
  }
  return 1;
}




int check_actions_ofkind(){
  //TODO查找到对应的块,查看action
  //先读取actionkind
  char tmp[1000];
  char end=mysgets(tmp," ",ord);
  if(end=='\0') return 0;
  char* tord=ord+strlen(tmp)+1;
  end=mysgets(tmp," ",tord);
  if(end=='\0') return 0;
  tord+=strlen(tmp)+1;
  end=mysgets(tmp," ",tord);
  int id=strToId(block.strIds,tmp);
  //查找该字符串是否已经注册,查找注册id
  //如果返回id小于0,则说明该字符串没注册,更不可能对应actionkind
  if(id<0) return 0;
  if(!hashset_contains(&block.actionKinds,&id)) return 0;
  //找到目的表格
  struct TblBlock* target=block.tbls_head.next;
  while(target!=NULL&&target->actionKind!=id) target=target->next;
  if(target==NULL) return 0;
  //展示actions
  int* arr=hashset_toArr(&target->actions);
  showStringsByIds(arr,target->actions.num);
  free(arr);
  return 1;
}


int check_actions_all(){
  struct TblBlock* target=block.tbls_head.next;
  while(target!=NULL){
    char* actionkind=getIdString(&block.idAllocator,target->actionKind);
    fprintf(fout,"actionkind:%s\n",actionkind);
    if(target==NULL) return 0;
    //展示actions
    int* arr=hashset_toArr(&target->actions);
    showStringsByIds(arr,target->actions.num);
    free(arr);
    target=target->next;
  }
  return 1;
}


int check_symbol(){
  int* arr=hashset_toArr(&block.symbols);
  int jud=showStringsByIds(arr,block.symbols.num);
  return jud;
}

int check_token(){
  int* arr=hashset_toArr(&block.tokens);
  int jud=showStringsByIds(arr,block.tokens.num);
  return jud;
}

int check_notDefine(){
  if(block.not_define==NULL) fprintf(fout,"%s\n",DEFAULT_NOTDEFINE_STRING);
  else fprintf(fout,"%s\n",block.not_define);
  return 1;
}

int check_all(){
  printLongSplit();
  fprintf(fout,"NotDefineString:\n");
  if(!check_notDefine()) return 0;
  fprintf(fout,"All symbols:\n");
  if(!check_symbol()) return 0;
  fprintf(fout,"All tokens:\n");
  if(!check_token()) return 0;
  printMidSplit();
  fprintf(fout,"All actionkinds:\n");
  if(!check_actionkind()) return 0;
  printMidSplit();
  fprintf(fout,"All actions:\n");
  if(!check_actions_all()) return 0;
  printMidSplit();
  fprintf(fout,"All syntaxs:\n");
  if(!check_syntaxs_all()) return 0;
  return 1;
}



int check_actionkind(){
  int* arr=hashset_toArr(&block.actionKinds);
  int jud=showStringsByIds(arr,block.actionKinds.num);
  return jud;
}

int check_syntaxs_ofkind(){
  //首先读取actionkind
  char tmp[1000];
  char* tord=ord;
  char end=mysgets(tmp," ",tord);
  if(end==' ')  tord+=strlen(tmp)+1;
  else return 0;
  end=mysgets(tmp," ",tord);
  if(end==' ')  tord+=strlen(tmp)+1;
  else return 0;
  end=mysgets(tmp," ",tord);
  //然后根据actionkind找到对应的块
  struct TblBlock* target=getTbl(tmp);
  if(target==NULL) return 0;
  return gtg_showTbl(target);
}


int check_syntaxs_all(){
  //循环打印不同actionkind的action
  struct TblBlock* tmpTbl=block.tbls_head.next;
  int jud=1;
  while(tmpTbl!=NULL){
    printShortSplit();
    jud=jud&&gtg_showTbl(tmpTbl);
    tmpTbl=tmpTbl->next;
  }
  return jud;
}

//查看指定动作类型的默认动作
int check_default_action(){
  //先获取actionkind所在的块 
  char tmp[1000];
  char* tord=ord;
  char end=mysgets(tmp," ",ord);
  if(end==' ')  tord+=strlen(tmp)+1;
  else return 0;
  end=mysgets(tmp," ",ord);
  if(end==' ')  tord+=strlen(tmp)+1;
  else return 0;
  end=mysgets(tmp," ",tord);
  int id=strToId(block.strIds,tmp);
  if(id<0) return 0;
  if(!hashset_contains(&block.actionKinds,&id)) return 0;
  struct TblBlock* tbl=getTbl(tmp);
  if(tbl==NULL) return 0; 
  char* default_action=getIdString(&block.idAllocator,tbl->defaultAction);
  fprintf(fout,"%s\n",default_action);
  return 1;
}

int output_orders(){
  gc(); //先进行收缩
  //not_define字符串不是默认字符串时写入not_define
  if(block.not_define!=NULL){
    fprintf(fout,"set notDefine\n");
    check_notDefine();
  }
  //首先输出所有的symbol
  int num=block.symbols.num;
  if(num!=0){
    fprintf(fout,"symbol add -n %d\n",num);
    check_symbol();
  }
  //然后输出所有token
  num=block.tokens.num;
  if(num!=0){
    fprintf(fout,"token add -n %d\n",num);
    check_token();
  }
  //然后输出所有的actionkind
  num=block.actionKinds.num;
  if(num!=0){
    fprintf(fout,"actionkind add -n %d\n",num);
    check_actionkind();
  }
  //然后输出每个actionkind的action和syntax
  struct TblBlock* tmpTbl=block.tbls_head.next;
  while(tmpTbl!=NULL){
    //输出对应表格对应的default actionkind
    char* actionkind=getIdString(&block.idAllocator,tmpTbl->actionKind);
    if(actionkind==NULL) return 0;
    //首先写入action,如果有的话
    if(tmpTbl->actions.num!=0){
      int* arr=hashset_toArr(&tmpTbl->actions);
      num=tmpTbl->actions.num;
      fprintf(fout,"action add %s\n",actionkind);
      showStringsByIds(arr,num);
      free(arr);
    }
    //然后写入default action
    if(actionkind==NULL) return 0;
    fprintf(fout,"set defaultAction %s\n",actionkind);
    if(tmpTbl->defaultAction==NOT_DEFINE_ID) fprintf(fout,"%s\n",DEFAULT_NOTDEFINE_STRING);
    else{
      char* def_act=getIdString(&block.idAllocator,tmpTbl->defaultAction);
      if(def_act==NULL){
        free(actionkind);
        return 0;
      }
      fprintf(fout,"%s\n",def_act);
      free(def_act);
    }
    //最后写入syntaxs
    if(tmpTbl->syntax_num!=0){
      fprintf(fout,"syntax add %s -n %d\n",actionkind,tmpTbl->syntax_num);
      gtg_showSyntaxs(tmpTbl,actionkind);
    }
    free(actionkind);
    tmpTbl=tmpTbl->next;
  }
  return 1;
}


int output_grammar(){
  //首先进行放缩
  gc();
  //首先生成全局的not_define定义
  if(block.not_define==NULL) fprintf(fout,"#define %s -1\n\n",DEFAULT_NOTDEFINE_STRING);
  else fprintf(fout ,"#define %s -1\n\n",block.not_define);

  //获取symbol和token的数量
  int symbolsNum=block.symbols.num;
  int tokensNum=block.tokens.num;
  //然后生成某个ids数组到0-n的映射，TODO
  int* symbolIds=hashset_toArr(&block.symbols);
  int* symbols=get_hashArr_fromZero(symbolIds,symbolsNum,NULL);
  output_grammar_genEnum("Symbol",symbolIds,symbolsNum);
  int* tokenIds=hashset_toArr(&block.tokens);
  int* tokens=get_hashArr_fromZero(tokenIds,tokensNum,NULL);
  output_grammar_genEnum("Token",tokenIds,tokensNum);
  //先生成所有actionkind的enum
  struct TblBlock* tbl=block.tbls_head.next;
  while (tbl!=NULL)
  {
    char* actionkind=getIdString(&block.idAllocator,tbl->actionKind);
    int* ids=hashset_toArr(&tbl->actions);
    output_grammar_genEnum(actionkind,ids,tbl->actions.num);
    free(ids);
    fprintf(fout,"int %s_Tbl[Symbols_NUM][Tokens_NUM]={\n",actionkind);
    free(actionkind);
    //生成每一行的表格内容,当然首先要准备个表格
    int* table=malloc(sizeof(int)*symbolsNum*tokensNum);
    //然后全部内容初始化为-1
    memset(table,-1,symbolsNum*tokensNum*sizeof(int));
    //然后根据syntax填写内容
    struct syntax_line* sl=tbl->syntaxs_head.next;
    while(sl!=NULL){
      int x=symbols[sl->symbol];
      int y=tokens[sl->token];
      *(table+x*symbolsNum+y)=sl->action;
      sl=sl->next;
    }
    //然后开始打印
    for(int i=0;i<symbolsNum;i++){
      char* name=getIdString(&block.idAllocator,symbolIds[i]);
      int x=symbols[symbolIds[i]];
      //记录当前的symbol与token的关系
      fprintf(fout,"[%s] {",name);
      free(name);
      for(int j=0;j<tokensNum;j++){
        int y=tokens[tokenIds[j]];
        int actionId=*(table+x*symbolsNum+y);
        char* action=NULL;
        if(actionId==-1){
          if(tbl->defaultAction==-1&&block.not_define==NULL) action=strcpy(malloc(strlen(DEFAULT_NOTDEFINE_STRING)+1),DEFAULT_NOTDEFINE_STRING);
          else if(tbl->defaultAction==-1) action=strcpy(malloc(strlen(block.not_define)+1),block.not_define);
          else action=getIdString(&block.idAllocator,tbl->defaultAction);
        }else{
          action=getIdString(&block.idAllocator,actionId);
        }
        name=getIdString(&block.idAllocator,tokenIds[i]);    //记录这个时候的token的名字
        fprintf(fout,"[%s] %s",name,action);
        if(action!=NULL) free(action);
        if(j!=tokensNum-1) fprintf(fout,",");
      }
      if(i==symbolsNum-1) fprintf(fout,"}\n};\n\n");
      else fprintf(fout,"},\n");
    }
    free(table);
    tbl=tbl->next;
  }
  free(symbolIds);
  free(tokenIds);
  return 1;
}

int output_grammar_genEnum(char* enumName,int* arr,int arrSize){
  fprintf(fout,"typedef enum enum_%s{\n",enumName);
  for(int i=0;i<arrSize;i++){
    char* enumItem=getIdString(&block.idAllocator,arr[i]);
    fprintf(fout,"%s,\n",enumItem);
    free(enumItem);
  }
  fprintf(fout,"%s_NUM\n}%s;\n\n",enumName,enumName);
  return 1;
}

int* get_hashArr_fromZero(int* ids,int idsSize,int* returnSize){
  int max=-1;
  for(int i=0;i<idsSize;i++) if(ids[i]>max) max=ids[i];
  int* out=malloc(sizeof(int)*(max+1));
  int j=0;
  for(int i=0;i<idsSize;i++){
    out[ids[i]]=j++;
  }
  if(returnSize) *returnSize=max+1;
  return out;
}


/*
other
*/

int gtg_delString(char* tmp)
{
  if(!isUsedStr(tmp)) return 0;

  //首先判断是否是notDefine对应的字符串
  if(block.not_define!=NULL&&strcmp(block.not_define,tmp)==0){
    free(block.not_define);
    block.not_define=NULL;
    return 1;
  }
  int id=strToId(block.strIds,tmp);
  if(id<0) return 0;
  // 然后解除这个id与字符串的绑定
  delString(&block.idAllocator, id);
  // 并把这个id从字符串id表中取出
  delStr(block.strIds, tmp);
  // 然后判断是否是各种类型的内容,如果是,从集合中取出
  if (hashset_contains(&block.symbols, &id))
  {
    hashset_remove(&block.symbols, &id);
  }
  else if (hashset_contains(&block.tokens, &id))
  {
    hashset_remove(&block.tokens, &id);
  }
  else if (hashset_contains(&block.actionKinds, &id))
  {
    hashset_remove(&block.actionKinds,&id);
    struct TblBlock *cur = block.tbls_head.next;
    struct TblBlock *pre = &block.tbls_head;
    // 找到actionkind对应的tbl块
    while (cur != NULL)
    {
      if (cur->actionKind != id)
      {
        pre = cur;
        cur = pre->next;
        continue;
      }
      // 否则进行删除处理
      cur = pre->next;
      pre->next->next = NULL;
      delTblBlocks(pre);
      pre->next = cur;
      break;
    }
  }
  // 否则是actions,查找位置,删除
  else
  {
    // 遍历块,查询到是哪个块的
    struct TblBlock *cur = block.tbls_head.next;
    struct TblBlock *pre = &block.tbls_head;
    // 找到actionkind对应的tbl块
    int hasDel=0;
    while (cur != NULL)
    {
      if (!hashset_contains(&cur->actions, &id))
      {
        pre = cur;
        cur = pre->next;
        continue;
      }
      hashset_remove(&cur->actions,&id);
      if(id==cur->defaultAction){
        cur->defaultAction=-1;
      }
      hasDel=1;
      break;
    }
    if(!hasDel) return 0;
  }
  return 1;
}


int gtg_replaceString(char* input){
  char* stops=">";
  char old[200];
  char* new;
  char end=mysgets(old,stops,input);
  if(end!='>') return 0;
  if(old[strlen(old)-1]!='-') return 0;
  else old[strlen(old)-1]='\0';
  new=input+strlen(old)+2;
  //要替换成的新字符串不能够是已经使用的字符串
  if(isUsedStr(new)) return 0;
  int id=strToId(block.strIds,old);
  if(id<0) return 0;
  resetIdString(&block.idAllocator,id,new);
  delStr(block.strIds,old);
  putStrId(block.strIds,new,id);
  return 1;
}


int syntax_line_add(char* toAdd,struct TblBlock* tmpTbl){
  if(tmpTbl==NULL||toAdd==NULL) return 0;
  char* stops=",";
  char tmp[1000];
  char end=mysgets(tmp,stops,toAdd);
  toAdd+=strlen(tmp)+1;
  int symbol=strToId(block.strIds,tmp);
  //如果symbol是等于*,表示能够匹配所有的symbol,则对所有的symbol进行修改
  if(strcmp(tmp,"*")==0){
    end=mysgets(tmp,stops,toAdd);
    if(end!=',') return 0;
    toAdd+=strlen(tmp)+1;
    char action[200];
    end=mysgets(action,stops,toAdd);
    if(strToId(block.strIds,action)<0) return 0;
    int* symbols=hashset_toArr(&block.symbols);
    if(strcmp(tmp,"*")==0){
      //把所有symbol,token都的跳转都设置为这个action
      int* tokens=hashset_toArr(&block.tokens);
      for(int i=0;i<block.symbols.num;i++){
        char* symbolStr=getIdString(&block.idAllocator,symbols[i]);
        for(int j=0;j<block.tokens.num;j++){
          char newAdd[500];
          char* tokenStr=getIdString(&block.idAllocator,tokens[j]);
          sprintf(newAdd,"%s,%s,%s",symbolStr,tokenStr,action);
          syntax_line_add(newAdd,tmpTbl);
          free(tokenStr);
        }
        free(symbolStr);
      }
      free(tokens);
    }
    else{
      for(int i=0;i<block.symbols.num;i++){
        char newAdd[500];
        char* symbolStr=getIdString(&block.idAllocator,symbols[i]);
        sprintf(newAdd,"%s,%s,%s",symbolStr,tmp,action);
        syntax_line_add(newAdd,tmpTbl);
        free(symbolStr);
      }
    }
    free(symbols);
    return 1;
  }
  
  if(symbol<0||end!=',') return 0;
  if(!hashset_contains(&block.symbols,&symbol)) return 0;
  end=mysgets(tmp,stops,toAdd);
  toAdd+=strlen(tmp)+1;
  if(strcmp(tmp,"*")==0){
    if(end!=',')  return 0;
    char* symbolStr=getIdString(&block.idAllocator,symbol);
    int* tokens=hashset_toArr(&block.tokens);
    char action[200];
    mysgets(action,stops,toAdd);
    for(int i=0;i<block.tokens.num;i++){
      char newLine[500];
      char* tokenStr=getIdString(&block.idAllocator,tokens[i]);
      sprintf(newLine,"%s,%s,%s",symbolStr,tokenStr,action);
      syntax_line_add(newLine,tmpTbl);
      free(tokenStr);
    }
    free(tokens);
    free(symbolStr);
    return 1;
  }
  int token=strToId(block.strIds,tmp);
  if(token<0||end!=',') return 0;
  if(!hashset_contains(&block.tokens,&token)) return 0;
  end=mysgets(tmp,stops,toAdd);
  int action=strToId(block.strIds,tmp);
  if(action<0) return 0;  //如果action未注册,报错
  // //如果该action不属于这个表,不报错,因为栈动作相关的表需要symbol作为表元素
  // if(!hashset_contains(&tmpTbl->actions,&action)) return 0;
  //否则加入这个表,而且是从表头后第一位,压入栈顶,越晚加入的内容越靠近表头
  struct syntax_line* newSL=malloc(sizeof(struct syntax_line));
  newSL->next=tmpTbl->syntaxs_head.next;
  tmpTbl->syntaxs_head.next=newSL;
  newSL->action=action;
  newSL->token=token;
  newSL->symbol=symbol;
  tmpTbl->syntax_num++;
  return 1;
}


int gtg_showTbl(struct TblBlock *tmpTbl)
{
  if (tmpTbl == NULL)
    return 0;
  int id = tmpTbl->actionKind;
  char *actionkind = getIdString(&block.idAllocator, id);
  // 判断这个字符串是否存在，不可能不存在,如果不存在就是出了异常
  if (actionkind == NULL)
    return 0;
  fprintf(fout, "Table of %s:\n", actionkind);
  fprintf(fout, "default action:");
  if (tmpTbl->defaultAction == NOT_DEFINE_ID)
  {
    if (block.not_define == NULL)
      fprintf(fout, "%s", DEFAULT_NOTDEFINE_STRING);
    else
      fprintf(fout, "%s", block.not_define);
  }
  else
  {
    char *defaultAction = getIdString(&block.idAllocator, tmpTbl->defaultAction);
    if(defaultAction==NULL){
      dropIdUseTimes(&block.idAllocator,tmpTbl->defaultAction,1);
      tmpTbl->defaultAction=NOT_DEFINE_ID;
    }
    else{
      fprintf(fout, "%s", defaultAction);
      free(defaultAction);
    }
  }
  fprintf(fout, "\n");
  gtg_showSyntaxs(tmpTbl,actionkind);
  free(actionkind);
  return 1;
}

int gtg_showSyntaxs(struct TblBlock* tmpTbl,char* actionkind)
{
  struct syntax_line *tmp_syntax_line =tmpTbl->syntaxs_head.next;
  struct syntax_line *pre = &tmpTbl->syntaxs_head;
  while (tmp_syntax_line != NULL)
  {
    char *symbol = getIdString(&block.idAllocator, tmp_syntax_line->symbol);
    char *token = getIdString(&block.idAllocator, tmp_syntax_line->token);
    char *action = getIdString(&block.idAllocator, tmp_syntax_line->action);
    // 进行检查
    int isLegal = 1; // 标记是否是个正常的syntax
    if (symbol == NULL)
    {
      // 把这个id的引用次数减1
      isLegal = 0;
      dropIdUseTimes(&block.idAllocator, tmp_syntax_line->symbol, 1);
    }
    if (token == NULL)
    {
      // 把这个id的引用次数减1
      isLegal = 0;
      dropIdUseTimes(&block.idAllocator, tmp_syntax_line->token, 1);
    }
    if (action == NULL)
    {
      // 把这个id的引用次数减1
      isLegal = 0;
      dropIdUseTimes(&block.idAllocator, tmp_syntax_line->action, 1);
    }
    if (!isLegal)
    {
      pre->next = tmp_syntax_line->next;
      free(tmp_syntax_line);
      tmp_syntax_line = pre->next;
      tmpTbl->syntax_num--;
      continue;
    }
    fprintf(fout, "%s:%s,%s,%s\n", actionkind, symbol, token, action);
    free(symbol);
    free(token);
    free(action);
    pre = tmp_syntax_line;
    tmp_syntax_line = tmp_syntax_line->next;
  }
}


//获取对应actionkind的表
struct TblBlock* getTbl(char* actionkind){
  int id=strToId(block.strIds,actionkind);
  if(id<0) return NULL;
  struct TblBlock* kindof=NULL;
  kindof=block.tbls_head.next;
  while (kindof!=NULL&&kindof->actionKind!=id) kindof=kindof->next;
  return kindof;
}