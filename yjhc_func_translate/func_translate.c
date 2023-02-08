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
  *(out.gloabalTypeTbl)=getGlobalTypeTbl();
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

    if(nodes==NULL){
      isRight=0;
      break;
    }

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
  //����Ǵ��������
  if(!isRight){
    fprintf(funcTranslator->warningFout,"\nsyntax error!\n");
  }
  if(!isRight) return 0;
  return 1;
}


//��ȫ��Ա�������������������Եĵ���
TBNode* member_use_complement(FuncTranslator* funcTranslator,TBNode* nodes){
  //������ǳ�Ա�������ò�ȫ����
  if(funcTranslator->curFunc==NULL) return nodes;
  char* ownerName=funcTranslator->curFunc->owner;
  if(ownerName==NULL) return nodes; //�������û��������,����������ú���
  //ȡ���������ˣ��ҵ���Ӧ����
  int typeIndex=findType(funcTranslator->gloabalTypeTbl,ownerName,NULL);
  Type type;
  if(typeIndex!=0){
    vector_get(&funcTranslator->gloabalTypeTbl->types,typeIndex,&type);
  }
  //��������ݴ�����
  TBNode* track=nodes;
  TBNode* pre=NULL;
  TBNode* prePre=NULL;
  TBNode head;
  head.next=nodes;
  nodes->last=&head;
  //��ʼ��������Ҫ��ȫ��λ��
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
  if(nodes->token.kind>Tokens_NUM) return NOT_TRANSLATE_FTK;    //�����һ�������ľ���,���÷���
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
  //�ж��Ƿ����������Ե������,self.var����ʽ
  if(nodes->token.kind==SELF_KEYWORD){
    if(nodes->next->token.kind==OP&&strcmp(nodes->next->token.val,".")==0){
      if(nodes->next->next->token.kind==VAR) return SELF_FIELD_VISIT_FTK;
      else if(nodes->next->next->token.kind==FUNC) return SELF_FUNC_VISIT_FTK;
      else return NOT_LEAGAL_FTK;
    }
    else return NOT_LEAGAL_FTK;
  }

  //�ж��Ƿ�������ǿת���
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

  //����ǵ�����֪����,���ǲ���Ҫ���������
  if(nodes->next==NULL&&nodes->token.kind==VAR){
    return 1;
  }


  //���ֻ��������ʽ�ͳ������Ż��߽����,����û�����Է������,���ǲ���Ҫ�������÷���ľ���
  BitMapUtil bmu={
    .mapSize=sizeof(long long)
  };
  BitMap bm=getBitMap(&bmu);
  TBNode* track=nodes;
  while(track!=NULL){
    int ifRet=0;
    TokenKind kind=track->token.kind;
    // if(kind==LEFT_BRACE||kind==LEFT_PAR||kind==LEFT_BRACKET
    // ){
    //   ifRet=1;
    // }
    // else if(track->token.kind==OP&&strcmp(track->token.val,".")==0){
    //   ifRet=1;
    // }
    // if(ifRet){
    //   delBitMap(&bm);
    //   return 0;
    // }
    put_bitmap(&bmu,&bm,track->token.kind);
    track=track->next;
  }
  long long mode=CONTROL_KEYWORD_BITMAP|SEP_BITMAP|(1<<CONST);
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
      toNum++;
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
  if(nodes==NULL||nodes->token.kind!=LEFT_BRACKET) return 0;
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
//������һ��������鹦�ܣ����ƥ�䷵�طǣ�ֵ����鲻ƥ�䷵�أ�
//����������治����Է����argNodes���л��չ���
int checkArgs(FuncTranslator* transLator,Func* thisFunc,TBNode* argNodes){
  //argNodes������(arglist)
  //�������ſ�ͷ,�����Ž�β
  //�ܹ��Ӻ����ֻ�ȡ�������ֺͲ����б�
  int argIndex=0;
  TokenKind ends[]={COMMA,RIGHT_PAR};
  int endsSize=2;
  //�Ե�һ������������ѭ����ȡ����
  //��ʼ��ʱ��argNodes���ڿ�ͷ��������λ��
  //����������û�ж�ȡ�������б�ľ�ͷ��ʱ���ѭ����ȡ
  while(argNodes!=NULL&&argIndex<thisFunc->args.size){
    //��ȡֱ�����Ż��������Ž�β
    TBNode* tail=NULL;
    TBNode* preHead=NULL;
    TBNode* head=argNodes->next;;
    TBNode* sufTail=NULL;
    //�����argIndex����������ʧ��
    if(!searchExpressUntil(head,&tail,ends,endsSize)||tail==NULL||tail->next==NULL||head->token.kind==COMMA||head->token.kind==RIGHT_PAR){
      fprintf(transLator->warningFout,"%d arg not found in use of func %s:\n\t",argIndex+1,thisFunc->func_name);
      return 0;
    }
    //�������������ɹ�,ȡ���������ʽ���д���
    preHead=head->last;
    sufTail=tail->next;
    
    preHead->next=sufTail;
    head->last=NULL;
    tail->next=NULL;
    sufTail->last=preHead;

    //����������ʽ
    head=process_singleLine(transLator,head);
    //�������ʧ��
    if(head==NULL){
      fprintf(transLator->warningFout,"fail to recognized %d arg in use of func %s\n",argIndex+1,thisFunc->func_name);
      return 0;
    }

    //������ɹ�,�������ӻ���,Ȼ���ô���ɹ���ʽ���ж��Ƿ���ƥ���
    sufTail->last=head;
    head->next=sufTail;
    head->last=preHead;
    preHead->next=head;

    //�����β�������Ϣ
    Arg arg;
    Type argType;
    int argTypeLayer;
    //���屣��ʵ��������Ϣ
    Val val;
    Type type;
    int typeLayer;
    //��ȡ�β�����
    vector_get(&thisFunc->args,argIndex,&arg);
    //��ȡ�β�������,ͨ��typeId
    if(!findTypeById(transLator->gloabalTypeTbl,arg.typeId,&argType,&argTypeLayer)){
      // fprintf
      return 0;
    }
    //��ȡʵ����Ϣ
    if(head->token.kind==CONST){
      val.isConst=1;
      if(isConstIntToken(head->token)){
        findType_valtbl(transLator->globalValTbl,"int",&type,&typeLayer);
      }else if(isConstStrToken(head->token)){
        findType_valtbl(transLator->globalValTbl,"char*",&type,&typeLayer);
      }
    }
    else if(!findVal(transLator->partialValTbl,head->token.val,&val,&type,&typeLayer)){
      return 0;
    }
    //��������ƥ��

    //������Ͳ�ƥ��
    if((arg.isConst^val.isConst)||strcmp(argType.defaultName,type.defaultName)!=0||argTypeLayer!=typeLayer){
      //���Ȼ����������ȫ��
      char* typeName=getTypeName(type.defaultName,typeLayer);
      char* argTypeName=getTypeName(argType.defaultName,argTypeLayer);
      if(val.isConst) fprintf(transLator->warningFout,"Error! use const val %s of %s type ",head->token.val,typeName);
      else fprintf(transLator->warningFout,"Error! use var %s of %s type ",head->token.val,typeName);
      
      if(arg.isConst) fprintf(transLator->warningFout,"but not const val of %s type as the %dth arg of func %s\n",argTypeName,argIndex+1,thisFunc->func_name);
      else fprintf(transLator->warningFout,"but not var of %s type as the %dth arg of func %s\n",argTypeName,argIndex+1,thisFunc->func_name);
      free(typeName);
      free(argTypeName);
      return 0;
    }

    //��������ƥ��,����argIndex;
    argIndex++;
    //Ȼ�������һ����������
    argNodes=head->next;
  }
  //����β�������ʵ��������ƥ��
  //���ʵ������С���β�����,(ps:����ƥ���ʱ��Ľ�����ʱ��argIndexӦ�õ����β�����<thisFunc->args.size))
  if(argIndex<thisFunc->args.size){
    fprintf(transLator->warningFout,"miss arguments in use of func %s",thisFunc->func_name);
    if(thisFunc->owner!=NULL) fprintf(transLator->warningFout,"of %s type",thisFunc->owner);
    fprintf(transLator->warningFout,"\n");
    return 0;
  }
  //���ʵ�ʲ��������������βε�����,Ҳ����û�ж���ʵ�ʲ����������Ѿ�ƥ���������βε����
  else if(argNodes->token.kind!=RIGHT_PAR){
    fprintf(transLator->warningFout,"too many arguments of func %s",thisFunc->func_name);
    if(thisFunc->owner!=NULL) fprintf(transLator->warningFout,"of %s type",thisFunc->owner);
    fprintf(transLator->warningFout,"\n");
    return 0;
  }
  //������ǲ����������������,������������,�����б�ƥ����ȷ
  return 1;
}

//�����Ա���Է������
TBNode* translateMemberFieldVisit(FuncTranslator* funcTranslator,TBNode* tokens){
  //��Ҫ��֤�������������tokensһ���Ƕ�Ӧ���͵�
  //���ȵ�һ����һ����VAR,���������쳣���
  //���Ȼ�ȡ����������
  Type type;
  Val val;
  int retLayer;
  char* newTypeName=NULL;  //�±�����������
  //�����û���ҵ����������������Ϊ��c��׼���ﶨ��Ľṹ����,��Ϊ��û�м���c��Ķ���
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
  //�����ж��Ƿ��ǳ�������
  int isConst=0;
  {
    int isRight=1;
    char* arrName=tokens->token.val;
    Val val;
    int layer;
    if(!findVal(funcTranslator->partialValTbl,arrName,&val,NULL,&layer)){
      fprintf(funcTranslator->warningFout,"undefined arr name %s in:\n\t",arrName);
      isRight=0;
    }
    else if(layer<1){
      fprintf(funcTranslator->warningFout,"not arr name %s for arr visit in:\n\t",arrName);
      isRight=0;
    }
    if(!isRight){
      fshow_tokenLine(funcTranslator->warningFout,tokens);
      del_tokenLine(tokens);
      return NULL;
    }
    isConst=val.isConst;
  }
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
    fprintf(funcTranslator->warningFout,"warning!can not visit arr element with empty index!\n");
    del_tokenLine(tokens);
    return NULL;
  }
  //Ȼ�����±���ʽ,��ʵ�±���ʽӦ����һ�����Ϊ���ͱ���������
  
  //���ȷ�����±���ʽ
  head->last=NULL;
  preHead->next=sufTail;
  sufTail->last->next=NULL;
  sufTail->last=preHead;


  //Ȼ�����±���ʽ
  head=process_singleLine(funcTranslator,head);
  //����±���ʽ��������
  if(head==NULL){
    del_tokenLine(tokens);
    return NULL;
  }
  //���ӻ�ԭ���Ľӿ�
  preHead->next=head;
  head->last=preHead;
  head->next=sufTail;
  sufTail->last=head;
  //�ж��±���ʽ������
  Type type;
  Val val;
  int layer;
  int isRight=1;
  //����±���ʽ����������,������ȷ���±���ʽ
  if(head->token.kind==CONST){
    if(!isConstIntToken(head->token)){
      isRight=0;
      fprintf(funcTranslator->warningFout,"index to visit arr should be int,but not const string %s\n",head->token.val);
    }
  }
  //�ӱ���������ȡ�±���ʽ�ı�����Ϣ,������Ҳ���,˵�������쳣
  else if(!findVal(funcTranslator->partialValTbl,head->token.val,&val,&type,&layer)){
    //�����Ī������,��ʾ,Ȼ�󷵻ش�����Ϣ
    fprintf(funcTranslator->warningFout,"var %s used but not defined!\n",head->token);  //ʹ��δ�������
    isRight=0;
  }
  //�жϱ��ʽ��������
  //����±겻��int���ͻ��߲���ָ����Ϊ0,��Ϊ������±���ʽ,�����쳣ֵ
  else if(layer!=0||type.kind!=TYPE_INT){
    char* typeName=getTypeName(type.defaultName,layer);
    fprintf(funcTranslator->warningFout,"you must use int val to visit arr elements but not val %s of %s type\n",head->token.val,typeName);
    free(typeName);
    isRight=0;
  }
  if(!isRight){
    del_tokenLine(tokens);
    return NULL;
  }
  //��������ȷ���͵ı��ʽ

  //TODO,Ȼ���ȡ��������ָ����
  if(!findVal(funcTranslator->partialValTbl,preHead->last->token.val,&val,&type,&layer)){
    fprintf(funcTranslator->warningFout,"unknown arr %s!\n",preHead->last->token.val);
    isRight=0;
  }
  else if(layer<1){
    fprintf(funcTranslator->warningFout,"visit un-arr val %s!\n",preHead->last->token.val);
    isRight=0;
  }
  if(!isRight){
    del_tokenLine(tokens);
    return NULL;
  }
  
  //�������ĵ�
  TBNode* sufSufTail=sufTail->next;



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

  addVal_valtbl(funcTranslator->partialValTbl,tokens->token.val,NULL,isConst,type.defaultName,layer);

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
  findType_valtbl(funcTranslator->partialValTbl,typeName,&type,&layer);

  //Ȼ���������ʽ
  TBNode* preHead=tokens->next->next;
  TBNode* head=tokens->next->next->next;

  //�����и��������ʽ
  preHead->next=NULL;
  head->last=NULL;

  head=process_singleLine(funcTranslator,head);  //����Ӧ����������ʽ
  if(head==NULL){
    del_tokenLine(tokens);
    return NULL;
  }
  //���ӻغ�����ʽ

  preHead->next=head;
  head->last=preHead;

  //Ȼ��Ѻ�����ʽ�ϲ��ɱ�������
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
  StrSet constArrName=getStrSet(myStrHash);
  //��������
  findType_valtbl(funcTranslator->partialValTbl,typeName,&type,&typeLayer);

    /*�������������ܵ���ʽ��Ч��
    int* a,b,*c;���a->int*,b->int,c->int*
    int arr[2][3]; arr->const int**
    int a=2,b;
    int arr[]={2,3,4};
    */
  vector vars=getVector(sizeof(char*));
  vector layers=getVector(sizeof(int));
  //����,��ȡ��һ������,
  TBNode* track=tokens->next;
  int isFirst=1;  //�����ж��Ƿ��Ǹ���䶨��ĵ�һ������
  int state=0;  //����ָ�����붯��
  int isRight=1;
  //Ȼ���ȡ����Ķ���
  while (track!=NULL)
  {
    //״̬0��ʱ��������������
    if(state==0&&track->token.kind==VAR){
      //�ж��Ƿ��ǵ�һ�θ��������,�������ͳ��ǰ���*����
      int layer=0;
      if(isFirst){
        layer=typeLayer;
        isFirst=0;
      }else{
        //ͳ��ǰ���*����
        TBNode* countM=track->last;
        while(countM!=NULL&&countM->token.kind!=TYPE&&countM->token.kind!=COMMA){
          layer++;
          countM=countM->last;
        }
      }
      char* valName=track->token.val;
      vector_push_back(&vars,&valName);
      vector_push_back(&layers,&layer);
      state=1;
    }
    else if(state==0&&track->token.kind==OP&&strcmp(track->token.val,"*")==0){
      // state=0;  //״̬����0����
    }
    //״̬1��ʱ�������������ź�,�ڴ�������,����,�Ⱥ�
    else if(state==1){
      if(track->token.kind==COMMA) state=0;
      else if(track->token.kind==LEFT_BRACKET) state=3;
      else if(track->token.kind==OP&&strcmp(track->token.val,"=")==0){
        state=2;
      }
      else{
        isRight=0;
        break;
      }
    }
    //״̬2�������Ⱥ�֮��,��ȡһ��ֵ���ʽ������
    else if(state==2){
      //��ȡ��,��������NULL����
      TBNode* tail;
      TBNode* sufTail;
      TBNode* head;
      TBNode* preHead;

      //�������ʽֱ������
      TokenKind kinds[]={COMMA };
      if(!searchExpressUntil(track,&tail,kinds,1)){
        isRight=0;
        break;
      }
      head=track;
      preHead=head->last;
      sufTail=tail->next;
      
      //����ֵ���ʽ,��ֵ���ʽ���д���
      preHead->next=NULL;
      head->last=NULL;
      tail->next=NULL;
      if(sufTail!=NULL) sufTail->last=NULL;
      //����ֵ���ʽ
      head=process_singleLine(funcTranslator,head);
      if(head==NULL){
        preHead->next=sufTail;
        if(sufTail!=NULL) sufTail->last=preHead;
        isRight=0;
        break;
      }
      //�������ɹ�,�ӻ�����
      preHead->next=head;
      head->last=preHead;
      head->next=sufTail;
      if(sufTail!=NULL)
        sufTail->last=head;
      //������ֵ���ʽ�������
      if(sufTail==NULL){
        state=1;
        break;
      }else{
        track=sufTail;
        state=0;
      }
    }
    //state3��״̬1����������֮��,��ȡһ��ά�ȴ�С���ʽ������
    else if(state==3){
      //�Ѹ���������ֵ���ʽ
      char* cName=NULL;
      vector_get(&vars,vars.size-1,&cName);
      addStr_StrSet(&constArrName,cName);

      //��ȡ�����Ž��д���,������״̬1
      TBNode* head=track;
      TBNode* preHead=head->last;
      TBNode* tail=NULL;
      TBNode* sufTail=NULL;
      if(!searchBracketExpression(track->last,&head,&tail)){
        isRight=0;
        break;
      }
      //��ȡ�ɹ���head,tail��Ϊֹ�ֱ������һ�����

      //���ݷ����ű��ʽ��÷�������ά�ȴ�С���ʽ
      preHead=head;
      head=preHead->next;
      sufTail=tail;
      tail=sufTail->last;

      //���뷽�����ڱ��ʽ,Ҳ���Ƿ����ά�ȴ�С���ʽ
      preHead->next=NULL;head->last=NULL;
      sufTail->last=NULL;tail->next=NULL;
      
      //�Ըñ��ʽ����
      head=process_singleLine(funcTranslator,head);
      if(!head){
        sufTail->last=preHead;
        preHead->next=sufTail;
        isRight=0;
        break;
      }
      //�����ɹ�������
      preHead->next=head;
      head->last=preHead;
      head->next=sufTail;
      sufTail->last=head;
      //����ά�ȱ��ʽ�ڵ������Ƿ����Ҫ��
      if(head->token.kind==VAR){
        //��ȡ��������,�ж��Ƿ��ǳ���
        //TODO,���������,�����ǳ����͵���
        Val val;
        Type type;
        int typeLayer;
        if(!findVal(funcTranslator->partialValTbl,head->token.val,&val,&type,&typeLayer)){
          fprintf(funcTranslator->warningFout,"var %s used but not defined\n",head->token.val);
          isRight=0;
        }
        else if(typeLayer!=0||type.kind!=TYPE_INT||!val.isConst
        ){
          fprintf(funcTranslator->warningFout,"used %s as arr dimension but not const int val\n",head->token.val);
          isRight=0;
        }
        if(!isRight) break;
      }else if(head->token.kind!=CONST){
        fprintf(funcTranslator->warningFout,"arr definition should use const int val,but not %s\n",head->token.val);
        isRight=0;
        break;
      }else{
        //TODO
      }
      //��ָ���ν��и���
      int layer;
      vector_get(&layers,layers.size-1,&layer);
      layer++;
      vector_set(&layers,layers.size-1,&layer,NULL);

      // vector_get(&layers,layers.size-1,&layer);
      state=1;
      track=sufTail;
    }
    //�쳣���
    else{
      isRight=0;
      break;
    }
    track=track->next;
  }
  //Ȼ��ϲ���������Ϊ���Ͷ������
  //ֵ���ʽȱʧ
  if(state==2){
    fprintf(funcTranslator->warningFout,"miss val for inition in ");
    fshow_tokenLine(funcTranslator->warningFout,tokens);
    fprintf(funcTranslator->warningFout,"\n");
    isRight=0;
  }
  //�������Ŷ����쳣
  else if(state==3){
    fprintf(funcTranslator->warningFout,"syntax error!un complete definition of arr in ");
    fshow_tokenLine(funcTranslator->warningFout,tokens);
    fprintf(funcTranslator->warningFout,"\n");
    isRight=0;
  }
  if(!isRight){
    del_tokenLine(tokens);
    vector_clear(&layers);
    vector_clear(&vars);
    initStrSet(&constArrName);
    return NULL;
  }
  
  //���������
  for(int i=0;i<vars.size;i++){
    //ȡ����
    char* valName=NULL;
    int layer;
    vector_get(&vars,i,&valName);
    //ȡ�����
    vector_get(&layers,i,&layer);

    int isConst=0;
    //TODO,�����Ƿ����ж�,������id�Ǳ���������,������Ϊ��Ӧ��ָ�볣��
    if(containsStr_StrSet(&constArrName,valName)) 
      isConst=1;

    //���Ӵ����ӱ������
    if(!preValNameAddJudge(funcTranslator,valName)){
      fprintf(funcTranslator->warningFout,"fail to define var %s in:\n\t",valName);
      fshow_tokenLine(funcTranslator->warningFout,tokens);
      isRight=0;
      break;
    }
    //��������
    addVal_valtbl(funcTranslator->partialValTbl,valName,NULL,isConst,type.defaultName,layer);
  }
  //�����
  vector_clear(&vars);
  vector_clear(&layers);
  //TODO,�ͷų�id��ռ�
  initStrSet(&constArrName);
  if(!isRight){
    del_tokenLine(tokens); return NULL;
  }

  //�ϲ�tokensΪ�����������
  TBNode* next=tokens->next;
  tokens->next=NULL;
  if(next!=NULL)
    next->last=NULL;
  next=connect_tokens(next,CONST,"");
  tokens->next=next;
  if(next!=NULL) next->last=tokens;
  tokens=connect_tokens(tokens,CONST," ");
  return tokens;
}

