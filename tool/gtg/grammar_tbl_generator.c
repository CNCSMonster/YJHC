#include "grammar_tbl_generator.h"

//该程序目的,实现一个能够用来生成yjhc用的语法文件的编辑器

//初始化程序数据,
void init(){
  block.actionKinds=hashset_cre(sizeof(int));
  block.actions_head.next=NULL;
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


int delTblBlocks(struct tblBlock* tbls_head){
  while (tbls_head->next!=NULL)
  {
    struct tblBlock* tmp=tbls_head->next;
    tbls_head->next=tmp->next;
    int actionkind=tmp->actionKind;
    hashset_del(&tmp->actions);
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
  //忽略所有有效命令前的空格以及换行
  while((c=fgetc(fin))!=EOF){
    if(c==' ') continue;
    if(c=='#'){
      while((c=fgetc(fin))!=EOF&&c!='\n');
      if(c==EOF) break;
    }else{
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
    for(int j=0;j<n_each_line_gtg-1&&i<num;j++){
      str=getIdString(&block.idAllocator,ids[i++]);
      if(str==NULL) return 0;
      fprintf(fout,"%s ",str);
      free(str);
    }
    if(i<num){
      str=getIdString(&block.idAllocator,ids[i++]);
      if(str==NULL) return 0;
      fprintf(fout,"%s",str);
      free(str);
    }
    fprintf(fout,"\n");
  }
  return 1;
}


//处理命令,如果是不合理的命令则返回0,否则处理成功后返回非0值
int maintainOrd(char* ord){
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
    printf("fail to execute such line ");
  }
  if(fout!=stdout) fclose(fout);
  if(ord!=NULL)  free(ord);
  ord=NULL;
  return 1;
}

int isUsedStr(char* str){
  //首先查找字符串id表中查找该字符串的编号,如果查找到了说明已经使用过了
  int jud=strToId(block.strIds,str);
  if(jud<0) return 0;
  return 1;
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
      end=myfgets(tmp,"\n",fin);
    }
    else if(strlen(tmp)==0) continue;
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
  //判断是否是查看命令
  else if(strcmp(tmp,"check")==0){
    end=mysgets(tmp,stops,ord+strlen(tmp)+1);
    if(strcmp(tmp,"actions")==0){
      return CHECK_ACTION_ALL;
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
  return NOT_DEFINE;
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
  //垃圾回收,对于含有未定义语法块的句子进行回收






  return error();
}

int actionkind_setdefault(){return error();
  
}

int set_not_define(){return error();

}

int action_add(){
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
  end=mysgets(tmp," ",track);
  track+=strlen(tmp)+1;
  //判断actionkind是否合理
  int id;
  int ackindId;
  //如果字符串表类里面没有这个字符串,或者这个字符串对应的id没有保存在actionkind表里面
  if((id=strToId(block.strIds,tmp))<0||!hashset_contains(&block.actionKinds,&id)){
    printf("your input is not correct id!");
    return error();
  }
  ackindId=id;    //保存actionkind的id,后面使用

  //如果没有附加-n属性,则读取下一行所有内容作为输入
  if(n_gtg==0){
    free(ord);
    ord=fgetOrd(fin);
    //然后把ord中所有内容加入到该actionkind的action中
    track=ord;
    int islast=1;
    //TODO
    while ((end = mysgets(tmp, " ", track)) != '\0'||islast)
    {
      track+=strlen(tmp)+1;
      if(end=='\0') islast=0;
      if (strlen(tmp) == 0)
        continue;
      //再判断这个名字有没有用过
      if (isUsedStr(tmp))
      {
        fprintf(fout,"%s has been input!\n");
        continue;
      }
      id = allocateId(&block.idAllocator, tmp);
      putStrId(block.strIds, tmp, id);
      // TODO,找到对应actionkind的块,加入该内容
      struct tblBlock *tmpAEK = block.actions_head.next;
      while (tmpAEK != NULL && tmpAEK->actionKind != ackindId)
      {
        tmpAEK = tmpAEK->next;
      }
      if (tmpAEK == NULL)
      {
        fprintf(fout,"actions block for such actionkind doesn't exist!\n");
        fclose(fout);
        return 0;
      }
      //往该actions块中加入内容
      hashset_add(&(tmpAEK->actions), &id);
    }
  }
  //否则读取接下来n个有效字符串作为该action的输入
  else{
    for(int i=0;i<n_gtg;i++){
      while((end=myfgets(tmp," \n#",fin))!=EOF){
        if(end=='#'){
          end=myfgets(tmp,"\n",fin);
          if(end==EOF) break;
          else continue;
        }
        else if(strlen(tmp)==0) continue;
        //再判断这个名字有没有用过
        if(isUsedStr(tmp)){
          fprintf(fout,"%s has been input!\n");
          fclose(fout);
          return 0;
        }
        id=allocateId(&block.idAllocator,tmp);
        putStrId(block.strIds,tmp,id);
        //TODO,找到对应actionkind的块,加入该内容
        struct tblBlock* tmpAEK=block.actions_head.next;
        while(tmpAEK!=NULL&&tmpAEK->actionKind!=ackindId){
          tmpAEK=tmpAEK->next;
        }
        if(tmpAEK==NULL){
          fprintf(fout,"actions block for such actionkind doesn't exist!\n");
          fclose(fout);
          return 0;
        }
        //往该actions块中加入内容
        hashset_add(&(tmpAEK->actions),&id);
        break;
      }
    }
  }
  return 1;
}

int syntax_add(){
  //TODO
  //如果没有-n属性
  if(n_gtg==0){

  }
  //如果有-n属性,则读取接下来n个信息作为输入
  else{

  }

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
  //读取-n属性
  while((end=mysgets(returnPath,"-",line))!='\0'){
    line+=strlen(returnPath)+1;
    end=mysgets(returnPath," ",line);
    if(end!=' '){
      return 0;
    }else if(strcmp("o",returnPath)==0){
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
        fprintf(fout,"\nerror!you can't use empty string as symbol!\n",tmp);
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
        fprintf(fout,"\nerror!you can't use empty string as token!\n",tmp);
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
        fprintf(fout,"\nerror!you can't use empty string as symbol!\n",tmp);
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
        fprintf(fout,"\nerror!you can't use empty string as symbol!\n",tmp);
        continue;
      }
      //首先,给该字符串分配id
      int id=allocateId(&block.idAllocator,tmp);
      //然后把id加入到字符串id表中,同时把id加入到symbols表中
      if(id>=0){
        putStrId(block.strIds,tmp,id);
        hashset_add(&block.actionKinds,&id);
        //加入新的actionkind块
        struct tblBlock* addTbl=malloc(sizeof(struct tblBlock));
        addTbl->next=block.actions_head.next;
        block.actions_head.next=addTbl;
        addTbl->actionKind=id;
        addTbl->actions=hashset_cre(sizeof(int));
        addTbl->defaultAction=-1; //负数表示使用默认的id
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
        fprintf(fout,"\nerror!you can't use empty string as symbol!\n",tmp);
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
        struct tblBlock* addTbl=malloc(sizeof(struct tblBlock));
        addTbl->next=block.actions_head.next;
        block.actions_head.next=addTbl;
        addTbl->actionKind=id;
        addTbl->actions=hashset_cre(sizeof(int));
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
int del(){return error();

}

int replace(){return error();

}

int gtg_exit(){

  //先进行空间整理和化简
  
  //把所有内容先写入文件
  fout=fopen(DEFAULT_GTG_WORK_FILE,"w");

  if(fout==NULL) return 0;
  if(ord!=NULL) free(ord);
  ord=NULL; 
  //TODO保存数据到文件中
  //先保存symbols
  if(!check_symbol()) return 0;
  //再保存tokens
  if(!check_token()) return 0;

  if(!check_actionkind()) return 0;
  //再保存actions和syntax,根据每个actionkind保存

  fclose(fout);

  //释放空间
  //清空symbol表并释放空间
  hashset_del(&block.symbols);
  //清除token表释放
  hashset_del(&block.tokens);
  //清除actionkind表并释放空间
  hashset_del(&block.actionKinds);
  //清除语法块
  delTblBlocks(&block.actions_head);
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
  hashset_del(&block.symbols);
  //清除token表释放
  hashset_del(&block.tokens);
  //清除actionkind表并释放空间
  hashset_del(&block.actionKinds);
  //清除语法块
  delTblBlocks(&block.actions_head);
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




int check_action_ofkind(){return error();

}


int check_action_all(){return error();

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


int check_all(){
  //执行文件,把-o等属性获取之后去掉-o,然后执行各种check


  return 1;
}
int check_actionkind(){
  int* arr=hashset_toArr(&block.actionKinds);
  int jud=showStringsByIds(arr,block.actionKinds.num);
  return jud;
}

int check_syntax_ofkind(){
  //TODO
  return error();
}


int check_syntaxs(){
  return error();
}


int output_orders(){return error();

}


int output_grammar(){return error();

}