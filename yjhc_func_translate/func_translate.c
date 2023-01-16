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

  out.globalValTbl=malloc(sizeof(FuncTbl));
  *(out.globalValTbl)=getValTbl(*(out.gloabalTypeTbl));
  loadFile_valtbl(out.globalValTbl,valFin);

  out.partialValTbl=out.globalValTbl;

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
  int funcIndex=0;  //��¼���뵽�ĺ������±�
  while((nodes=readTokenSentence(&actionSet))!=NULL){
    //�Ƚ��д���
    nodes=process_singleLine(funcTranslator,nodes);
    //Ȼ��Ѵ�����д���ļ�
    if(nodes!=NULL) fput_tokenLine(fout,nodes);
    //���п�Ľ�������
    if(preBlocks==actionSet.blocks-1){
      //�ж��Ƿ�Ҫ���غ���
      if(preBlocks==0){ //���غ���
        //���Ҷ�Ӧ����
        char* name;
        vector_get(&funcTranslator->funcTbl->funcNames,funcIndex,&name);
        //�������ֲ��Һ���
        hashtbl_get(&funcTranslator->funcTbl->funcs,&name,&funcTranslator->curFunc);
        //TODO���밲ȫ���
        if(funcTranslator->curFunc==NULL){
          isRight=0;
          break;
        }
      }
      //�����µľֲ�����
      funcTranslator->partialValTbl->next=malloc(sizeof(ValTbl));
      *(funcTranslator->partialValTbl->next)=getValTbl(getTypeTbl());
      funcTranslator->partialValTbl->next->next=NULL;
      funcTranslator->partialValTbl->next->pre=funcTranslator->partialValTbl->next;
      funcTranslator->partialValTbl=funcTranslator->partialValTbl->next;
      //TODO,���뺯���Ĳ������ֲ�������
      
    }
    else if(preBlocks==actionSet.blocks+1){
      //TODO,�жϺ����Ƿ��˳�
      if(preBlocks==1){
        funcTranslator->curFunc=NULL;
      }
      //�ͷžֲ�����
      ValTbl* tmpValTbl=funcTranslator->partialValTbl;
      //���а�ȫ�Լ��,TODO
      if(tmpValTbl==funcTranslator->globalValTbl){
        isRight=0;
        break;
      }
      funcTranslator->partialValTbl=tmpValTbl->pre;
      del_valTbl(tmpValTbl);
      free(tmpValTbl);
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
  //�����������ѯ
  //�������typedef,��˵����������������
  if(in_bitmap(&bmu,&bm,TYPEDEF_KEYWORD)) return TYPEDEF_FTK;
  //�����ͷ������,����const�ؼ���ʲô��,�������������
  if(nodes->token.kind==TYPE){
    return VAR_DEFINE_FTK;
  }
  else if(nodes->token.kind==CONST){
    return CONST_DEFINE_FTK;
  }
  //�ж��Ƿ�����Ҫ����ĺ�������
  //Ҳ�����ж��Ƿ���a.funcA()��������,����������ڳ�Ա�����ڵĻ�,
  //�ж��Ƿ���ʡ����self��funcA()
  //�ֻ����Ƿ���self.funcA()


  //�ж��Ƿ��Ǹ�ֵ���
  //Ҳ�����ж��Ƿ���a=1����a,b,c=1,2,3��������

  //�ж��Ƿ�

  
  

  


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