//���볣���������
TBNode* translateConstDef(FuncTranslator* funcTranslator,TBNode* tokens){
  /*
  �����ڱ����������,�����ǰѱ�������Ϊ����
  */
  //���Ȼ�ȡ���Ͷ���
  //���Ȼ�ȡ����
  char* typeName=tokens->next->token.val;
  Type type;
  int typeLayer;
  //��������
  if(!findType_valtbl(funcTranslator->partialValTbl,typeName,&type,&typeLayer)){
    //���Ͳ����ڵ������TODO,����

  }
  //TODO,��������Ҫ�����ʼ��ֵ


    /*�������������ܵ���ʽ��Ч��
    const int* a,b,*c;���a->int*,b->int,c->int*
    const int arr[2][3]; arr->const int**
    const int a=2,b;
    const int arr[]={2,3,4};
    */
  vector vars=getVector(sizeof(char*));
  vector layers=getVector(sizeof(int));
  //ֵ��,ÿ�����������ʱ��Ҫ����һ��ֵ,���û�г�ʼ���ͳ�ʼ��ΪĬ��ֵ
  vector initVals=getVector(sizeof(char*));
  //����,��ȡ��һ������,
  TBNode* track=tokens->next->next;
  int isFirst=1;  //�����ж��Ƿ��Ǹ���䶨��ĵ�һ������
  int state=0;  //����ָ�����붯��
  int isRight=1;
  //Ȼ���ȡ����Ķ���
  while (track!=NULL)
  {
    //״̬0��ʱ��������������
    if(state==0&&track->token.kind==VAR){
      //�ж��Ƿ��ǵ�һ�θ��������,�������ͳ��ǰ���*����
      int layer=0;
      if(isFirst){
        layer=typeLayer;
        isFirst=0;
      }else{
        //ͳ��ǰ���*����
        TBNode* countM=track->last;
        while(countM!=NULL&&countM->token.kind!=TYPE&&countM->token.kind!=COMMA){
          layer++;
        }
      }
      //����layer��typeName��ȡĬ��ֵ
      char* defaultVal;
      if(!getDefaultValOfCertainType(&defaultVal,type.kind,layer)){
        isRight=0;break;
      }
      char* valName=track->token.val;
      vector_push_back(&vars,&valName);
      vector_push_back(&layers,&layer);
      //��ʼʱ,���ö�ӦֵΪĬ��ֵ
      vector_push_back(&initVals,&defaultVal);
      state=1;
    }
    else if(state==0&&track->token.kind==OP&&strcmp(track->token.val,"*")==0){
      // state=0;  //״̬����0����
    }
    //״̬1��ʱ�������������ź�,�ڴ�������,����,�Ⱥ�
    else if(state==1){
      if(track->token.kind==COMMA) state=0;
      else if(track->token.kind==LEFT_BRACKET) state=3;
      else if(track->token.kind==OP&&strcmp(track->token.val,"=")==0){
        state=2;
      }
      else{
        isRight=0;
        break;
      }
    }
    //״̬2�������Ⱥ�֮��,��ȡһ��ֵ���ʽ������
    else if(state==2){
      //��ȡ��,��������NULL����
      TBNode* tail;
      TBNode* sufTail;
      TBNode* head;
      TBNode* preHead;

      //�������ʽֱ������
      TokenKind kinds[]={COMMA };
      if(!searchExpressUntil(track,&tail,kinds,1)){
        isRight=0;
        break;
      }
      head=track;
      preHead=head->last;
      sufTail=tail->next;
      
      //����ֵ���ʽ,��ֵ���ʽ���д���
      preHead->next=NULL;
      head->last=NULL;
      tail->next=NULL;
      if(sufTail!=NULL) sufTail->last=NULL;

      //����ʣ�ಿ��
      preHead->next=sufTail;
      if(sufTail!=NULL) sufTail->last=preHead;

      //����ֵ���ʽ
      head=process_singleLine(funcTranslator,head);
      if(head==NULL||head->next!=NULL||head->last!=NULL){
        del_tokenLine(head);
        isRight=0;
        break;
      }
      //�������ɹ�,�ӻ�����
      preHead->next=head;
      head->last=preHead;
      head->next=sufTail;
      if(sufTail!=NULL)
        sufTail->last=head;

      //��ֵ���ʽ���븳ֵ��,�������ֵ���Ƕ�̬��ֵ,Ҫ���տռ�
      vector_set(&initVals,initVals.size-1,&(head->token.val),NULL);

      //������ֵ���ʽ�������
      if(sufTail==NULL){
        state=1;
        break;
      }else{
        track=sufTail;
        state=0;
      }
    }
    //state3��״̬1����������֮��,��ȡһ��ά�ȴ�С���ʽ������
    else if(state==3){
      //��ȡ�����Ž��д���,������״̬1
      TBNode* head=track;
      TBNode* preHead=head->last;
      TBNode* tail=NULL;
      TBNode* sufTail=NULL;
      if(!searchBracketExpression(track->last,&head,&tail)){
        isRight=0;
        break;
      }
      //��ȡ�ɹ���head,tail��Ϊֹ�ֱ������һ�����

      //���ݷ����ű��ʽ��÷�������ά�ȴ�С���ʽ
      preHead=head;
      head=preHead->next;
      sufTail=tail;
      tail=sufTail->last;

      //���뷽�����ڱ��ʽ,Ҳ���Ƿ����ά�ȴ�С���ʽ
      preHead->next=NULL;head->last=NULL;
      sufTail->last=NULL;tail->next=NULL;
      
      //�Ըñ��ʽ����
      head=process_singleLine(funcTranslator,head);
      if(!head){
        sufTail->last=preHead;
        preHead->next=sufTail;
        isRight=0;
        break;
      }
      //�����ɹ�������
      preHead->next=head;
      head->last=preHead;
      head->next=sufTail;
      sufTail->last=head;
      //����ά�ȱ��ʽ�ڵ������Ƿ����Ҫ��
      int isSuitable=0;
      if(head->token.kind==CONST){
        isSuitable=1;
      }
      else if(head->token.kind==VAR){
        //��ȡ��������,�ж��Ƿ��ǳ���
        //TODO,���������,�����ǳ����͵���
        Val val;
        Type type;
        int typeLayer;
        if(findVal(funcTranslator->partialValTbl,head->token.val,&val,&type,&typeLayer)
        &&typeLayer==0&&type.kind==TYPE_INT&&val.isConst
        ){
          isSuitable=1;
        }
      }else{

      }
      //���ά�ȴ�С���ʽ���Ǻ��ʵĳ���,����б�����ʾ
      if(!isSuitable){
        fprintf(funcTranslator->warningFout,"arr definition should use const int val,but not %s\n",head->token.val);
      }
      //��ָ���ν��и���
      int layer;
      vector_get(&layers,layers.size-1,&layer);
      layer++;
      vector_set(&layers,layers.size-1,&layer,NULL);
      state=1;
      track=sufTail;
    }
    //�쳣���
    else{
      isRight=0;
      break;
    }
    track=track->next;
  }
  //Ȼ��ϲ���������Ϊ���Ͷ������
  //ֵ���ʽȱʧ
  
  if(state==2){
    fprintf(funcTranslator->warningFout,"miss val for inition in ");
    fshow_tokenLine(funcTranslator->warningFout,tokens);
    fprintf(funcTranslator->warningFout,"\n");
    isRight=0;
  }
  //�������Ŷ����쳣
  else if(state==3){
    fprintf(funcTranslator->warningFout,"syntax error!un complete definition of arr in ");
    fshow_tokenLine(funcTranslator->warningFout,tokens);
    fprintf(funcTranslator->warningFout,"\n");
    isRight=0;
  }
  if(!isRight){
    del_tokenLine(tokens);
    vector_clear(&vars);
    vector_clear(&initVals);
    vector_clear(&layers);
    return NULL;
  }
  
  //���볣����
  for(int i=0;i<vars.size;i++){
    //ȡ����
    char* valName=NULL;
    int layer;
    char* defaultVal=NULL;
    int ifFree;
    vector_get(&vars,i,&valName);
    //ȡ�����
    vector_get(&layers,i,&layer);
    //ȡ��������ʼֵ
    vector_get(&initVals,i,&defaultVal);
    //TODO,���Ӵ����ӳ������
    if(!preValNameAddJudge(funcTranslator,valName)){
      fprintf(funcTranslator->warningFout,"fail to define const val %s in:\n\t",valName);
      fshow_tokenLine(funcTranslator->warningFout,tokens);
      isRight=0;
      break;
    }
    //��������
    addVal_valtbl(funcTranslator->partialValTbl,valName,defaultVal,1,type.defaultName,layer);
  }
  //���������
  vector_clear(&vars);
  vector_clear(&layers);
  vector_clear(&initVals);
  if(!isRight){
    del_tokenLine(tokens); return NULL;
  }
  //�ϲ�tokensΪ�����������
  TBNode* next=tokens->next;
  tokens->next=NULL;
  if(next!=NULL)
    next->last=NULL;
  next=connect_tokens(next,CONST,"");
  tokens->next=next;
  if(next!=NULL) next->last=tokens;
  tokens=connect_tokens(tokens,CONST," ");
  return tokens;
}

