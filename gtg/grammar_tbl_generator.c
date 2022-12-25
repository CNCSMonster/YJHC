#include "grammar_tbl_generator.h"

//�ó���Ŀ��,ʵ��һ���ܹ���������yjhc�õ��﷨�ļ��ı༭��

//��ʼ����������,
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


//�����ж�ȡ��,��ȡһ������,���Һ���ǰ���ո�,����ע�ͷ�����,���Ҷ�������ֹͣ,����û�г�������,��̬����ռ�
char* fgetOrd(FILE* fin){
  int baseSize=100;
  int size=baseSize;
  char* out=malloc(sizeof(char)*size);
  int i=0;
  char c;
  //����������Ч����ǰ�Ŀո��ע��
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


//��������,����ǲ�����������򷵻�0,������ɹ��󷵻ط�0ֵ
int maintainOrd(){
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
  //�����ж��Ƿ��Ĭ���ַ����غ�
  if(block.not_define==NULL&&strcmp(str,DEFAULT_NOTDEFINE_STRING)==0) return 1;
  else if(block.not_define!=NULL&&strcmp(str,block.not_define)==0) return 1;
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
      char tmp2[1000];
      end=myfgets(tmp2,"\n",fin);
    }
    if(strlen(tmp)==0) continue;
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
  else if(strcmp(tmp,"run")==0){
    return RUN;
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
    else if(strcmp(tmp,"notDefine")==0) return SET_NOT_DEFINE;
  }
  //�ж��Ƿ��ǲ鿴����
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
  //��������,�����ظ��������л���

  //��Ҫά��һ����,��¼�Ѿ����ʹ�����Ϣ
  //ÿ��symbol->tokens���
  HSet hset=hashset_cre(sizeof(long long));
  //��ÿ����Ŀ�ʼ����
  struct TblBlock* tbl=block.tbls_head.next;
  while(tbl!=NULL){
    //Ϊÿ�����ʼһ�οռ�
    free_hashset(&hset);
    //Ҫɾ�����������,symbol����token�Ѿ�ʧЧ�����,����(symbol,token)�Ѿ���ǰ����ֹ������
    struct syntax_line* next=tbl->syntaxs_head.next;
    struct syntax_line* pre=&(tbl->syntaxs_head);
    while(next!=NULL){
      //�����жϸþ��Ƿ��Ǻ����
      int symbol=next->symbol;
      int token=next->token;
      int action=next->action;
      if(!hashset_contains(&block.symbols,&symbol)||!hashset_contains(&block.tokens,&token))
      {
        //����token��symbol��action�����ô��������˳�
        dropIdUseTimes(&block.idAllocator,symbol,1);
        dropIdUseTimes(&block.idAllocator,token,1);
        dropIdUseTimes(&block.idAllocator,action,1);
        pre->next=next->next;
        free(next);
        next=pre->next;
        tbl->syntax_num--;
        continue;
      }
      //�����ж��Ƿ��Ѿ����ֹ�
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
  
  //���������hset��Ҫ���տռ�
  free_hashset(&hset);
  return 1;
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
  struct TblBlock* targetTbl=getTbl(actionkind);
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
  //��������Ҫ�趨���ַ������Ѿ���ϵͳ�е��ַ���,���趨ʧ��
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
  struct TblBlock* tbl=getTbl(actionkind);
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
  int ifGet=1;
  //��ȡactionkind����
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
    //��ʼ��ȡactionkind��Ϊ����
    end=mysgets(actionkind," ",track);
    id=strToId(block.strIds,actionkind);
    if(id>=0){
      kindof=block.tbls_head.next;
      while (kindof!=NULL&&kindof->actionKind!=id) kindof=kindof->next;
    }
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
  //��ȡ-o����
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
        struct TblBlock* addTbl=malloc(sizeof(struct TblBlock));
        addTbl->next=block.tbls_head.next;
        block.tbls_head.next=addTbl;
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
        struct TblBlock* addTbl=malloc(sizeof(struct TblBlock));
        addTbl->next=block.tbls_head.next;
        block.tbls_head.next=addTbl;
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

int run(){
  //��ȡ������ļ���
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
  delTblBlocks(&block.tbls_head);
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
  delTblBlocks(&block.tbls_head);
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
  struct TblBlock* target=block.tbls_head.next;
  while(target!=NULL&&target->actionKind!=id) target=target->next;
  if(target==NULL) return 0;
  //չʾactions
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
  struct TblBlock* target=getTbl(tmp);
  if(target==NULL) return 0;
  return gtg_showTbl(target);
}


int check_syntaxs_all(){
  //ѭ����ӡ��ͬactionkind��action
  struct TblBlock* tmpTbl=block.tbls_head.next;
  int jud=1;
  while(tmpTbl!=NULL){
    printShortSplit();
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
  struct TblBlock* tbl=getTbl(tmp);
  if(tbl==NULL) return 0; 
  char* default_action=getIdString(&block.idAllocator,tbl->defaultAction);
  fprintf(fout,"%s\n",default_action);
  return 1;
}

int output_orders(){
  gc(); //�Ƚ�������
  //not_define�ַ�������Ĭ���ַ���ʱд��not_define
  if(block.not_define!=NULL){
    fprintf(fout,"set notDefine\n");
    check_notDefine();
  }
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
  struct TblBlock* tmpTbl=block.tbls_head.next;
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


int output_grammar(){
  //���Ƚ��з���
  gc();
  //��������ȫ�ֵ�not_define����
  if(block.not_define==NULL) fprintf(fout,"#define %s -1\n\n",DEFAULT_NOTDEFINE_STRING);
  else fprintf(fout ,"#define %s -1\n\n",block.not_define);

  //��ȡsymbol��token������
  int symbolsNum=block.symbols.num;
  int tokensNum=block.tokens.num;
  //Ȼ������ĳ��ids���鵽0-n��ӳ�䣬TODO
  int* symbolIds=hashset_toArr(&block.symbols);
  int* symbols=get_hashArr_fromZero(symbolIds,symbolsNum,NULL);
  output_grammar_genEnum("Symbol",symbolIds,symbolsNum);
  int* tokenIds=hashset_toArr(&block.tokens);
  int* tokens=get_hashArr_fromZero(tokenIds,tokensNum,NULL);
  output_grammar_genEnum("Token",tokenIds,tokensNum);
  //����������actionkind��enum
  struct TblBlock* tbl=block.tbls_head.next;
  while (tbl!=NULL)
  {
    char* actionkind=getIdString(&block.idAllocator,tbl->actionKind);
    int* ids=hashset_toArr(&tbl->actions);
    output_grammar_genEnum(actionkind,ids,tbl->actions.num);
    free(ids);
    fprintf(fout,"int %s_Tbl[Symbols_NUM][Tokens_NUM]={\n",actionkind);
    free(actionkind);
    //����ÿһ�еı������,��Ȼ����Ҫ׼�������
    int* table=malloc(sizeof(int)*symbolsNum*tokensNum);
    //Ȼ��ȫ�����ݳ�ʼ��Ϊ-1
    memset(table,-1,symbolsNum*tokensNum*sizeof(int));
    //Ȼ�����syntax��д����
    struct syntax_line* sl=tbl->syntaxs_head.next;
    while(sl!=NULL){
      int x=symbols[sl->symbol];
      int y=tokens[sl->token];
      *(table+x*symbolsNum+y)=sl->action;
      sl=sl->next;
    }
    //Ȼ��ʼ��ӡ
    for(int i=0;i<symbolsNum;i++){
      char* name=getIdString(&block.idAllocator,symbolIds[i]);
      int x=symbols[symbolIds[i]];
      //��¼��ǰ��symbol��token�Ĺ�ϵ
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
        name=getIdString(&block.idAllocator,tokenIds[i]);    //��¼���ʱ���token������
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

  //�����ж��Ƿ���notDefine��Ӧ���ַ���
  if(block.not_define!=NULL&&strcmp(block.not_define,tmp)==0){
    free(block.not_define);
    block.not_define=NULL;
    return 1;
  }
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
    struct TblBlock *cur = block.tbls_head.next;
    struct TblBlock *pre = &block.tbls_head;
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
    struct TblBlock *cur = block.tbls_head.next;
    struct TblBlock *pre = &block.tbls_head;
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
  //Ҫ�滻�ɵ����ַ������ܹ����Ѿ�ʹ�õ��ַ���
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
  //���symbol�ǵ���*,��ʾ�ܹ�ƥ�����е�symbol,������е�symbol�����޸�
  if(strcmp(tmp,"*")==0){
    end=mysgets(tmp,stops,toAdd);
    if(end!=',') return 0;
    toAdd+=strlen(tmp)+1;
    char action[200];
    end=mysgets(action,stops,toAdd);
    if(strToId(block.strIds,action)<0) return 0;
    int* symbols=hashset_toArr(&block.symbols);
    if(strcmp(tmp,"*")==0){
      //������symbol,token������ת������Ϊ���action
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
  if(action<0) return 0;  //���actionδע��,����
  // //�����action�����������,������,��Ϊջ������صı���Ҫsymbol��Ϊ��Ԫ��
  // if(!hashset_contains(&tmpTbl->actions,&action)) return 0;
  //������������,�����Ǵӱ�ͷ���һλ,ѹ��ջ��,Խ����������Խ������ͷ
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

int gtg_showSyntaxs(struct TblBlock* tmpTbl,char* actionkind)
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
struct TblBlock* getTbl(char* actionkind){
  int id=strToId(block.strIds,actionkind);
  if(id<0) return NULL;
  struct TblBlock* kindof=NULL;
  kindof=block.tbls_head.next;
  while (kindof!=NULL&&kindof->actionKind!=id) kindof=kindof->next;
  return kindof;
}