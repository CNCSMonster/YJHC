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
    hashset_del(&tmp->actions);
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
  //����������Ч����ǰ�Ŀո��Լ�����
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
    printf("fail to execute such line ");
  }
  if(fout!=stdout) fclose(fout);
  if(ord!=NULL)  free(ord);
  ord=NULL;
  return 1;
}

int isUsedStr(char* str){
  //���Ȳ����ַ���id���в��Ҹ��ַ����ı��,������ҵ���˵���Ѿ�ʹ�ù���
  int jud=strToId(block.strIds,str);
  if(jud<0) return 0;
  return 1;
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
  //�ж��Ƿ��ǲ鿴����
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
  //�ж��Ƿ����������
  else if(strcmp(tmp,"output")==0){
    end=mysgets(tmp,stops,ord+strlen(tmp)+1);
    if(strcmp(tmp,"-o")==0) return OUTPUT_ORDERS;
    else if(strcmp(tmp,"-g")==0) return OUTPUT_GRAMMAR;
  }

  //������δ��������
  return NOT_DEFINE;
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

int actionkind_setdefault(){return error();
  
}

int set_not_define(){return error();

}

int action_add(){
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
  end=mysgets(tmp," ",track);
  track+=strlen(tmp)+1;
  //�ж�actionkind�Ƿ����
  int id;
  int ackindId;
  //����ַ�����������û������ַ���,��������ַ�����Ӧ��idû�б�����actionkind������
  if((id=strToId(block.strIds,tmp))<0||!hashset_contains(&block.actionKinds,&id)){
    printf("your input is not correct id!");
    return error();
  }
  ackindId=id;    //����actionkind��id,����ʹ��

  //���û�и���-n����,���ȡ��һ������������Ϊ����
  if(n_gtg==0){
    free(ord);
    ord=fgetOrd(fin);
    //Ȼ���ord���������ݼ��뵽��actionkind��action��
    track=ord;
    int islast=1;
    //TODO
    while ((end = mysgets(tmp, " ", track)) != '\0'||islast)
    {
      track+=strlen(tmp)+1;
      if(end=='\0') islast=0;
      if (strlen(tmp) == 0)
        continue;
      //���ж����������û���ù�
      if (isUsedStr(tmp))
      {
        fprintf(fout,"%s has been input!\n");
        continue;
      }
      id = allocateId(&block.idAllocator, tmp);
      putStrId(block.strIds, tmp, id);
      // TODO,�ҵ���Ӧactionkind�Ŀ�,���������
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
      //����actions���м�������
      hashset_add(&(tmpAEK->actions), &id);
    }
  }
  //�����ȡ������n����Ч�ַ�����Ϊ��action������
  else{
    for(int i=0;i<n_gtg;i++){
      while((end=myfgets(tmp," \n#",fin))!=EOF){
        if(end=='#'){
          end=myfgets(tmp,"\n",fin);
          if(end==EOF) break;
          else continue;
        }
        else if(strlen(tmp)==0) continue;
        //���ж����������û���ù�
        if(isUsedStr(tmp)){
          fprintf(fout,"%s has been input!\n");
          fclose(fout);
          return 0;
        }
        id=allocateId(&block.idAllocator,tmp);
        putStrId(block.strIds,tmp,id);
        //TODO,�ҵ���Ӧactionkind�Ŀ�,���������
        struct tblBlock* tmpAEK=block.actions_head.next;
        while(tmpAEK!=NULL&&tmpAEK->actionKind!=ackindId){
          tmpAEK=tmpAEK->next;
        }
        if(tmpAEK==NULL){
          fprintf(fout,"actions block for such actionkind doesn't exist!\n");
          fclose(fout);
          return 0;
        }
        //����actions���м�������
        hashset_add(&(tmpAEK->actions),&id);
        break;
      }
    }
  }
  return 1;
}

int syntax_add(){
  //TODO
  //���û��-n����
  if(n_gtg==0){

  }
  //�����-n����,���ȡ������n����Ϣ��Ϊ����
  else{

  }

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
        fprintf(fout,"\nerror!you can't use empty string as symbol!\n",tmp);
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
        fprintf(fout,"\nerror!you can't use empty string as token!\n",tmp);
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
        fprintf(fout,"\nerror!you can't use empty string as symbol!\n",tmp);
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
        fprintf(fout,"\nerror!you can't use empty string as symbol!\n",tmp);
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
        addTbl->defaultAction=-1; //������ʾʹ��Ĭ�ϵ�id
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
        fprintf(fout,"\nerror!you can't use empty string as symbol!\n",tmp);
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
int del(){return error();

}

int replace(){return error();

}

int gtg_exit(){

  //�Ƚ��пռ�����ͻ���
  
  //������������д���ļ�
  fout=fopen(DEFAULT_GTG_WORK_FILE,"w");

  if(fout==NULL) return 0;
  if(ord!=NULL) free(ord);
  ord=NULL; 
  //TODO�������ݵ��ļ���
  //�ȱ���symbols
  if(!check_symbol()) return 0;
  //�ٱ���tokens
  if(!check_token()) return 0;

  if(!check_actionkind()) return 0;
  //�ٱ���actions��syntax,����ÿ��actionkind����

  fclose(fout);

  //�ͷſռ�
  //���symbol���ͷſռ�
  hashset_del(&block.symbols);
  //���token���ͷ�
  hashset_del(&block.tokens);
  //���actionkind���ͷſռ�
  hashset_del(&block.actionKinds);
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
  hashset_del(&block.symbols);
  //���token���ͷ�
  hashset_del(&block.tokens);
  //���actionkind���ͷſռ�
  hashset_del(&block.actionKinds);
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
  //ִ���ļ�,��-o�����Ի�ȡ֮��ȥ��-o,Ȼ��ִ�и���check


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