//����ö�ٱ��ʽ
TBNode* translateSetExp(FuncTranslator* funcTranslator,TBNode* tokens){
  //TODO
  tokens=connect_tokens(tokens,CONST," ");  //ö�ٱ��ʽ��ֵ���ܹ��������÷���������Ϊ����
  return tokens;
}

//�����������
TBNode* translateCountDef(FuncTranslator* functranslator,TBNode* tokens){
  
  //����С���ž����������ȼ���
  //��ȥ�����˺����������С����,С���ŵ�������ֻ������,����һ��ֻ�ܹ������㳣��,����С��������������ǵ�һ����
  tokens=removeParExp(functranslator,tokens);
  
  //Ȼ�����������б��ʽ,�����б��ʽ���з�������,ȥ�����л�����
  tokens=removeSetExp(functranslator,tokens);

  //Ȼ��ȥ������self.���ñ��ʽ
  tokens=removeSelfExp(functranslator,tokens);

  //Ȼ��ȥ�����г�Ա���ʱ��ʽ��������ʱ��ʽ
  tokens=removeMemVisitAndArrVisit(functranslator,tokens);

  //ȥ��������ʽ
  tokens=removeOP(functranslator,tokens);

  return tokens;
}


//�������������Ӻ���

//ȥ�����ű��ʽ
TBNode* removeParExp(FuncTranslator* translator,TBNode* tokens){
  //ȥ�����ű��ʽ�ܼ�
  

  return tokens;
}

