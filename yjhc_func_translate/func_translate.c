#include "func_translate.h"



//��������������,�������ǰ�δ�����yjhc�ĺ�������tokenתΪc�ĺ�������token����
FuncTranslator getFuncTranslator(char* typePath,char* funcHeadPath,char* valPath){
  FuncTranslator out={
    .funcTbl=NULL,
    .gloabalTypeTbl=NULL,
    .partialValTbl=NULL,
    .globalValTbl=NULL,
    .curFunc=NULL
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
        printf("redefinition of function %s\n",funcName);
      else{
        Type type;
        int typeIndex;
        extractTypeIndexAndPointerLayer(typeId,&typeIndex,NULL);
        vector_get(&translator->gloabalTypeTbl->types,typeIndex,&type);
        printf("redefinition of member function %s of %s type\n",funcName,type.defaultName);
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
          printf("member func %s of %s type not find definition\n",funcsArr[j],type.defaultName);
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
    printf("definition of member function %s but without declaration in \"%s\"'s definition\n",funcName,type.defaultName);
  }
  free(keys);
  initStrSet(&strset);
  if(!isRight) return 0;
  return 1;
}




//ʹ�ú�����������ʼ����
int func_translate(FuncTranslator* funcTranslator,char* tokenInPath,char* tokenOutPath){
  if(tokenInPath==NULL || tokenOutPath==NULL ) return 0;
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
    //��һ�����Ӳ��亯��
    nodes=process_singleLine(funcTranslator,nodes);

    //Ȼ��Ѵ�����д���ļ�
    if(nodes!=NULL) fput_tokenLine(fout,nodes);

    //����һ��debug�Ĵ���
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
  return 1;
}



//�жϾ��ӵķ�������,��ѡ��ͬ�ķ������
FTK getTokenLineKind(FuncTranslator* funcTransltor,TBNode* nodes){
  //���һ����������ȫ�����ǽ��,����������ǲ��÷����
  //���Ƚ���һ�α���,ȡ�����з��ŵ�����
  //Ȼ��ʹ��λ�������жϸ÷����Ƿ���ȷ
  //����˵0000_0000�˸�λ�ñ�ʾ���ַ���
  //�����ǵľ��ӵķ�������ֲ��� 1100_0000��1��ʾ��
  //Ҫ�ж��Ƿ�ȫ�����Ŷ���0001_1111��
  //��ȡ������1110_0000,Ȼ���ú�����ǰ����������
  //��������Ϊ0,��˵��Ŀ����Ӻ�����������,����û��
  if(nodes==NULL) return NOT_TRANSLATE_FTK;
  if(funcTransltor->curFunc==NULL) return NOT_TRANSLATE_FTK;
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
  //�������sepOrCon����ı��,
  //������Ϊ0,˵��û���������,
  //˵��û��sepOrCon������ַ�
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
  //�ж��Ƿ���������ʽ�������������ʽ�������������ʽ

  //�ж��Ƿ��Ǻ����������,�����Ǻ�����������
  if(nodes->token.kind==FUNC){
    //�����һ���Ǻ���,��Ҫ�ж���������Ƿ����ڲ�����
    return FUNC_USE_FTK;
  }
  //�ж��Ƿ��Ƕ�����÷������
  if(nodes->token.kind==VAR&&nodes->next!=NULL&&nodes->next->token.kind==OP){
    TBNode* tn=nodes->next->next;
    if(tn!=NULL&&tn->token.kind==FUNC) return TYPE_METHOD_USE_FTK;
  }
  //�ж��Ƿ��ǵ�һ������ʹ�����

  return NOT_TRANSLATE_FTK;
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

}

//�������ͷ����������
TBNode* translateTypeMethodUse(FuncTranslator* functranslator,TBNode* tokens){

}

