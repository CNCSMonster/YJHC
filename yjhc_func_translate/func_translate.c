#include "func_translate.h"



//��������������,�������ǰ�δ�����yjhc�ĺ�������tokenתΪc�ĺ�������token����
FuncTranslator getFuncTranslator(char* typePath,char* funcHeadPath,char* valPath){
  FuncTranslator out={
    .funcTbl=NULL,
    .gloabalTypeTbl=NULL,
    .partialValTbl=NULL,
    .globalValTbl=NULL,
    .curFunc=NULL,
    .judSentenceKindErr=0,
    .warningFout=stdout
  };
  if(typePath==NULL||funcHeadPath==NULL||valPath==NULL) return out;
  int isRight=1;  //��¼����ȷ��
  FILE* typeFin=fopen(typePath,"r");
  FILE* funcFin=fopen(funcHeadPath,"r");
  FILE* valFin=fopen(valPath,"r");
  if(typeFin==NULL||funcFin==NULL||valFin==NULL){
    isRight=0;
  }
  if(!isRight){
    if(typeFin!=NULL) fclose(typeFin);
    if(funcFin!=NULL) fclose(funcFin);
    if(valFin!=NULL) fclose(valFin);
    return out;
  }
  out.gloabalTypeTbl=malloc(sizeof(TypeTbl));
  *(out.gloabalTypeTbl)=getTypeTbl();
  loadFile_typeTbl(out.gloabalTypeTbl,typeFin);

  out.funcTbl=malloc(sizeof(FuncTbl));
  *(out.funcTbl)=getFuncTbl(out.gloabalTypeTbl);
  loadFile_functbl(out.funcTbl,funcFin);

  out.globalValTbl=malloc(sizeof(ValTbl));
  *(out.globalValTbl)=getValTbl(*(out.gloabalTypeTbl));
  loadFile_valtbl(out.globalValTbl,valFin);
  out.partialValTbl=out.globalValTbl;
  //��ɿռ������ͷſռ�
  fclose(typeFin);
  fclose(valFin);
  fclose(funcFin);
  return out;
}

//����funcTranslator�����·��,���óɹ�����1,����ʧ�ܷ���0
int functranslator_setFout(FuncTranslator* translator,char* warningOutPath){
  if(warningOutPath==NULL) return 0;
  FILE* newFout=fopen(warningOutPath,"w");
  if(newFout==NULL) return 0;
  translator->warningFout=newFout;
  return 1;
}


//���з���ǰ���,���纯���Ƿ�ȫ��ʵ��
int pre_translate_check(FuncTranslator* translator){
  //����ǰ���
  //���1,���ṹ�巽���Ƿ�ȫ��ʵ��
  //����,ȡ�����нṹ�巽����һ��������
  StrSet strset=getStrSet(myStrHash);
  int isRight=1;  //��ʶ
  //�Ȱ����к�������strset
  for(int i=0;i<translator->funcTbl->funcKeys.size&&isRight;i++){
    //���ݺ�������ȡ��������ʶ��
    char* funcKey;
    vector_get(&translator->funcTbl->funcKeys,i,&funcKey);
    //�����ʶ��
    if(!addStr_StrSet(&strset,funcKey)){
      //��ȡ���˺�typeid
      long long typeId;
      char funcName[200];
      extractFuncNameAndOwnerFromKey(funcKey,funcName,&typeId);
      if(typeId==0)
        fprintf(translator->warningFout,"redefinition of function %s\n",funcName);
      else{
        Type type;
        int typeIndex;
        extractTypeIndexAndPointerLayer(typeId,&typeIndex,NULL);
        vector_get(&translator->gloabalTypeTbl->types,typeIndex,&type);
        fprintf(translator->warningFout,"redefinition of member function %s of %s type\n",funcName,type.defaultName);
      }
      isRight=0;
    }
  }
  if(!isRight){
    initStrSet(&strset);
    return 0;
  }
  //Ȼ����нṹ��ķ�������.�ж��Ƿ���δʵ�ֵ�
  for(int i=2;i<translator->gloabalTypeTbl->types.size&&isRight;i++){
    Type type;
    vector_get(&translator->gloabalTypeTbl->types,i,&type);
    if(type.kind!=TYPE_UNION&&type.kind!=TYPE_STRUCT) continue;
    //��ȡtypeId
    long long typeId=getTypeId(i,0);
    char** funcsArr=toStrArr_StrSet(&type.funcs);
    for(int j=0;j<type.funcs.num&&isRight;j++){
        char* tfk=getFuncKey(funcsArr[j],typeId);
        if(!delStr_StrSet(&strset,tfk)){
          //��ӡû��ʵ�ֵĺ���
          fprintf(translator->warningFout,"member func %s of %s type not find definition\n",funcsArr[j],type.defaultName);
          isRight=0;
        }
        free(tfk);
    }
    free(funcsArr);
  }
  if(!isRight){
    initStrSet(&strset);
    return 0;
  }
  //�ҵ�û����������ʵ���˵ĺ���,Ҳ����strset��ʣ�µĺ���
  char** keys=toStrArr_StrSet(&strset);
  for(int i=0;i<strset.num;i++){
    //ȥ�жϸú����Ƿ���ȫ�ֺ���,�����ȫ�ֵĺ���,�򲻽��м��
    char funcName[300];
    long long retOwnerId;
    extractFuncNameAndOwnerFromKey(keys[i],funcName,&retOwnerId);
    //����retOwnerId�ж��Ƿ���ȫ�ֺ���
    free(keys[i]);
    if(retOwnerId==0){
      continue;
    }
    //��ȡ��Ӧ���͵�����
    int index;
    extractTypeIndexAndPointerLayer(retOwnerId,&index,NULL);
    Type type;
    vector_get(&translator->gloabalTypeTbl->types,index,&type);
    if(type.kind!=TYPE_STRUCT&&type.kind!=TYPE_UNION) continue;
    fprintf(translator->warningFout,"definition of member function %s but without declaration in \"%s\"'s definition\n",funcName,type.defaultName);
  }
  free(keys);
  initStrSet(&strset);
  if(!isRight) return 0;
  return 1;
}