//ȥ�����б��ʽ
TBNode* removeSetExp(FuncTranslator* translator,TBNode* tokens){
  //TODO,���б��ʽ�����һ������ֵ,�ܹ������Խṹ�����ͻ����������ͽ��г�ʼ��
  //TODO,���б��ʽ���ܹ����÷���,Ҳ���ܹ����в���,���Կ��Ը�һ���������ͱ�־
  //TODO,Ȼ�����б��ʽ���Ϳ�����Ϊһ����������ʹ���
  if(tokens==NULL) return NULL;

  //���ȴ������ű��ʽ�����ÿ���ӱ��ʽ,��ÿ���ӱ��ʽ���д���
  //��ǰ�Ȳ����Ǿ�����ʼ��������,����ÿ���ӱ��ʽ����û�о�����,Ҳû��ָ���±��
  TBNode head;
  head.token.kind=UNKNOWN;
  head.last=NULL;
  head.next=tokens;
  TBNode* track=tokens->next;
  TBNode* tail;
  TokenKind kinds[]={RIGHT_BRACE,COMMA};
  int kindsSize=2;
  int isRight=1;
  while(searchExpressUntil(track,&tail,kinds,kindsSize)){
    TBNode* preHead=track->last;
    TBNode* sufTail=tail->next;

    //������ӱ��ʽ
    preHead->next=sufTail;sufTail->last=preHead;
    track->last=NULL;tail->next=NULL;

    //���ӱ��ʽ����
    track=translateCountDef(translator,track);
    if(track==NULL){
      isRight=0;
      break;
    }
    if(sufTail->token.kind==RIGHT_BRACE) break;
    track=sufTail->next;
  }
  tokens=head.next;
  tokens->last=NULL;
  if(!isRight){
    del_tokenLine(tokens);
    return NULL;
    
  }
  tokens=connect_tokens(tokens,CONST," ");
  return tokens;
}

