#include "func_translate.h"



//��������������,�������ǰ�δ�����yjhc�ĺ�������tokenתΪc�ĺ�������token����
FuncTranslator getFuncTranslator(char* typePath,char* funcHeadPath,char* valPath){
  FuncTranslator out={
    .funcTbl=NULL,
    .gloabalTypeTbl=NULL,
    .partialValTbl=NULL,
    .valTbl=NULL
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

  out.valTbl=malloc(sizeof(FuncTbl));
  *(out.valTbl)=getValTbl(*(out.gloabalTypeTbl));
  loadFile_valtbl(out.valTbl,valFin);

  //��ɿռ������ͷſռ�
  fclose(typeFin);
  fclose(valFin);
  fclose(funcFin);
  return out;
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
  while((nodes=readTokenSentence(&actionSet))!=NULL){
    //�Ƚ��д���
    nodes=process_singleLine(funcTranslator,nodes);
    //Ȼ��Ѵ�����д���ļ�
    if(nodes!=NULL) fput_tokenLine(fout,nodes);
    //���п�Ľ�������
    if(preBlocks==actionSet.blocks-1){
      //TODO

    }
    else if(preBlocks==actionSet.blocks+1){
      //TODO

    }
    preBlocks=actionSet.blocks;
    del_tokenLine(nodes);
  }
  release_token_reader();
  fclose(fin);
  fclose(fout);
  return 1;
}

//��nodeds���д���,�ɹ����ط�0ֵ,ʧ�ܷ���0
TBNode* process_singleLine(FuncTranslator* funcTranslator,TBNode* nodes){
  //�����ж�����
  FTK kind=getTokenLineKind(funcTranslator,nodes);
  if(tranFuncs[kind]!=NULL)
    nodes=tranFuncs[kind](funcTranslator,nodes);
  return nodes;
}


//��������ͷź���������
int release_funcTranslator(FuncTranslator* funcTranslator){
  //ɾ�������Լ���Ƕ�����ͱ�
  while(funcTranslator->valTbl!=NULL){
    ValTbl* tmpTbl=funcTranslator->valTbl;
    funcTranslator->valTbl=tmpTbl->next;
    del_valTbl(tmpTbl);
    free(tmpTbl);
  }
  del_functbl(funcTranslator->funcTbl); //ɾ��������
  free(funcTranslator->funcTbl);
  free(funcTranslator->gloabalTypeTbl);
  funcTranslator->funcTbl=NULL;
  funcTranslator->gloabalTypeTbl=NULL;
  funcTranslator->partialValTbl=NULL;
  return 1;
}


//�жϾ��ӵķ�������,��ѡ��ͬ�ķ������
FTK getTokenLineKind(FuncTranslator* funcTransltor,TBNode* tokens){



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