//�����Ƿ������������,�����������,���ط�0ֵ,���û�У�����0
int check_main_function(FuncTranslator* translator){
  //�ں������в�������ΪNULL,������Ϊmain�ĺ���
  char* funcKey=getFuncKey("main",0);
  int out=0;
  if(hashtbl_get(&translator->funcTbl->funcs,&funcKey,NULL)){
    out=1;
  }
  free(funcKey);
  return out;
}


//ʹ�ú�����������ʼ����
int func_translate(FuncTranslator* funcTranslator,char* tokenInPath,char* tokenOutPath){
  if(tokenInPath==NULL || tokenOutPath==NULL ) return 0;
  //���Ƚ���ѡ���Լ��
  //�������������
  if(!check_main_function(funcTranslator)){
    printf("main function miss!\n");
    return 0;
  }
  //���к��������Լ��
  if(!pre_translate_check(funcTranslator)){
    return 0;
  }
  //��ʼ�������������
  FILE* fin=fopen(tokenInPath,"r");
  FILE* fout=fopen(tokenOutPath,"w");
  int isRight=1;
  if(fin==NULL||fout==NULL) isRight=0;
  if(!isRight){
    if(!fin) fclose(fin);
    if(!fout) fclose(fout);
    return 0;
  }
  init_token_reader(fin);
  //��ʽ����
  TBNode* nodes;
  ActionSet actionSet;
  int preBlocks=0;
  //��ȫ�����з���,Ȼ���ʽ���Ĺ������������,
  //����Ľ��Ӧ����token����
  int funcIndex=0;  //��¼���뵽�ĺ������±�
  while((nodes=readTokenSentence(&actionSet))!=NULL){
    //�Ƚ��д���
    //���з���������ȱʧ���Ȳ���
    nodes=member_use_complement(funcTranslator,nodes);

    // ��һ�����Ӳ��亯��
    nodes=process_singleLine(funcTranslator,nodes);

    //Ȼ��Ѵ�����д���ļ�
    if(nodes!=NULL) 
      fput_tokenLine(fout,nodes);

    //debug�õĴ�ӡ���
    // if(nodes!=NULL) fshow_tokenLine(stdout,nodes);

    //���п�Ľ�������
    if(preBlocks==actionSet.blocks-1){
      //�ж��Ƿ�Ҫ���غ���
      if(preBlocks==0){ //���غ���
        //���Ҷ�Ӧ����
        char* funcKey;
        vector_get(&funcTranslator->funcTbl->funcKeys,funcIndex,&funcKey);
        funcIndex++;
        //�������ֲ��Һ���
        hashtbl_get(&funcTranslator->funcTbl->funcs,&funcKey,&funcTranslator->curFunc);
        //TODO���밲ȫ���
        if(funcTranslator->curFunc==NULL){
          isRight=0;
          break;
        }
      }
      //�����µľֲ�����
      funcTranslator->partialValTbl=extendValTbl(funcTranslator->partialValTbl);
      //���뺯���Ĳ������ֲ�������
      loadArgs_valtbl(funcTranslator->partialValTbl,funcTranslator->funcTbl,funcTranslator->curFunc);
    }
    else if(preBlocks==actionSet.blocks+1){
      //�жϺ����Ƿ��˳�
      if(preBlocks==1){
        funcTranslator->curFunc=NULL;
      }
      funcTranslator->partialValTbl=recycleValTbl(funcTranslator->partialValTbl);
    }
    preBlocks=actionSet.blocks;
    del_tokenLine(nodes);
    nodes=NULL;
  }
  if(nodes!=NULL) del_tokenLine(nodes);
  release_token_reader();
  fclose(fin);
  fclose(fout);
  if(!isRight) return 0;
  return 1;
}