//ȥ��self���ʽ
TBNode* removeSelfExp(FuncTranslator* translator,TBNode* tokens){
  if(tokens==NULL) return NULL;
  TBNode* track=tokens;
  TBNode head={
    .last=NULL,
    .next=NULL,
    .token={.kind=UNKNOWN,.val=NULL}
  };
  head.next=tokens;
  tokens->last=&head;
  int isRight=1;
  while(track!=NULL){
    if(track->token.kind!=SELF_KEYWORD){
      track=track->next;
      continue;
    }
    if(track->next==NULL||track->next->next==NULL||track->next->token.kind!=OP||strcmp(track->next->token.val,".")!=0){
      isRight=0;
      break;
    }
    //����self���Ե���
    if(track->next->next->token.kind==VAR){
      TBNode* preHead=track->last;
      TBNode* tail=track->next->next;
      TBNode* sufTail=tail->next;
      preHead->next=NULL;track->last=NULL;
      tail->next=NULL;if(sufTail!=NULL) sufTail->last=NULL;
      preHead->next=sufTail;if(sufTail!=NULL) sufTail->last=NULL;
      track=translateSelfFieldVisit(translator,track);
      if(track==NULL){
        isRight=0;
        break;
      }
      preHead->next=track;track->last=preHead;
      track->next=sufTail;if(sufTail!=NULL) sufTail->last=track;
      track=head.next;  //��track������һ��Ԫ��λ��,��������
    }
    //����self��������
    else if(track->next->next->token.kind==FUNC&&track->next->next->next!=NULL
      &&track->next->next->next->token.kind==LEFT_PAR
    ){
      TBNode* preHead=track->last;
      TBNode* tail=track->next->next->next->next;
      TBNode* sufTail=NULL;
      if(!searchArgExpression(tail,&tail)){
        isRight=0;
        break;
      }
      sufTail=tail->next;

      preHead->next=sufTail;if(sufTail!=NULL) sufTail->last=preHead;
      track->last=NULL;tail->next=NULL;

      track=translateSelfFuncVisit(translator,track);
      if(track==NULL){
        isRight=0;
        break;
      }
      preHead->next=track;track->last=preHead;
      track->next=sufTail;if(sufTail!=NULL) sufTail->last=track;
      track=head.next;
    }
    else{
      isRight=0;
      break;
    }
  }
  tokens=head.next;
  tokens->last=NULL;
  if(!isRight){
    del_tokenLine(tokens);
    return NULL;
  }
  return tokens;
}

//ȥ�����г�Ա���ʱ��ʽ��������ʱ��ʽ,(��ʵ��,��Ϊ��Ա���ʵ�Ч����������ʵ�Ч���Ǳ���һ����)
TBNode* removeMemVisitAndArrVisit(FuncTranslator* translator,TBNode* tokens){
  //TODO
  return tokens;
}

//ȥ�������,Ҫ�������ȼ�,��ȥ�����ȼ���ߵ������
TBNode* removeOP(FuncTranslator* translator,TBNode* tokens){
  //TODO
  return tokens;
}



//����ָ�붨�����
TBNode* translateFuncPointerDef(FuncTranslator* functranslator,TBNode* tokens){
  //���ں���ָ��,��ȡ��������,Ȼ�����ȫ�ֺ�����
  //����ָ�����治������δ֪����










  return tokens;
}

//typedef�������ͱ������
TBNode* translateTypedef(FuncTranslator* functranslator,TBNode* tokens){
  //���ȶ�ȡ����һ���ַ����ĵط�,�������������,˵���Ǻ���ָ�붨�����
  TokenKind kinds[]={LEFT_PAR};
  TBNode* tail=NULL;
  //�ж��Ƿ��Ǻ���ָ������������
  if(!searchExpressUntil(tokens,&tail,kinds,1)||tail->next==NULL||tail->next->token.kind!=LEFT_PAR){
    //������Ǻ���ָ������������,������ͨ��������������
    if(tail==NULL||tail->token.kind!=LEFT_PAR){
      if(tokens->next==NULL||tokens->next->next==NULL){
        del_tokenLine(tokens);return NULL;
      }
      char* oldName=tokens->next->token.val;
      char* newName=tokens->next->next->token.val;
      //TODO,���������Ϸ��Լ��
      //������ǺϷ���������
      if(!isLegalNewTypeName(functranslator,newName)){
        fprintf(functranslator->warningFout,"\nfail to use %s as new type name!\n",newName);
        del_tokenLine(tokens);
        return NULL;
      }
      //�����������Ƿ��Ѿ�����
      Type type;
      int layer;
      //����ɵ�������������,������δ֪����
      if(!findType_valtbl(functranslator->partialValTbl,oldName,&type,&layer)||type.kind==TYPE_UNKNOW){
        fprintf(functranslator->warningFout,"undefined old type %s in:\n",oldName);
        fshow_tokenLine(functranslator->warningFout,tokens);  
        del_tokenLine(tokens);
        return NULL;
      }
      //TODO,����������
      if(!assignTypeNewName(functranslator->partialValTbl,oldName,layer,newName)){
        fprintf(functranslator->warningFout,"fail to assign new type name %s to %s in:\n",newName,oldName);
        fshow_tokenLine(functranslator->warningFout,tokens);  
        del_tokenLine(tokens);
        return NULL;
      }
      //Ȼ��ϲ�tokensΪCONST
      tokens=connect_tokens(tokens,CONST," ");
      return tokens;
    }
    del_tokenLine(tokens);
    return NULL;
  }
  
  //�����Ǻ���ָ�붨�����
  char newFuncPointerTypeName[1000];
  char funcPointerTypeName[1000];
  sprint_tbnodes(funcPointerTypeName,sizeof(funcPointerTypeName),tokens->next);
  if(!extractFuncPointerFieldName(funcPointerTypeName,newFuncPointerTypeName,NULL)){
    fprintf(functranslator->warningFout,"fail to extract funcPointer Type in:\n");
    fshow_tokenLine(functranslator->warningFout,tokens);
    del_tokenLine(tokens);
    return NULL;
  }
  if(formatFuncPointerTypeName(funcPointerTypeName)==NULL){
    fprintf(functranslator->warningFout,"unlegal funcPointerType definition in:\n");
    fshow_tokenLine(functranslator->warningFout,tokens);
    del_tokenLine(tokens);
    return NULL;
  }
  //���������������ж�
  if(!isLegalNewTypeName(functranslator,newFuncPointerTypeName)){
    fprintf(functranslator->warningFout,"\nfail to use %s as new type name!\n",newFuncPointerTypeName);
    del_tokenLine(tokens);
    return NULL;
  }
  if (!assignTypeNewName(functranslator->partialValTbl, funcPointerTypeName, 0, newFuncPointerTypeName))
  {
    fprintf(functranslator->warningFout, "fail to assign new type name %s to %s in:\n", newFuncPointerTypeName, funcPointerTypeName);
    fshow_tokenLine(functranslator->warningFout, tokens);
    del_tokenLine(tokens);
    return NULL;
  }
  tail=tokens->next;
  tail->last=NULL;
  tokens->next=NULL;
  tail=connect_tokens(tail,CONST,"");
  tail->last=tokens;
  tokens->next=tail;
  tokens=connect_tokens(tokens,CONST," ");
  return tokens;
}

