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
    if(nodes!=NULL) fput_tokenLine(fout,nodes);

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
  if(kind=NOT_LEAGAL_FTK){
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
  //���һ����������ȫ�����ǽ��,����������ǲ��÷����
  //���Ƚ���һ�α���,ȡ�����з��ŵ�����
  //Ȼ��ʹ��λ�������жϸ÷����Ƿ���ȷ
  //����˵0000_0000�˸�λ�ñ�ʾ���ַ���
  //�����ǵľ��ӵķ�������ֲ��� 1100_0000��1��ʾ��
  //Ҫ�ж��Ƿ�ȫ�����Ŷ���0001_1111��
  //��ȡ������1110_0000,Ȼ���ú�����ǰ����������
  //��������Ϊ0,��˵��Ŀ����Ӻ�����������,����û��
  if(nodes==NULL) return NOT_TRANSLATE_FTK;
  if(funcTranslator->curFunc==NULL) return NOT_TRANSLATE_FTK;
  BitMapUtil bmu={
    .mapSize=sizeof(long long)
  };
  BitMap bm=getBitMap(&bmu);
  TBNode* track=nodes;
  while(track!=NULL){
    put_bitmap(&bmu,&bm,track->token.kind);
    track=track->next;
  }
  long long curMap;
  memcpy(&curMap,bm.map,sizeof(curMap));
  long long sepOrCon=SEP_BITMAP|CONTROL_KEYWORD_BITMAP;
  //�ж��Ƿ�������н���͹ؼ��ֵı��ʽ,���ý����κη��붯������
  if(((~sepOrCon)&curMap)==0){
    return NOT_TRANSLATE_FTK;
  }
  
  //�ж��Ƿ���typedef���
  if(nodes->token.kind==TYPEDEF_KEYWORD){
    return TYPEDEF_FTK;
  }
  //�ж��Ƿ��Ǳ����������
  if(nodes->token.kind==TYPE&&nodes->next!=NULL&&nodes->next->token.kind==VAR){
    return VAR_DEFINE_FTK;
  }
  //�ж��Ƿ��ǳ����������
  if(nodes->token.kind==CONST_KEYWORD){
    if(nodes->next!=NULL&&nodes->next->token.kind==TYPE) {
      return CONST_DEFINE_FTK;
    }
  }
  
  //�ж��Ƿ��Ǻ����������
  if(nodes->token.kind==FUNC){
    return FUNC_USE_FTK;
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

  //�ж��Ƿ��ǲ���Ҫ���г�Ա�������÷�������
  if(isNotNeedTranslate(funcTranslator,nodes)){
    return NOT_TRANSLATE_FTK;
  }
  if(funcTranslator->judSentenceKindErr){
    //�����쳣,�����ض��ж�:���Ӳ����﷨
    funcTranslator->judSentenceKindErr=0;
    return NOT_LEAGAL_FTK;
  }
  
  //������Ҫ���г�Ա�������÷���
  return MEMBER_FUNCTION_USE_FTK;
}

//�ж��Ƿ��ǲ���Ҫ��������,Ҳ����û�г�Ա�������õ����
int isNotNeedTranslate(FuncTranslator* funcTranslator,TBNode* nodes){
  //���û�г�Ա��������,����Ҫ����
  //�ж��Ƿ��г�Ա�������õķ���
  TBNode* track=nodes;
  //�������self�Ͳ��ǲ���Ҫ��������
  while(track!=NULL){
    if(track->token.kind==SELF_KEYWORD){
      return 0; //��Ҫ����
    }
    track=track->next;
  }
  //���û���������˳�,˵�����治����self�ؼ���
  track==nodes;
  while (track!=NULL)
  {
    if(track->token.kind!=FUNC||track->last==NULL||strcmp(track->last->token.val,".")!=0){
      track=track->next;
      continue;
    }
    //��ǰ����ֱ����һ�����ʽ����Ϊֹ

    //����������NULLΪֹ,������NULL˵������쳣
    TBNode* ownerHead=searchOwnerHead(track->last->last);
    TBNode* ownerTail=track->last->last;
    if(ownerHead!=NULL){
      //��ȡ���ʽ����
      Type type;
      int retLayer;
      //�����ȡ���ʽ���ͳɹ�,���������ж��Ƿ�ʽ��Ա��������
      if(getTypeOfExpression(funcTranslator,nodes,&type,&retLayer)){
        
      }
      //�����ȡʧ��,����
      else{

      }

    }

    track=track->next;
  }
  return 1; //�����ѭ����û�з��ֲ������,
}

//��ǰ������һ���������ʽ�����
TBNode* searchOwnerHead(TBNode* tail){
  TBNode* track=tail;
  //�Ա��ʽ��������
  TBNode* head=tail;
  while(track!=NULL){
    if(track->token.kind==VAR){
      head=track;
    }
    else if(track->token.kind==RIGHT_PAR){
      //��ǰ����ֱ�����ű��ʽ����
      int count=1;
      TBNode* track2=track->last;
      TBNode* sufTrack2=NULL;
      while(track2!=NULL&&count!=0){
        if(track2->token.kind==RIGHT_PAR) count++;
        else if(track2->token.kind==LEFT_PAR) count--;
        sufTrack2=track2;
        track2=track2->last;
      }
      //�쳣���,���Ų�����,����NULL
      if(count!=0){
        return NULL;
      }
      //�����Ƕ����
      if(track2==NULL||track2->token.kind!=FUNC){
        head=sufTrack2;
      }
      else{
        head=track2;
      }
    }
    else if(track->token.kind==RIGHT_BRACKET){
      //��ǰ����ֱ��������ʽ����
      int count=1;
      TBNode* track2=track2->last;
      TBNode* sufTrack2=NULL;
      while(track2!=NULL){
        sufTrack2=track2;
        if(track2->token.kind==RIGHT_BRACKET&&) count++;
        else if(track2->token.kind==LEFT_BRACKET) count--;
        if(count!=0||track2->last==NULL||track2->last->token==RIGHT_BRACKET) track2=track2->last;
        sufTrack2=track2->next;
        break;
      }
      //���ʽǰ�������Ӧ����field,������
      if(count!=0||track2==NULL){
        return NULL;
      }
      if(track2->last->token.kind==VAR){
        head=track2->last;
      }
      else{
        return NULL;
      }
    }
    else return NULL;
    if(head->last->token.kind==OP&&strcmp(head->last->token.val,".")==0){
      track=head->last->last
    }
    else break;
  }
  return head;
}


//��ȡ���ʽ������
int getTypeOfExpression(FuncTranslator* translator,TBNode* nodes,Type* retType,int* retTypeLayer){

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


//�ж��Ƿ��Ǹ�ֵ���
int isAssignSentence(FuncTranslator* funcTranslator,TBNode* nodes){
  //׼������
  // ��ʽΪ:var=val,var2=val,...var3=val
  //׼�����Զ������ж��Ƿ��Ǹ�ֵ���
  int state=0;
  while(nodes!=NULL){
    //��ʼ״̬������������VAR
    if(state==0&&nodes->token.kind==VAR){
      state=1;
    } 
    //����var֮�����״̬1,��������,����=
    //����','����״̬0,�ڴ�������һ������
    else if(state==1&&nodes->token.kind==COMMA){
      state=0;
    }
    //����=��˵���Ǹ�ֵ���
    else if(state==1&&strcmp(nodes->token.val,"=")==0){
      return 1;
    }else{
      funcTranslator->judSentenceKindErr=1;
      return 0;
    }
    nodes=nodes->next;
  }
  return 0;
}



//���빦���Ӵ���,����ɹ����ط�NULL,����ʧ�ܷ���NULL

//��������������
TBNode* translateVarDef(FuncTranslator* functranslator,TBNode* tokens){

}

//���볣���������
TBNode* translateConstDef(FuncTranslator* functranslator,TBNode* tokens){

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

}

//���븳ֵ���
TBNode* translateAssign(FuncTranslator* functranslator,TBNode* tokens){
  //���Ȼ�ȡ���͵�typeId

  //Ȼ��Ѻ����ÿ�����������������,������Ϊ���
  char* typeName=tokens->token.val; //����ȡ��������

  TBNode* track=tokens->next;
  TBNode* tHead=NULL;
  int state=0;
  int leftPar=1; //ͳ������������,ֻ������������Ϊ0��ʱ�����������ŲŽ��д���
  //TODO
  while(track!=NULL){
    if(state==0){
      //ȡ��������
      if(track->token.kind!=VAR) return NULL;
      addVal_valtbl(functranslator->partialValTbl,track->token.val,NULL,0,typeName);
      tHead=track;
      state=1;
    }
    else if(state!=1) return NULL;
    //�����һ�����ӵĽ�β
    else if(leftPar==0&&track->token.kind==COMMA){
      //��ȡ��������ӽ��д���
      TBNode* tail=track->last;
      TBNode* preHead=tHead->last;
      tHead->last=NULL;
      tail->next=NULL;
      tHead=process_singleLine(functranslator,tHead); //������˫������ͷ��
      //TODO,
      tail=findTBNodesTail(tHead); //�ҵ�˫������β��


    }
    else if(track->token.kind==RIGHT_PAR){
      leftPar--;
    }
    else if(track->token.kind==LEFT_PAR){
      leftPar++;
    }

  }
  //�����һ����д���
  if(tHead->next!=NULL){

  }

}






//�������ͷ����������
TBNode* translateTypeMethodUse(FuncTranslator* functranslator,TBNode* tokens){

}