//��ȫ��Ա�������������������Եĵ���
TBNode* member_use_complement(FuncTranslator* funcTranslator,TBNode* nodes){
  //������ǳ�Ա�������ò�ȫ����
  if(funcTranslator->curFunc==NULL) return nodes;
  //������û�г�Ա����
  TBNode* track=nodes;
  TBNode* pre=NULL;
  TBNode* prePre=NULL;
  TBNode head;
  head.next=nodes;
  nodes->last=&head;
  // ȡ����������
  char* ownerName=funcTranslator->curFunc->owner;
  if(ownerName==NULL) return nodes; //�������û��������,����������ú���
  //ȡ���������ˣ��ҵ���Ӧ����
  int typeIndex=findType(funcTranslator->gloabalTypeTbl,ownerName,NULL);
  Type type;
  if(typeIndex!=0){
    vector_get(&funcTranslator->gloabalTypeTbl->types,typeIndex,&type);
  }
  while (track!=NULL)
  {
    int ifToComplete=0; //�ж��Ƿ�ȥ��ȫ
    //���func�ǱȽϵ�����,����ƥ���ж��Ƿ���Ҫǰ�����self.
    //���һ��track�Ȳ���funcҲ����var������Ҫ���г�Ա���ò�ȫ
    if(track->token.kind!=FUNC&&track->token.kind!=VAR){
      ifToComplete=0;
    }else if(pre!=NULL&&prePre!=NULL&&strcmp(pre->token.val,".")==0&&prePre->token.kind==SELF_KEYWORD){
      ifToComplete=0;
    }else if(track->token.kind==VAR&&hashtbl_get(&type.fields,&(track->token.val),NULL)){
      ifToComplete=1;
    }else if(track->token.kind==VAR){
      ifToComplete=0;
    }else if(containsStr_StrSet(&type.funcPointerFields,track->token.val)){
      ifToComplete=1;
    }else if(containsStr_StrSet(&type.funcs,track->token.val)){
      ifToComplete=1;
    }
    //����ж�Ҫ��ȫ������в�ȫ����
    if(ifToComplete){
      // ����в�ȫ
      TBNode *tmpPre = track->last;
      // ����һ��..���͵Ŀ�ָ��
      TBNode *newSelf;  // ������ؼ��ֽڵ�
      TBNode *newVisit; // �·��ʷ���
      // ��ȡһ���µ�tokenNode
      newSelf = getTBNode(SELF_STRING_VALUE, SELF_KEYWORD);
      newVisit = getTBNode(".", OP);
      newSelf->next = newVisit;
      newVisit->last = newSelf;
      tmpPre->next = newSelf;
      newSelf->last = tmpPre;
      track->last = newVisit;
      newVisit->next = track;
    }
    track=track->next;
    if(track!=NULL) pre=track->last;
    if(pre!=NULL) prePre=pre->last;
  }
  nodes=head.next;
  nodes->last=NULL;
  return nodes;
}