//�ж��Ƿ��ǺϷ�����������,����Ƿ��ط�0ֵ,������Ƿ���0
int isLegalNewTypeName(FuncTranslator* funcTranslator,const char* newName){
  // ���������Ϸ��Լ��
  // ������������Ѿ�ע��Ϊ����
  //����һ��newName���д���
  char tmpName[1000];
  strcpy(tmpName,newName);
  //�ж��������Ƿ��Ǳ�ע��ı���
  if(hashtbl_get(&funcTranslator->partialValTbl->newOld,&newName,NULL)){
    fprintf(funcTranslator->warningFout,"%s has been used as another type name\n",newName);
    return 0;
  }
  else if (findType_valtbl(funcTranslator->partialValTbl, tmpName, NULL, NULL))
  {
    fprintf(funcTranslator->warningFout,"type %s has defined!\n",tmpName);
    return 0;
  }
  // ������������Ѿ�ע��Ϊ����
  else if (findVal(funcTranslator->partialValTbl, tmpName, NULL, NULL, NULL))
  {
    fprintf(funcTranslator->warningFout,"val %s has defined!\n",tmpName);
    return 0;
  }
  // ������������Ѿ�ע��Ϊ����ָ��
  else if (findFuncPointer_valtbl(funcTranslator->partialValTbl, tmpName,NULL,NULL,NULL ))
  {
    fprintf(funcTranslator->warningFout,"func pointer %s has defined!\n",tmpName);
    return 0;
  }
  //�ж��Ƿ���ں�������
  else if(findFunc(funcTranslator->funcTbl,tmpName,NULL)){
    fprintf(funcTranslator->warningFout,"func %s has defined!\n",tmpName);
    return 0;
  }
  //�ж��Ƿ��ǺϷ���������
  else if(!isLegalId(tmpName)){
    fprintf(funcTranslator->warningFout,"unlegal type name %s!\n",tmpName);
    return 0;
  }
  return 1;
}



//���뺯���������
TBNode* translateFuncUse(FuncTranslator* functranslator,TBNode* tokens){
  char* funcName=tokens->token.val;
  Func* func=findFunc(functranslator->funcTbl,funcName,NULL);
  //������صĺ����ṹ��ָ��ΪNULL�Ļ�,˵����δ֪�ĺ���,��δ֪�������ö�ÿ��������������ƥ��
  //��ÿ������ֻ��Ҫ���б�ﴦ����
  if(func==NULL){
    //�ⲿ��Ӧ���ǲ��ᵽ���λ��,����Ŀǰͷ�ļ�������,���Է���
    TBNode* track=tokens->next->next; //�������㼣������һ��������ͷ��
    int argIndex=0; //��¼��ǰ�����Ĳ����±�
    int isRight=1;  //��Ƿ��������Ƿ�����쳣
    //Ȼ��ÿ�ζ�ȡһ������ֱ����ȡ�����������һ������Ϊֹ
    while(track!=NULL){
      TBNode* argHead=track;  //����ͷ��
      TBNode* argTail=NULL; //����β
      //����������β��
      //�����������β��ʧ��,˵���������ʽ�����쳣
      if(!searchArgExpression(argHead,&argTail)){
        //
        isRight=0;
        break;
      }
      //��������ɹ�,���ȰѲ������ʽժ��,����,Ȼ���ٽӻ�,Ȼ�������һ��
      TBNode* preHead=argHead->last;
      TBNode* sufTail=argTail->next;

      //ժ���������ʽ
      preHead->next=NULL;argHead->last=NULL;
      sufTail->last=NULL;argTail->next=NULL;
      preHead->next=sufTail;sufTail->last=preHead;

      argHead=process_singleLine(functranslator,argHead);
      if(argHead==NULL||argHead->last==NULL||argHead->next!=NULL){
        del_tokenLine(argHead);
        isRight=0;
        break;
      }
      
      //�����������ɹ�,���ӻ���
      preHead->next=argHead;argHead->last=preHead;
      sufTail->last=argHead;argHead->next=sufTail;
      //�ж��Ƿ�Ҫ����
      if(sufTail->token.kind==RIGHT_PAR){
        break;
      }
      track=sufTail->next;
      if(track==NULL){
        isRight=0;
        break;
      }
    }

    //TODO,�쳣����
    if(!isRight){
      del_tokenLine(tokens);
      return NULL;
    }
    //����������������,���廯��һ��unknown���͵ı���
    tokens=connect_tokens(tokens,VAR,"");
    //��Ϊunknown���ͱ�����������
    addVal_valtbl(functranslator->partialValTbl,tokens->token.val,NULL,0,NULL,0);
    return tokens;
  }
  
  TBNode* head=tokens->next;
  TBNode* preHead;
  TBNode* tail;
  TBNode* sufTail;
  TokenKind ends[]={RIGHT_PAR};
  int endsSize=sizeof(ends)/sizeof(ends[0]);
  //�������������б�
  //�������Ϊ��
  if(head->next->token.kind==RIGHT_PAR){
    //�ղ��������вκ���
    if(functranslator->curFunc->args.size!=0){
      fprintf(functranslator->warningFout,"miss arguement for use of func %s in:\n\t",func->func_name);
      fshow_tokenLine(functranslator->warningFout,tokens);
      del_tokenLine(tokens);
      return NULL;
    }
  }
  //��������Ҳ��������б�
  else if(!searchExpressUntil(head->next,&tail,ends,endsSize)||tail==NULL||(tail->next!=NULL&&tail->next->token.kind!=RIGHT_PAR)){
    fprintf(functranslator->warningFout,"unlegal func use in:\n\t");
    fshow_tokenLine(functranslator->warningFout,tokens);
    del_tokenLine(tokens);
    return NULL;
  }
  tail=tail->next;  //��tail����������λ��
  preHead=head->last;
  sufTail=tail->next;

  //����������ű��ʽ
  tail->next=NULL;head->last=NULL;
  if(sufTail!=NULL){
    sufTail->last=preHead;
  }
  preHead->next=NULL;

  //Ȼ����в���ƥ��
  int isRightArgs=checkArgs(functranslator,func,head);
  //�ӻز����б�
  preHead->next=head;head->last=preHead;
  tail->next=sufTail;
  if(sufTail!=NULL) sufTail->last=tail;
  //�������ƥ��ʧ��
  if(!isRightArgs){
    fprintf(functranslator->warningFout,"un correct use of func %s in:\n\t",func->func_name);
    fshow_tokenLine(functranslator->warningFout,tokens);
    del_tokenLine(tokens);
    return NULL;
  }
  //�������ƥ��ɹ�,
  //�������������ֽ��д���

  //���ȷ���� func(...)����,���ⲿ�ֺϲ�
  tail->next=NULL;
  if(sufTail!=NULL) sufTail->last=NULL;
  tokens=connect_tokens(tokens,VAR,"");
  //�ϲ�func(..)���ֺ�������沿������
  tokens->next=sufTail;
  if(sufTail!=NULL) sufTail->last=tokens;

  //��������
  Type type;  //���Ȼ�ȡ����ֵ����
  int typeLayer;
  if(!findTypeById(functranslator->gloabalTypeTbl,func->retTypeId,&type,&typeLayer)){
    fprintf(functranslator->warningFout,"undefined retType of func %s in:\n\t",func->func_name);
    fshow_tokenLine(functranslator->warningFout,tokens);
    del_tokenLine(tokens);
    return NULL;
  }
  //����ֵ���Ͳ��ҳɹ�,��������
  addVal_valtbl(functranslator->partialValTbl,tokens->token.val,NULL,1,type.defaultName,typeLayer);

  return process_singleLine(functranslator,tokens);
}

