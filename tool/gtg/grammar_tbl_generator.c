#include "grammar_tbl_generator.h"

//�ó���Ŀ��,ʵ��һ���ܹ���������yjhc�õ��﷨�ļ��ı༭��

//��ʼ����������,
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
  //ע��,ɾ���﷨�е�ʱ��,Ҫ�����Ӧ��idAllocator��������ô���ͳ��
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
    free_hashset(&tmp->actions);
    int dropTimes=delSyntaxs(&tmp->syntaxs_head);
    free(tmp);
    if(dropTimes<0) return 0;
  }
  return 1;
}


//�����ж�ȡ��,��ȡһ������,���Һ���ǰ���ո�,����ע�ͷ�����,���Ҷ�������ֹͣ,����û�г�������,��̬����ռ�
char* fgetOrd(FILE* fin){
  int baseSize=100;
  int size=baseSize;
  char* out=malloc(sizeof(char)*size);
  int i=0;
  char c;
  //����������Ч����ǰ�Ŀո�
  while((c=fgetc(fin))!=EOF){
    if(c==' ') continue;
    if(c=='#'){
      while((c=fgetc(fin))!=EOF&&c!='\n');
      if(c==EOF) break;
    }else if(c=='\n') return strcpy(malloc(1),"");
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


//��������,����ǲ�����������򷵻�0,������ɹ��󷵻ط�0ֵ
int maintainOrd(char* ord){
  if(ord==NULL) return 0;
  OrdKind kind=ordKind(ord);
  //��ִ��ǰ����-o��-n��Ϣ����ȡ
  //TODO,��ȡ-o���Ժ�-n����
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
  n_gtg=0;    //���֮ǰ���������ִ�С
  return 1;
}

int isUsedStr(char* str){
  //���Ȳ����ַ���id���в��Ҹ��ַ����ı��,������ҵ���˵���Ѿ�ʹ�ù���
  int jud=strToId(block.strIds,str);
  if(jud<0) return 0;
  return -1;
}

//��ȡһ������,����ǰ���ո�͵���ע���Լ�
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

//�ж���������
OrdKind ordKind(char* ord){
  char* stops=" \n\r";
  //�ж��Ƿ��ǿ���������
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
  //�ж��Ƿ�����ʾ����
  else if(strcmp(tmp,"help")==0){
    return HELP;
  }
  //�ж��Ƿ����滻����/ɾ������
  else if(strcmp(tmp,"replace")==0){
    return REPLACE;
  }
  //�ж��Ƿ�����������
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
    else if(strcmp(tmp,"notDefine")) return SET_NOT_DEFINE;
  }
  //�ж��Ƿ��ǲ鿴����
  else if(strcmp(tmp,"check")==0){
    end=mysgets(tmp,stops,ord+strlen(tmp)+1);
    if(strcmp(tmp,"actions")==0){
      return CHECK_ACTION_ALL;
    }
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
  //�ж��Ƿ����������
  else if(strcmp(tmp,"output")==0){
    end=mysgets(tmp,stops,ord+strlen(tmp)+1);
    if(strcmp(tmp,"-o")==0) return OUTPUT_ORDERS;
    else if(strcmp(tmp,"-g")==0) return OUTPUT_GRAMMAR;
  }

  //������δ��������
  return NOT_DEFINE_ORD;
}



/*
���ִ�������ķ�ʽ
һ���������ʽ������
*/

int error() //������ʾ,������û�ж���������ʱ�����������ʾ
{
  printf("your input is not defined in gtg grammar!\n");
  printf("you could enter \"help\" to watch help messages\n");
  return 1;
}

int gc(){
  //��������,���ں���δ�����﷨��ľ��ӽ��л���



  return error();
}

int cls(){
  system("cls");
  return 1;
}

int set_defaultAction(){
  //���ȶ�ȡactionkind
  char* track=ord;
  char tmp[1000];
  char end;
  //��ȡactionkind����
  end=mysgets(tmp," ",track);
  if(end==' ') track+=strlen(tmp)+1;
  else return error();
  end=mysgets(tmp," ",track);
  if(end==' ') track+=strlen(tmp)+1;
  else return error();
  //��ʼ��ȡactionkind��Ϊ����
  char actionkind[200];
  end=mysgets(actionkind," ",track);
  struct tblBlock* targetTbl=getTbl(actionkind);
  if(targetTbl==NULL) return 0;
  char* action=fgetWord(fin);
  //�����Ҫ�ָ���Ĭ������
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
  if(block.not_define==NULL) block.not_define=newNotDefineString;
  else{
    free(block.not_define);
    block.not_define=newNotDefineString;
  }
  return 1;
}

int action_add(){
  //TODO fix
  char* track=ord;
  char tmp[1000];
  char end;
  int id;
  //��ȡactionkind����
  end=mysgets(tmp," ",track);
  if(end==' ') track+=strlen(tmp)+1;
  else return error();
  end=mysgets(tmp," ",track);
  if(end==' ') track+=strlen(tmp)+1;
  else return error();
  //��ʼ��ȡactionkind��Ϊ����
  char actionkind[500];
  end=mysgets(actionkind," ",track);
  struct tblBlock* tbl=getTbl(actionkind);
  if(tbl==NULL) return 0;
  //���û�и���-n����,���ȡ��һ������������Ϊ����
  if(n_gtg==0){
    char* tord=fgetOrd(fin);
    //Ȼ���ord���������ݼ��뵽��actionkind��action��
    track=tord;
    int islast=1;
    while ((end = mysgets(tmp, " ", track)) != '\0'||islast)
    {
      track+=strlen(tmp)+1;
      if(end=='\0') islast=0;
      if (strlen(tmp) == 0)
        continue;
      //���ж����������û���ù�
      if (isUsedStr(tmp))
      {
        fprintf(fout,"%s has been input!\n",tmp);
        continue;
      }
      id = allocateId(&block.idAllocator, tmp);
      if(id<0) return 0;
      putStrId(block.strIds, tmp, id);
      //����actions���м�������
      hashset_add(&tbl->actions, &id);
    }
    free(tord);
  }
  //�����ȡ������n����Ч�ַ�����Ϊ��action������
  else{
    for(int i=0;i<n_gtg;i++){
      char* tmp=fgetWord(fin);
      if(strlen(tmp)==0){
        free(tmp);
        continue;
      }
      //���ж����������û���ù�
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
  //��ȡactionkind����
  end=mysgets(tmp," ",track);
  if(end==' ') track+=strlen(tmp)+1;
  else return error();
  end=mysgets(tmp," ",track);
  if(end==' ') track+=strlen(tmp)+1;
  else return error();
  //��ʼ��ȡactionkind��Ϊ����
  char actionkind[200];
  end=mysgets(actionkind," ",track);
  int id=strToId(block.strIds,actionkind);
  struct tblBlock* kindof=NULL;
  if(id>=0){
    kindof=block.actions_head.next;
    while (kindof!=NULL&&kindof->actionKind!=id) kindof=kindof->next;
  }
  //���û��-n����,��ȡ���е�n��
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
      struct tblBlock* tmpTbl=block.actions_head.next;
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
  //�����-n����,���ȡ������n����Ϣ��Ϊ����
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
      struct tblBlock *tmpTbl = block.actions_head.next;
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
  //��ȡ-n����
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
  //��ȡ-n����
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
  //���û�и���-n����,ʹ�ý�����һ����Ϊ����
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
      //����,�����ַ�������id
      int id=allocateId(&block.idAllocator,tmp);
      //Ȼ���id���뵽�ַ���id����,ͬʱ��id���뵽symbols����
      if(id>=0){
        putStrId(block.strIds,tmp,id);
        hashset_add(&block.symbols,&id);
      }
      else fprintf(fout,"\nerror!fail to allocate id for %s\n",tmp);
      
    }while(end!='\0'&&end!='#');
    free(line);
  }
  //����ʹ�ý�����n��������Ϊ����
  else{
    for(int i=0;i<n_gtg;i++){
      char* tmp=fgetWord(fin);
      //�ж��Ƿ����Ѿ�������ַ���
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
      //����,�����ַ�������id
      int id=allocateId(&block.idAllocator,tmp);
      //Ȼ���id���뵽�ַ���id����,ͬʱ��id���뵽symbols����
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
  //���û�и���-n����,ʹ�ý�����һ����Ϊ����
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
      //����,�����ַ�������id
      int id=allocateId(&block.idAllocator,tmp);
      //Ȼ���id���뵽�ַ���id����,ͬʱ��id���뵽symbols����
      if(id>=0){
        putStrId(block.strIds,tmp,id);
        hashset_add(&block.tokens,&id);
      }
      else fprintf(fout,"\nerror!fail to allocate id for %s\n",tmp);
      
    }while(end!='\0'&&end!='#');
    free(line);
  }
  //����ʹ�ý�����n��������Ϊ����
  else{
    for(int i=0;i<n_gtg;i++){
      char* tmp=fgetWord(fin);
      //�ж��Ƿ����Ѿ�������ַ���
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
      //����,�����ַ�������id
      int id=allocateId(&block.idAllocator,tmp);
      //Ȼ���id���뵽�ַ���id����,ͬʱ��id���뵽symbols����
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
  //���û�и���-n����,ʹ�ý�����һ����Ϊ����
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
      //����,�����ַ�������id
      int id=allocateId(&block.idAllocator,tmp);
      //Ȼ���id���뵽�ַ���id����,ͬʱ��id���뵽symbols����
      if(id>=0){
        putStrId(block.strIds,tmp,id);
        hashset_add(&block.actionKinds,&id);
        //�����µ�actionkind��
        struct tblBlock* addTbl=malloc(sizeof(struct tblBlock));
        addTbl->next=block.actions_head.next;
        block.actions_head.next=addTbl;
        addTbl->actionKind=id;
        addTbl->syntax_num=0;
        addTbl->actions=hashset_cre(sizeof(int));
        addTbl->defaultAction=NOT_DEFINE_ID; //������ʾʹ��Ĭ�ϵ�id
        addTbl->syntaxs_head.next=NULL;
      }
      else fprintf(fout,"\nerror!fail to allocate id for %s\n",tmp);
      
    }while(end!='\0'&&end!='#');
    free(line);
  }
  //����ʹ�ý�����n��������Ϊ����
  else{
    for(int i=0;i<n_gtg;i++){
      char* tmp=fgetWord(fin);
      //�ж��Ƿ����Ѿ�������ַ���
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
      //����,�����ַ�������id
      int id=allocateId(&block.idAllocator,tmp);
      //Ȼ���id���뵽�ַ���id����,ͬʱ��id���뵽symbols����
      if(id>=0){
        putStrId(block.strIds,tmp,id);
        hashset_add(&block.actionKinds,&id);
        //�����µ�actionkind��
        struct tblBlock* addTbl=malloc(sizeof(struct tblBlock));
        addTbl->next=block.actions_head.next;
        block.actions_head.next=addTbl;
        addTbl->actionKind=id;
        addTbl->actions=hashset_cre(sizeof(int));
        addTbl->syntax_num=0;
        addTbl->defaultAction=-1; //������ʾʹ��Ĭ�ϵ�id
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
  //Ȼ��չʾ�ڿ���̨��
  char c;
  while((c=fgetc(fh))!=EOF) fputc(c,stdout);
  fclose(fh);
  fputc('\n',stdout);
  return 1;
}

//in this way you will del a string.
int del(){
  //���û��-n����ָ��,��ɾ����һ������������ַ���
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
    //����ɶ���ʱdel����
    for(int i=0;i<n_gtg;i++){
      char* tmp=fgetWord(fin);
      //�ж��Ƿ����Ѿ�������ַ���
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
    //����ɶ���ʱdel����
    for(int i=0;i<n_gtg;i++){
      char* tmp=fgetWord(fin);
      if(!gtg_replaceString(tmp)) fprintf(fout,"fail to execute replace \"%s\"\n",tmp);
    }
  }
  return 1;
}

int gtg_exit(){
  //������������д���ļ�
  fout=fopen(DEFAULT_GTG_WORK_FILE,"w");

  if(fout==NULL) return 0;
  if(ord!=NULL) free(ord);
  ord=NULL; 
  output_orders();  
  fclose(fout);
  //�ͷſռ�
  //���symbol���ͷſռ�
  free_hashset(&block.symbols);
  //���token���ͷ�
  free_hashset(&block.tokens);
  //���actionkind���ͷſռ�
  free_hashset(&block.actionKinds);
  //����﷨��
  delTblBlocks(&block.actions_head);
  //�ͷŲ�����id������
  freeIdAllocator(&block.idAllocator);
  //�ͷ��ַ���-id��ϣ��
  delStrIdTable(block.strIds);
  free(block.strIds);
  //����Ĭ���ַ���
  if(block.not_define!=NULL){
    free(block.not_define);
    block.not_define=NULL;
  }
  exit(0);
}

int gtg_init(){

  //���symbol���ͷſռ�
  free_hashset(&block.symbols);
  //���token���ͷ�
  free_hashset(&block.tokens);
  //���actionkind���ͷſռ�
  free_hashset(&block.actionKinds);
  //����﷨��
  delTblBlocks(&block.actions_head);
  //�ͷŲ�����id������
  freeIdAllocator(&block.idAllocator);
  block.idAllocator=getIdAllocator();
  //�ͷ��ַ���-id��ϣ��
  delStrIdTable(block.strIds);
  //����Ĭ���ַ���
  if(block.not_define!=NULL){
    free(block.not_define);
    block.not_define=NULL;
  }
  return 1;
}




int check_actions_ofkind(){
  //TODO���ҵ���Ӧ�Ŀ�,�鿴action
  //�ȶ�ȡactionkind
  char tmp[1000];
  char end=mysgets(tmp," ",ord);
  if(end=='\0') return 0;
  char* tord=ord+strlen(tmp)+1;
  end=mysgets(tmp," ",tord);
  if(end=='\0') return 0;
  tord+=strlen(tmp)+1;
  end=mysgets(tmp," ",tord);
  int id=strToId(block.strIds,tmp);
  //���Ҹ��ַ����Ƿ��Ѿ�ע��,����ע��id
  //�������idС��0,��˵�����ַ���ûע��,�������ܶ�Ӧactionkind
  if(id<0) return 0;
  if(!hashset_contains(&block.actionKinds,&id)) return 0;
  //�ҵ�Ŀ�ı��
  struct tblBlock* target=block.actions_head.next;
  while(target!=NULL&&target->actionKind!=id) target=target->next;
  if(target==NULL) return 0;
  //չʾactions
  int* arr=hashset_toArr(&target->actions);
  showStringsByIds(arr,target->actions.num);
  free(arr);
  return 1;
}


int check_actions_all(){
  struct tblBlock* target=block.actions_head.next;
  while(target!=NULL){
    char* actionkind=getIdString(&block.idAllocator,target->actionKind);
    fprintf(fout,"actionkind:%s\n",actionkind);
    if(target==NULL) return 0;
    //չʾactions
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


int check_all(){
  fprintf(fout,"All symbols:\n");
  if(!check_symbol()) return 0;
  fprintf(fout,"All tokens:\n");
  if(!check_token()) return 0;
  fprintf(fout,"All actionkinds:\n");
  if(!check_actionkind()) return 0;
  fprintf(fout,"All actions:\n");
  if(!check_actions_all()) return 0;
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
  //���ȶ�ȡactionkind
  char tmp[1000];
  char* tord=ord;
  char end=mysgets(tmp," ",tord);
  if(end==' ')  tord+=strlen(tmp)+1;
  else return 0;
  end=mysgets(tmp," ",tord);
  if(end==' ')  tord+=strlen(tmp)+1;
  else return 0;
  end=mysgets(tmp," ",tord);
  //Ȼ�����actionkind�ҵ���Ӧ�Ŀ�
  struct tblBlock* target=getTbl(tmp);
  if(target==NULL) return 0;
  return gtg_showTbl(target);
}


int check_syntaxs_all(){
  //ѭ����ӡ��ͬactionkind��action
  struct tblBlock* tmpTbl=block.actions_head.next;
  int jud=1;
  while(tmpTbl!=NULL){
    jud=jud&&gtg_showTbl(tmpTbl);
    tmpTbl=tmpTbl->next;
  }
  return jud;
}

//�鿴ָ���������͵�Ĭ�϶���
int check_default_action(){
  //�Ȼ�ȡactionkind���ڵĿ� 
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
  struct tblBlock* tbl=getTbl(tmp);
  if(tbl==NULL) return 0; 
  char* default_action=getIdString(&block.idAllocator,tbl->defaultAction);
  fprintf(fout,"%s\n",default_action);
  return 1;
}

int output_orders(){
  gc(); //�Ƚ�������
  //����������е�symbol
  int num=block.symbols.num;
  if(num!=0){
    fprintf(fout,"symbol add -n %d\n",num);
    check_symbol();
  }
  //Ȼ���������token
  num=block.tokens.num;
  if(num!=0){
    fprintf(fout,"token add -n %d\n",num);
    check_token();
  }
  //Ȼ��������е�actionkind
  num=block.actionKinds.num;
  if(num!=0){
    fprintf(fout,"actionkind add -n %d\n",num);
    check_actionkind();
  }
  //Ȼ�����ÿ��actionkind��action��syntax
  struct tblBlock* tmpTbl=block.actions_head.next;
  while(tmpTbl!=NULL){
    //�����Ӧ����Ӧ��default actionkind
    char* actionkind=getIdString(&block.idAllocator,tmpTbl->actionKind);
    if(actionkind==NULL) return 0;
    //����д��action,����еĻ�
    if(tmpTbl->actions.num!=0){
      int* arr=hashset_toArr(&tmpTbl->actions);
      num=tmpTbl->actions.num;
      fprintf(fout,"action add %s\n",actionkind);
      showStringsByIds(arr,num);
      free(arr);
    }
    //Ȼ��д��default action
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
    //���д��syntaxs
    if(tmpTbl->syntax_num!=0){
      fprintf(fout,"syntax add %s -n %d\n",actionkind,tmpTbl->syntax_num);
      gtg_showSyntaxs(tmpTbl,actionkind);
    }
    free(actionkind);
    tmpTbl=tmpTbl->next;
  }
  return 1;
}


int output_grammar(){return error();

}


/*
other
*/

int gtg_delString(char* tmp)
{
  if(!isUsedStr(tmp)) return 0;
  int id=strToId(block.strIds,tmp);
  if(id<0) return 0;
  // Ȼ�������id���ַ����İ�
  delString(&block.idAllocator, id);
  // �������id���ַ���id����ȡ��
  delStr(block.strIds, tmp);
  // Ȼ���ж��Ƿ��Ǹ������͵�����,�����,�Ӽ�����ȡ��
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
    struct tblBlock *cur = block.actions_head.next;
    struct tblBlock *pre = &block.actions_head;
    // �ҵ�actionkind��Ӧ��tbl��
    while (cur != NULL)
    {
      if (cur->actionKind != id)
      {
        pre = cur;
        cur = pre->next;
        continue;
      }
      // �������ɾ������
      cur = pre->next;
      pre->next->next = NULL;
      delTblBlocks(pre);
      pre->next = cur;
      break;
    }
  }
  // ������actions,����λ��,ɾ��
  else
  {
    // ������,��ѯ�����ĸ����
    struct tblBlock *cur = block.actions_head.next;
    struct tblBlock *pre = &block.actions_head;
    // �ҵ�actionkind��Ӧ��tbl��
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
  //Ҫ�滻�ɵ����ַ������ܹ����Ѿ�ʹ�õ��ַ���
  if(isUsedStr(new)) return 0;
  int id=strToId(block.strIds,old);
  if(id<0) return 0;
  resetIdString(&block.idAllocator,id,new);
  delStr(block.strIds,old);
  putStrId(block.strIds,new,id);
  return 1;
}


int syntax_line_add(char* toAdd,struct tblBlock* tmpTbl){
  if(tmpTbl==NULL||toAdd==NULL) return 0;
  char* stops=",";
  char tmp[1000];
  char end=mysgets(tmp,stops,toAdd);
  toAdd+=strlen(tmp)+1;
  int symbol=strToId(block.strIds,tmp);
  if(symbol<0||end!=',') return 0;
  end=mysgets(tmp,stops,toAdd);
  toAdd+=strlen(tmp)+1;
  int token=strToId(block.strIds,tmp);
  if(token<0||end!=',') return 0;
  end=mysgets(tmp,stops,toAdd);
  toAdd+=strlen(tmp)+1;
  int action=strToId(block.strIds,tmp);
  //�����action�����������,����
  if(!hashset_contains(&tmpTbl->actions,&action)) return 0;
  //������������
  struct syntax_line* newSL=malloc(sizeof(struct syntax_line));
  newSL->next=tmpTbl->syntaxs_head.next;
  tmpTbl->syntaxs_head.next=newSL;
  newSL->action=action;
  newSL->token=token;
  newSL->symbol=symbol;
  tmpTbl->syntax_num++;
  return 1;
}





int gtg_showTbl(struct tblBlock *tmpTbl)
{
  if (tmpTbl == NULL)
    return 0;
  int id = tmpTbl->actionKind;
  char *actionkind = getIdString(&block.idAllocator, id);
  // �ж�����ַ����Ƿ���ڣ������ܲ�����,��������ھ��ǳ����쳣
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

int gtg_showSyntaxs(struct tblBlock* tmpTbl,char* actionkind)
{
  struct syntax_line *tmp_syntax_line =tmpTbl->syntaxs_head.next;
  struct syntax_line *pre = &tmpTbl->syntaxs_head;
  while (tmp_syntax_line != NULL)
  {
    char *symbol = getIdString(&block.idAllocator, tmp_syntax_line->symbol);
    char *token = getIdString(&block.idAllocator, tmp_syntax_line->token);
    char *action = getIdString(&block.idAllocator, tmp_syntax_line->action);
    // ���м��
    int isLegal = 1; // ����Ƿ��Ǹ�������syntax
    if (symbol == NULL)
    {
      // �����id�����ô�����1
      isLegal = 0;
      dropIdUseTimes(&block.idAllocator, tmp_syntax_line->symbol, 1);
    }
    if (token == NULL)
    {
      // �����id�����ô�����1
      isLegal = 0;
      dropIdUseTimes(&block.idAllocator, tmp_syntax_line->token, 1);
    }
    if (action == NULL)
    {
      // �����id�����ô�����1
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


//��ȡ��Ӧactionkind�ı�
struct tblBlock* getTbl(char* actionkind){
  int id=strToId(block.strIds,actionkind);
  if(id<0) return NULL;
  struct tblBlock* kindof=NULL;
  kindof=block.actions_head.next;
  while (kindof!=NULL&&kindof->actionKind!=id) kindof=kindof->next;
  return kindof;
}