//��nodeds���д���,�ɹ����ط�0ֵ,ʧ�ܷ���0
TBNode* process_singleLine(FuncTranslator* funcTranslator,TBNode* nodes){
  if(nodes==NULL) return NULL;
  //�����ж�����
  FTK kind=getTokenLineKind(funcTranslator,nodes);
  if(kind==NOT_LEAGAL_FTK){
    del_tokenLine(nodes);
    return NULL;
  }
  if(tranFuncs[kind]!=NULL)
    nodes=tranFuncs[kind](funcTranslator,nodes);
  return nodes;
}


//��������ͷź���������
int release_funcTranslator(FuncTranslator* funcTranslator){
  //ɾ�������Լ���Ƕ�����ͱ�
  while(funcTranslator->globalValTbl!=NULL){
    ValTbl* tmpTbl=funcTranslator->globalValTbl;
    funcTranslator->globalValTbl=tmpTbl->next;
    del_valTbl(tmpTbl);
    free(tmpTbl);
  }
  del_functbl(funcTranslator->funcTbl); //ɾ��������
  free(funcTranslator->funcTbl);
  free(funcTranslator->gloabalTypeTbl);
  funcTranslator->funcTbl=NULL;
  funcTranslator->gloabalTypeTbl=NULL;
  funcTranslator->partialValTbl=NULL;
  funcTranslator->curFunc=NULL;
  if(funcTranslator->warningFout!=stdout){
    fclose(funcTranslator->warningFout);
    funcTranslator->warningFout=stdout;
  }
  return 1;
}



//�жϾ��ӵķ�������,��ѡ��ͬ�ķ������
FTK getTokenLineKind(FuncTranslator* funcTranslator,TBNode* nodes){
  if(nodes==NULL) return NOT_TRANSLATE_FTK;
  if(funcTranslator->curFunc==NULL) return NOT_TRANSLATE_FTK;
  
  //�ж��Ƿ��ǲ��ý��з������÷���ľ���
  if(ifNotNeedFuncTranslate(funcTranslator,nodes)){
    return NOT_TRANSLATE_FTK;
  }
  //����������ж��쳣
  if(funcTranslator->judSentenceKindErr){
    //�����쳣,�����ض��ж�:���Ӳ����﷨
    funcTranslator->judSentenceKindErr=0;
    return NOT_LEAGAL_FTK;
  }
  

  //�ж��Ƿ���typedef���
  if(nodes->token.kind==TYPEDEF_KEYWORD){
    return TYPEDEF_FTK;
  }

  //�ж��Ƿ�������ǿת��䣬TODO
  if(isTypeChangeSentence(funcTranslator,nodes)){
    return TYPE_CHANGE_FTK;
  }
  //����������ж��쳣
  if(funcTranslator->judSentenceKindErr){
    //�����쳣,�����ض��ж�:���Ӳ����﷨
    funcTranslator->judSentenceKindErr=0;
    return NOT_LEAGAL_FTK;
  }

 
  //�ж��Ƿ��Ǳ����������,����ı�������ָ��������
  if(nodes->token.kind==TYPE&&nodes->next!=NULL&&nodes->next->token.kind==VAR){
    return VAR_DEFINE_FTK;
  }
  
  //�ж��Ƿ��ǳ����������
  if(nodes->token.kind==CONST_KEYWORD){
    if(nodes->next!=NULL&&nodes->next->token.kind==TYPE) {
      return CONST_DEFINE_FTK;
    }
    else{
      return NOT_LEAGAL_FTK;
    }
  }
  
  //�ж��Ƿ��Ǻ����������
  if(nodes->token.kind==FUNC){
    return FUNC_USE_FTK;
  }

  //�ж��Ƿ��ǳ�Ա���Ե���
  if(nodes->token.kind==VAR&&nodes->next!=NULL&&
    strcmp(nodes->next->token.val,".")==0&&
    nodes->next->next!=NULL
    &&nodes->next->next->token.kind==VAR
  ){
    return MEMBER_FIELD_USE_FTK;
  }
  //�ж��Ƿ��ǳ�Ա��������
  if(nodes->token.kind==VAR&&
    nodes->next!=NULL
    &&strcmp(nodes->next->token.val,".")==0
    &&nodes->next->next!=NULL
    &&nodes->next->next->token.kind==FUNC
  ){
    return MEMBER_FUNCTION_USE_FTK;
  }

  if(isArrVisitSentence(funcTranslator,nodes)){
    return ARR_VISIT_FTK;
  }
  //����������ж��쳣
  if(funcTranslator->judSentenceKindErr){
    //�����쳣,�����ض��ж�:���Ӳ����﷨
    funcTranslator->judSentenceKindErr=0;
    return NOT_LEAGAL_FTK;
  }

  //�ж��Ƿ��Ǻ���ָ�붨�����,����Ǻ���ָ�붨�����,����к���ָ�붨�����Ĵ���
  if(isFuncPointerDefSentence(funcTranslator,nodes)){
    return FUNCPOINTER_DEF_FTK;
  }
  //����������ж��쳣
  if(funcTranslator->judSentenceKindErr){
    //�����쳣,�����ض��ж�:���Ӳ����﷨
    funcTranslator->judSentenceKindErr=0;
    return NOT_LEAGAL_FTK;
  }

  //�ж��Ƿ��Ǹ�ֵ���,Ҫ��������ƥ����
  if(isAssignSentence(funcTranslator,nodes)){
    return ASSIGN_FTK;
  }
  if(funcTranslator->judSentenceKindErr){
    //�����쳣,�����ض��ж�:���Ӳ����﷨
    funcTranslator->judSentenceKindErr=0;
    return NOT_LEAGAL_FTK;
  }

  //�����Ǹ����������,��������зֽ�
  return COUNT_FTK;
}