//���븳ֵ���
TBNode* translateAssign(FuncTranslator* functranslator,TBNode* tokens){
  //TODO,�����ȼ򵥵�ʵ��һ������ֵ���
  
  //����ֵ���ṹΪtg = ...; ...Ϊһ��ֵ���ʽ
  int isRight=1;
  if(tokens==NULL||tokens->next==NULL||tokens->next->next==NULL) isRight=0;
  else if(tokens->next->token.kind!=OP||strcmp(tokens->next->token.val,"=")!=0) isRight=0;
  else if(tokens->token.kind!=VAR){
    fprintf(functranslator->warningFout,"target of assign sentence should be var but not %s in:\n\t",tokens->token.val);
    fshow_tokenLine(functranslator->warningFout, tokens);
    isRight=0;
  }
  if(!isRight){
    del_tokenLine(tokens);
    return NULL;
  }
  //Ȼ��Ժ����ֵ���ʽ���д���,������ȡֵ���ʽ
  TBNode* valHead=tokens->next->next;
  TBNode* preValHead=valHead->last;
  preValHead->next=NULL;valHead->last=NULL;
  valHead=process_singleLine(functranslator,valHead);
  if(valHead==NULL){
    del_tokenLine(tokens);
    return NULL;
  }
  else{
    preValHead->next=valHead;
    valHead->last=preValHead;
  }
  if(tokens->next->next->token.kind!=VAR&&tokens->next->next->token.kind!=CONST)
    isRight=0;
  if(!isRight){
    del_tokenLine(tokens);
    return NULL;
  }

  //�����Ȼ�ȡĿ�ı����Լ�Ŀ�ı�������,�ж��Ƿ�����ƥ��
  Type type;
  Val val;
  int typeLayer;
  char* valName=tokens->token.val;
  if(!findVal(functranslator->partialValTbl,valName,&val,&type,&typeLayer)){
    fprintf(functranslator->warningFout,"undefined var %s in:\n\t",valName);
    isRight=0;
  }
  else if(val.isConst){
    fprintf(functranslator->warningFout,"Error!use const var %s as target var in assign sentence in:\n\t",valName);
    isRight=0;
  }
  if(!isRight){
    fshow_tokenLine(functranslator->warningFout,tokens);
    del_tokenLine(tokens);
    return NULL;
  }
  //TODO,��ֵ��Ŀ���������ͽ����ж��Ƿ�ƥ��
  

  //���ϲ�����tokens
  tokens=connect_tokens(tokens,CONST,"");

  return tokens;
}

//�����������Ե������
TBNode* translateSelfFieldVisit(FuncTranslator* funcTranslator,TBNode* tokens){
  //�����������Ե������,.�����->
  free(tokens->next->token.val);
  tokens->next->token.val=strcpy(malloc(strlen("->")),"->");
  //��ѯ����,Ȼ�������ϲ���һ������
  if(funcTranslator->curFunc==NULL){
    del_tokenLine(tokens);
    return NULL;
  }
  //���ҵ�ǰ����������
  char* ownerName=funcTranslator->curFunc->owner;
  //������������
  Type type;
  if(!findType_valtbl(funcTranslator->globalValTbl,ownerName,&type,NULL)){
    del_tokenLine(tokens);
    return NULL;
  }
  //��������˲����и�����,���б�����ʾ
  char* fieldName=tokens->next->next->token.val;
  if(!hashtbl_get(&type.fields,&fieldName,NULL)){
    //�������������
    fprintf(funcTranslator->warningFout,"visit unexistable field %s in type %s->%s function\n",fieldName,type.defaultName,funcTranslator->curFunc->func_name);
    del_tokenLine(tokens);
    return NULL;
  }
  //����ϲ�Ϊ������,���Һ����ķ�������
  int typeIndex;
  int typeLayer;
  extractTypeIndexAndPointerLayer(funcTranslator->curFunc->retTypeId,&typeIndex,&typeLayer);
  //ȡ������
  vector_get(&funcTranslator->gloabalTypeTbl->types,typeIndex,&type);
  //�ضϺ��沿��
  TBNode* tail=tokens->next->next;
  TBNode* sufTail=tail->next;
  tail->next=NULL;
  if(sufTail!=NULL) sufTail->last=NULL;
  //�ϲ�tokens
  tokens=connect_tokens(tokens,VAR,"");
  if(tokens==NULL){
    del_tokenLine(sufTail);
    return NULL;
  }
  //��������
  addVal_valtbl(funcTranslator->partialValTbl,tokens->token.val,NULL,0,type.defaultName,typeLayer);
  tokens->next=sufTail;
  sufTail->last=tokens;
  return process_singleLine(funcTranslator,tokens);
}

//�����������������
TBNode* translateSelfFuncVisit(FuncTranslator* funcTranslator,TBNode* tokens){
  //���в������
  if(funcTranslator->curFunc==NULL){
    del_tokenLine(tokens);
    return NULL;
  }
  //��ȡ��ǰ��������������
  char* ownerName=funcTranslator->curFunc->owner;
  Type type;
  int layer=0;
  findType_valtbl(funcTranslator->globalValTbl,ownerName,&type,&layer);
  //Ĭ��֮ǰ��Լ�����������ϸ���жϣ���˷������˵�����Ҫô��unionҪô��strcut������ָ����Ϊ0

  //ͨ��self���ú��������������,һ���Ǻ���ָ�����,��һ������������������

  //�����ʹ�ú���ָ��;
  if(containsStr_StrSet(&type.funcPointerFields,tokens->next->next->token.val)){
    // ��ϳɸõ��ú͸ú���Ϊһ��δ֪����,Ȼ��Ժ������з���
    //����һ�ֽ��͵�ѡ��
    //TODO,��ƻ�ȡ����ָ���Ӧ�Ĳ����б�ͷ���ֵ���͵Ľӿ�,�Ժ���ָ��ĺ�������Ҳ�����ж�
    TBNode* tail=tokens->next->next;
    TBNode* sufTail=tail->next;
    free(tokens->next->token.val);
    tokens->next->token.val=strcpy(malloc(strlen("->")),"->");
    tail->next=NULL;if(sufTail!=NULL) sufTail->last=NULL;
    //�ϳ�
    tokens=connect_tokens(tokens,FUNC,"");
    tokens->next=sufTail;
    if(sufTail!=NULL) sufTail->last=tokens;
    //�ϳ�֮������ͨ����
    tokens=process_singleLine(funcTranslator,tokens);
    //�ѷ�����ת��Ϊ���ʵı���
    //TODO
    return tokens;
  }
  //��������ǵ���������
  else if(containsStr_StrSet(&type.funcs,tokens->next->next->token.val)){
    //��ѯ��������
    char* ownerName=type.defaultName;
    char* funcName=tokens->next->next->token.val;
    Func* func=findFunc(funcTranslator->funcTbl,funcName,ownerName);
    //�������û���ҵ�
    if(func==NULL){
      fprintf(funcTranslator->warningFout,"member method %s of type %s undefined but used in:\n\t",funcName,ownerName);
      fshow_tokenLine(funcTranslator->warningFout,tokens);
      del_tokenLine(tokens);
      return NULL;
    }
    //���в���ƥ��,����
    TBNode* track=tokens->next->next->next->next; //��track������һ��������ͷλ��
    //��¼ƥ�䵽�Ĳ���������
    int i=0;
    int isRight=1;  //����Ƿ���������
    //TODO,��ʼƥ��
    while(track->token.kind!=RIGHT_PAR&&i<funcTranslator->curFunc->args.size){
      //��ȡ����ͷ,����process����
      TBNode* head=track;
      TBNode* preHead=head->last;
      TBNode* tail;
      TokenKind kinds[]={COMMA,RIGHT_PAR};
      //���Ҷ���
      if(!searchExpressUntil(head,&tail,kinds,2)||tail==NULL){
        isRight=0;
        break;
      }
      int isFinished=0;
      if(tail->next==NULL){
        isRight=0;
        break;
      }
      TBNode* sufTail=tail->next;
      if(sufTail->token.kind!=COMMA&&sufTail->token.kind!=RIGHT_PAR){
        isRight=0;
        break;
      }
      if(tail->next->token.kind==RIGHT_PAR) isFinished=1;
      //����process����

      //���ȷ���
      tail->next=NULL;
      sufTail->last=preHead;
      preHead->next=sufTail;
      head->last=NULL;

      head=process_singleLine(funcTranslator,head);
      if(head==NULL){
        isRight=0;
        break;
      }
      //TODO,����head����ɹ�,�ӻ���
      preHead->next=head;
      head->last=preHead;
      head->next=sufTail;
      sufTail->last=head;
      
      //Ȼ����в�������ƥ��
      //����ʵ������
      Type type;
      Val val;
      int typeLayer;
      //���ʵ�α���������
      if(!findVal(funcTranslator->partialValTbl,head->token.val,&val,&type,&typeLayer)){
        fprintf(funcTranslator->warningFout,"var %s used but not defined in\n\t:",head->token.val);
        fshow_tokenLine(funcTranslator->warningFout,tokens);
        isRight=0;
        break;
      }
      //Ȼ��ȡ���β�
      Arg arg;
      vector_get(&funcTranslator->curFunc->args,i,&arg);
      //��ȡ��������
      Type argType;
      int argTypeLayer;
      int typeIndex;
      extractTypeIndexAndPointerLayer(arg.typeId,&typeIndex,&argTypeLayer);
      vector_get(&funcTranslator->gloabalTypeTbl->types,typeIndex,&argType);


      //����ƥ��,TODO
      if(!(argType.defaultName==type.defaultName)&&argTypeLayer==typeLayer){
        //��ʾ�ǲ�ͬ��������
        char* argTypeName=getTypeName(argType.defaultName,argTypeLayer);
        char* typeName=getTypeName(type.defaultName,typeLayer);
        fprintf(funcTranslator->warningFout,"%d arg need %s type val but not %s,which is of %s type in:\n\t",i+1,argTypeName,head->token.val,typeName);
        fshow_tokenLine(funcTranslator->warningFout,tokens);
        free(argTypeName);
        free(typeName);
        isRight=0;
        break;
      }
      //�жϳ����Ƿ�ƥ��
      else if(arg.isConst&&!val.isConst){
        //TODO
      } 
      else if(!arg.isConst&&val.isConst){

      }
      

      //�����˳��жϺʹ���,
      //���������track����������β��������λ��
      if(isFinished){
        //TODO,
        track=sufTail;
        break;
      }
      //�����û���������,���뵽��һ��λ��
      else{
        track=sufTail->next;
      }
    }
    //TODO,����β�������ʵ�������Ƿ�ƥ��


    if(!isRight){
      fprintf(funcTranslator->warningFout,"%d arg wrong!in use of func %s of type %s in:\n\t",i+1,funcName,ownerName);
      fshow_tokenLine(funcTranslator->warningFout,tokens);
      fprintf(funcTranslator->warningFout,"in definition of func %s\n",funcTranslator->curFunc->func_name);
      del_tokenLine(tokens);
      return NULL;
    } 

    //���з�������

    //�޸�

    //TODO

  }
  //�������쳣���,���ò���������ķ����������﷨����
  else{
    fprintf(funcTranslator->warningFout,"visit undeclare function %s of type %s!\n",tokens->next->next->token.val,funcTranslator->curFunc->owner);
    del_tokenLine(tokens);
    return NULL;
  }
  return tokens;
}