//�ж��Ƿ��ǲ���Ҫ�������÷���ľ���
int ifNotNeedFuncTranslate(FuncTranslator* translator,TBNode* nodes){
  if(nodes==NULL) return 1;
  //���һ����������ȫ�����ǽ��,����������ǲ��÷����
  //���Ƚ���һ�α���,ȡ�����з��ŵ�����
  //Ȼ��ʹ��λ�������жϸ÷����Ƿ���ȷ
  //����˵0000_0000�˸�λ�ñ�ʾ���ַ���
  //�����ǵľ��ӵķ�������ֲ��� 1100_0000��1��ʾ��
  //Ҫ�ж��Ƿ�ȫ�����Ŷ���0001_1111��
  //��ȡ������1110_0000,Ȼ���ú�����ǰ����������
  //��������Ϊ0,��˵��Ŀ����Ӻ�����������,����û��

  //���ֻ��������ʽ�͵ķ���,���ǲ���Ҫ�������÷���ľ���
  BitMapUtil bmu={
    .mapSize=sizeof(long long)
  };
  BitMap bm=getBitMap(&bmu);
  TBNode* track=nodes;
  while(track!=NULL){
    put_bitmap(&bmu,&bm,track->token.kind);
    track=track->next;
  }
  long long mode=COUNT_BITMAP|CONTROL_KEYWORD_BITMAP|SEP_BITMAP;
  long long cur;
  memcpy(&cur,bm.map,sizeof(cur));
  delBitMap(&bm);
  //���nodes�к���mode�����token����,���ǲ���Ҫ����yjhc->c����ľ���
  if(cur&(~mode)){
    return 0;
  }
  return 1;
}


//�ж��Ӻ���
//�ж��Ƿ��Ǻ���ָ�붨�����
int isFuncPointerDefSentence(FuncTranslator* funcTranslator,TBNode* nodes){
  //����Ǻ���ָ�붨�����,���ʽ�� type ( * fpName) (type1,type2,...)
  int state=0;
  while(nodes!=NULL){
    if(state==0){
      if(nodes->token.kind!=TYPE) return 0;
      else state=1;
    }
    else if(state==1){
      if(nodes->token.kind!=LEFT_PAR) return 0;
      else state=2;
    }
    else if(state==2){
      if(strcmp(nodes->token.val,"*")!=0) return 0;
      else state=3;
    }
    else if(state==3){
      if(nodes->token.kind!=RIGHT_PAR) return 0;
      else state=4;
    }
    else if(state==4){
      if(nodes->token.kind!=LEFT_PAR) return 0;
      else state=5;
    }
    else if(state==5){
      if(nodes->token.kind==RIGHT_PAR){
        state=6;  //6���Ϊ�ս�״̬
      }
      else if(nodes->token.kind==TYPE){
        state=7;
      }
      else return 0;
    }
    else if(state=6){
      //6���ս�״̬�����˺���ָ�붨�����Ľ�β������ʲôtoken���Ǵ���
      return 0;
    }
    //״̬7Ϊ���������б��в������ͺ�״̬���ڴ����Ż�������
    else if(state==7){
      if(nodes->token.kind==COMMA) state=8;
      else if(nodes->token.kind==RIGHT_PAR) state=6;
      else return 0;
    }
    //״̬8Ϊ���ź�״̬
    else if(state==8){
      if(nodes->token.kind==TYPE) state=7;
      else return 0;
    }
    else{
      //����״̬��˵�����쳣״̬��ͣ���������쳣ֵ
      return 0;
    }
    nodes=nodes->next;
  }
  if(state==6) return 1;
  return 0;
}


//�ж��Ƿ��Ǹ�ֵ���,����Ǹ�ֵ���,��֧��������ֵ���
int isAssignSentence(FuncTranslator* funcTranslator,TBNode* nodes){
  //׼������
  // ��ʽΪ:var=val,var2=val,...var3=val
  //׼�����Զ������ж��Ƿ��Ǹ�ֵ���
  int state=0;
  const int fail_state=INT_MAX;
  const int accept_state=INT_MAX-1;
  int toNum=0;  //ͳ��Ŀ�ı���������
  while(nodes!=NULL&&state!=fail_state){
    //��ʼ״̬������������VAR
    if(state==0&&nodes->token.kind==VAR){
      state=1;
    } 
    //����var֮�����״̬1,��������,����=
    //����','����״̬0,�ڴ�������һ������
    else if(state==1&&nodes->token.kind==COMMA){
      state=0;
      toNum++;
    }
    //����=�Ž���״̬2,
    else if(state==1&&strcmp(nodes->token.val,"=")==0){
      state=accept_state;
    }
    //�����ȡֵ״̬
    else if(state==accept_state){
      //��ȡ������Ϊֹ
      int count=0;
      if(nodes->token.kind==COMMA){
        state=fail_state;
        break;
      }
      toNum--;
      do{
        TokenKind kind=nodes->token.kind;
        if(kind==RIGHT_PAR||kind==RIGHT_BRACE||kind==RIGHT_BRACKET) count--;
        else if(kind==LEFT_PAR||kind==LEFT_BRACE||kind==LEFT_BRACKET) count++;
        else if(kind==COMMA&&count==0) break;
        nodes=nodes->next;
      }while(nodes!=NULL);
      if(nodes==NULL) break;
    }
    else{
      funcTranslator->judSentenceKindErr=1;
      return 0;
    }
    nodes=nodes->next;
  }
  //������쳣���ͣ��
  if(state!=accept_state||toNum!=0){
    funcTranslator->judSentenceKindErr=1;
    return 0;
  }
  return 1;
}

//�ж��Ƿ��������Ա����
int isArrVisitSentence(FuncTranslator* funcTranslator,TBNode* nodes){
  if(nodes->token.kind!=VAR) return 0;
  nodes=nodes->next;
  if(nodes->token.kind!=LEFT_BRACKET) return 0;
  nodes=nodes->next;
  //��ȡ�м�����
  int leftBra=1;
  while(nodes!=NULL){
    if(nodes->token.kind==LEFT_PAR||nodes->token.kind==LEFT_BRACKET||nodes->token.kind==LEFT_BRACE)
    {
      leftBra++;
    }
    else if(nodes->token.kind==RIGHT_PAR||nodes->token.kind==RIGHT_BRACE||nodes->token.kind==RIGHT_BRACKET)
    {
      leftBra--;
    }
    if(leftBra==0) break;
    nodes=nodes->next;
  }
  if(leftBra!=0){
    funcTranslator->judSentenceKindErr=1;
    return 0;
  }
  return 1;
}

//�ж��Ƿ�������ǿת���
int isTypeChangeSentence(FuncTranslator* funcTranslator,TBNode* nodes){
  if(nodes==NULL) return 0;
  if(nodes->token.kind!=LEFT_PAR) return 0;
  nodes=nodes->next;
  if(nodes==NULL||nodes->token.kind!=TYPE) return 0;
  nodes=nodes->next;
  if(nodes==NULL||nodes->token.kind!=RIGHT_PAR) return 0;
  return 1;
}


//���빦���Ӵ���,����ɹ����ط�NULL,����ʧ�ܷ���NULL