//�������ͷ����������
TBNode* translateTypeMethodUse(FuncTranslator* functranslator,TBNode* tokens){
  //���ȸ��ݱ�����������������
  Val val;
  Type type;
  int layer;
  if(tokens->token.kind!=VAR){
    del_tokenLine(tokens);
    return NULL;
  }
  if(!findVal(functranslator->partialValTbl,tokens->token.val,&val,&type,&layer)){
    del_tokenLine(tokens);
    return NULL;
  }
  //��������Ǻ����������ָ��
  if(containsStr_StrSet(&type.funcPointerFields,tokens->next->next->token.val)){
    //���뺯��ָ���ڲ���������
    //TODO

  }
  //������Ͳ�����������ͷ���
  if(!containsStr_StrSet(&type.funcs,tokens->next->next->token.val)){
    fprintf(functranslator->warningFout,"%s type val %s doesn't have member function %s\n",type.defaultName,tokens->token.val,tokens->next->next->token.val);
    del_tokenLine(tokens);
    return NULL;
  }
  //��������ͺ��иó�Ա����,���г�Ա��������
  //TODO
  //�����ҵ��÷���
  Func* func=findFunc(functranslator->funcTbl,tokens->next->next->token.val,type.defaultName);
  //Ȼ��,�Ժ��洫��Ĳ������з���
  
  //Ȼ����г�Ա�����������

  //Ȼ�����Ӻ����ͺ�����ʽ



  return tokens;
}


int preValNameAddJudge(FuncTranslator* funcTranslator,char* s){
  if(strToId(funcTranslator->partialValTbl->valIds,s)>=0){
    fprintf(funcTranslator->warningFout,"redefinition of val %s\n",s);
    return 0;
  }
  //�ж��Ƿ���������
  Type type;
  if(findType_valtbl(funcTranslator->partialValTbl,s,&type,NULL)){
    fprintf(funcTranslator->warningFout,"the same typeName %s has exist !\n",s);
    return 0;
  }
  //���ж��Ƿ��ǺϷ�������
  if(!isLegalId(s)){
    //��ʵ����һ��������Ҫ,���ǽ���ִ�е�ʱ������õ�,
    //��Ϊ����ִ�е�ʱ��,ǰ���Ѿ������﷨��飬��������������﷨����ǰ��Ĺ����޷�ͨ��
    //��ʵ��translate��һ�׶�Ӧ�ø���������
    fprintf(funcTranslator->warningFout,"unlegal syntax in val name %s\n",s);
    return 0;
  }
  return 1;
}




int searchBracketExpression(TBNode* nodes,TBNode** head,TBNode** tail){
  //���Ȳ��ҿ�ͷ������
  while(nodes!=NULL){
    if(nodes->token.kind==LEFT_BRACKET){
      *head=nodes;
      break;
    }
    nodes=nodes->next;
  }
  if(nodes==NULL ) return 0;
  int leftP=1;
  do{
    nodes=nodes->next;
    TokenKind kind=nodes->token.kind;
    if(kind==LEFT_PAR||kind==LEFT_BRACE||kind==LEFT_BRACKET) leftP++;
    else if(kind==RIGHT_PAR||kind==RIGHT_BRACE||kind==RIGHT_BRACKET) leftP--;
    if(leftP==0&&kind==RIGHT_BRACKET) break;
  }while(nodes!=NULL);
  if(nodes==NULL) return 0;
  *tail=nodes;
  return 1;
}

//�ڲ����Ŀ�ͷ���������������Ľ�β

int searchArgExpression(TBNode* head,TBNode** tail){
  int leftP=0;
  while(head!=NULL){
    TokenKind kind=head->token.kind;
    if(kind==LEFT_PAR||kind==LEFT_BRACE||kind==LEFT_BRACKET) leftP++;
    else if(kind==RIGHT_PAR||kind==RIGHT_BRACE||kind==RIGHT_BRACKET) leftP--;
    if(leftP==0&&(head->next->token.kind==COMMA||head->next->token.kind==RIGHT_PAR)) break;
    head=head->next;
  }
  if(head==NULL) return 0;
  *tail=head;
  return 1;
}


int searchExpressUntil(TBNode* head,TBNode** retTail,TokenKind* kinds,int kindSize){
  int leftP=0;
  if(head==NULL) return 0;
  
  while(head!=NULL){
    TokenKind kind=head->token.kind;
    if(kind==LEFT_PAR||kind==LEFT_BRACE||kind==LEFT_BRACKET) leftP++;
    else if(kind==RIGHT_PAR||kind==RIGHT_BRACE||kind==RIGHT_BRACKET) leftP--;
    int ifBreak=0;
    if(head->next==NULL) break;
    kind=head->next->token.kind;
    for(int i=0;i<kindSize&&leftP==0;i++){
      if(kinds[i]==kind){
        ifBreak=1;
        break;
      }
    }
    if(ifBreak) break;
    head=head->next;
  }
  if(leftP!=0) return 0;
  *retTail=head;
  return 1;
}