//�����Ա���Է������
TBNode* translateMemberFieldVisit(FuncTranslator* funcTranslator,TBNode* tokens){
  //��Ҫ��֤�������������tokensһ���Ƕ�Ӧ���͵�
  //���ȵ�һ����һ����VAR,���������쳣���
  //���Ȼ�ȡ����������
  Type type;
  Val val;
  int retLayer;
  char* newTypeName=NULL;  //�±�����������
  //TODO �����û���ҵ����������������Ϊ��c��׼���ﶨ��Ľṹ����,��Ϊ��û�м���c��Ķ���
  if(!findVal(funcTranslator->partialValTbl,tokens->token.val,&val,&type,&retLayer)){
    //TODO �쳣���
    del_tokenLine(tokens);
    return NULL;
  }

  //��������ҵ�������Ϊ�ṹ�����ͻ���ö�����͵Ļ�
  if((type.kind==TYPE_STRUCT||type.kind==TYPE_UNION)
    &&(
      (retLayer==0&&strcmp(tokens->next->token.val,".")==0)
      ||
      (retLayer==1&&strcmp(tokens->next->token.val,"->")==0)
    )
  ){
    
    //��ȡ������������,���������û�и����Կɷ���,���б�����ʾ
    if(!hashtbl_get(&type.fields,&(tokens->next->next->token.val),&newTypeName)){
      fprintf(funcTranslator->warningFout,"visit unexist field %s of %s\n",tokens->next->next->token.val,tokens->token.val); 
      del_tokenLine(tokens);
      return NULL;
    }
  }
  //������������ͣ�����δ֪���ͣ�������Ǳ�׼���еĳ�Ա����;�ֻ��߽����˴���ָ���εķ���
  else{
    //��ʱ�����쳣����
    del_tokenLine(tokens);
    return NULL;
  }

  TBNode *tail = tokens->next->next;
  TBNode *sufTail = tail->next;
  tail->next = NULL;
  if (sufTail != NULL)
    sufTail->last = NULL;
  // �ϲ����ʽ
  TBNode *out = connect_tokens(tokens, VAR, "");
  // ����ϲ��쳣
  if (out == NULL)
  {
    del_tokenLine(sufTail);
    return NULL;
  }
  // ���ϲ�����Ƿ��ǹ����ڵ�
  if (out->next != NULL || out->last != NULL)
  {
    del_tokenLine(out);
    return NULL;
  }
  // ����ϲ��ɹ�,
  
  //�������Ӻϳɱ��ʽ�ͺ���ʣ��ڵ�
  out->next=sufTail;
  sufTail->last=out;
  //Ȼ��ע����ʽ�ַ�ֵΪĿ������
  addVal_valtbl(funcTranslator->partialValTbl,out->token.val,NULL,0,newTypeName,0);

  return process_singleLine(funcTranslator ,out);
}

//����Ԫ�ط������
TBNode* translateArrVisit(FuncTranslator* funcTranslator,TBNode* tokens){
  //���Ƚ�ȡ��������������֮�������
  TBNode* head=tokens->next->next;
  TBNode* preHead=head->last;
  TBNode* sufTail=head;
  int leftP=1;
  while(sufTail!=NULL){
    TokenKind kind=sufTail->token.kind;
    if(kind==LEFT_BRACE||kind==LEFT_PAR||kind==LEFT_BRACKET){
      leftP++;
    }
    else if(kind==RIGHT_BRACE||kind==RIGHT_BRACKET||kind==RIGHT_PAR)
    {
      leftP--;
    }
    if(leftP==0) break;
    sufTail=sufTail->next;
  }
  if(sufTail==NULL){
    del_tokenLine(tokens);
    return NULL;
  }
  //��������±���ʽΪ��,Ҳ���Ƿ�������Ϊ������,Ҳ���ǷǷ����
  else if(sufTail==head){
    del_tokenLine(tokens);
    return NULL;
  }
  //Ȼ�����±���ʽ,��ʵ�±���ʽӦ����һ�����Ϊ���ͱ���������
  
  //���ȷ�����±���ʽ
  head->last=NULL;
  preHead->next=NULL;
  sufTail->last->next=NULL;
  sufTail->last=NULL;


  //Ȼ�����±���ʽ
  head=process_singleLine(funcTranslator,head);
  //����±���ʽ��������
  if(head==NULL){
    del_tokenLine(tokens);
    del_tokenLine(sufTail);
    return NULL;
  }
  //�����ж��±���ʽ������
  Type type;
  Val val;
  int layer;
  //�ӱ���������ȡ�±���ʽ����
  findVal(funcTranslator->partialValTbl,head->token.val,&val,&type,&layer);
  //�жϱ��ʽ��������
  //����±겻��int���ͻ��߲���long long����,��Ϊ������±���ʽ,�����쳣ֵ
  if(layer!=0||type.kind!=TYPE_INT){
    del_tokenLine(tokens);
    del_tokenLine(sufTail);
    del_tokenLine(head);
    return NULL;
  }
  //��������ȷ���͵ı��ʽ,�ϲ�ǰ������
  TBNode* sufSufTail=sufTail->next;
  preHead->next=head;
  head->last=preHead;
  head->next=sufTail;
  sufTail->last=head;

  //��������Ԫ�ط��ʱ��ʽ,
  sufTail->next=NULL;
  if(sufSufTail!=NULL) sufSufTail->last=NULL;
  
  //����������ʱ��ʽ,������ʽ�������Ӧ����һ��var
  tokens=connect_tokens(tokens,VAR,"");

  //�ϲ�������������ʱ��ʽ��֮ǰ���������
  tokens->next=sufSufTail;
  if(sufSufTail!=NULL) sufSufTail->last=tokens;

  //��������
  layer--;
  addVal_valtbl(funcTranslator->partialValTbl,tokens->token.val,NULL,0,type.defaultName,layer);

  //Ȼ�󷵻ض�ʣ�²��ּ�������Ľ��
  return process_singleLine(funcTranslator,tokens);
}


//��������ǿת���
TBNode* translateTypeChange(FuncTranslator* funcTranslator,TBNode* tokens){
  //���ȵ�һ����ȡ������
  char* typeName=tokens->next->token.val;

  //��������
  Type type;
  int layer;
  findType_valtbl(funcTranslator->partialValTbl,&type,&layer);

  //Ȼ���������ʽ
  TBNode* preHead=tokens->next->next;
  TBNode* head=tokens->next->next->next;

  //�����и��������ʽ
  preHead->next=NULL;
  head->last=NULL;

  head=process_singleLine(head);  //����Ӧ����������ʽ
  if(head==NULL){
    del_tokenLine(tokens);
    return NULL;
  }


  //���ӻغ�����ʽ

  preHead->next=head;
  head->last=preHead;

  //Ȼ��Ѻ�����ʽ�ϲ�����������
  tokens=connect_tokens(tokens,VAR,"");
  //���������
  addVal_valtbl(funcTranslator->partialValTbl,tokens->token.val,NULL,0,type.defaultName,layer);
  return tokens;
}

//��������������
TBNode* translateVarDef(FuncTranslator* funcTranslator,TBNode* tokens){
  //���Ȼ�ȡ����
  char* typeName=tokens->token.val;
  Type type;
  int typeLayer;
  //��������
  findType_valtbl(funcTranslator->partialValTbl,typeName,&type,&typeLayer);

  /*�������������ܵ���ʽ��Ч��
  int* a,b,*c;���a->int*,b->int,c->int*
  int arr[2][3]; arr->const int**
  int a=2,b;
  int arr[]={2,3,4};
  */
 //TODO


  
  //�ϲ�tokensΪconst
  tokens=connect_tokens(tokens,CONST," ");
  return tokens;
}

//���볣���������
TBNode* translateConstDef(FuncTranslator* functranslator,TBNode* tokens){
  /*
  �����ڱ����������,�����ǰѱ�������Ϊ����
  */

  return tokens;
}

//�����������
TBNode* translateCountDef(FuncTranslator* functranslator,TBNode* tokens){
  //TODO

  return tokens;
}

//����ָ�붨�����
TBNode* translateFuncPointerDef(FuncTranslator* functranslator,TBNode* tokens){

}

//typedef�������ͱ������
TBNode* translateTypedef(FuncTranslator* functranslator,TBNode* tokens){

}

//���뺯���������
TBNode* translateFuncUse(FuncTranslator* functranslator,TBNode* tokens){
  //TODO

  return tokens;
}

//���븳ֵ���
TBNode* translateAssign(FuncTranslator* functranslator,TBNode* tokens){
  //TODO

  return tokens;
}






//�������ͷ����������
TBNode* translateTypeMethodUse(FuncTranslator* functranslator,TBNode* tokens){

  //���һ��λ�þ�����,����������з���



}

