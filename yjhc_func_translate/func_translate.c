#include "func_translate.h"



//��������������,�������ǰ�δ�����yjhc�ĺ�������tokenתΪc�ĺ�������token����
FuncTranslator getFuncTranslator(char* typePath,char* funcHeadPath,char* valPath,char* tokenPath,char* tokenOutPath){
  FuncTranslator out={
    .fout=NULL,
    .funcTbl=NULL,
    .gloabalTypeTbl=NULL,
    .partialValTbl=NULL,
    .ValTbl=NULL
  };
  if(typePath==NULL||funcHeadPath==NULL||valPath==NULL||tokenPath==NULL||tokenOutPath==NULL) return out;
  int isRight=1;  //��¼����ȷ��
  FILE* typeFin=fopen(typePath,"r");
  FILE* funcFin=fopen(funcHeadPath,"r");
  FILE* valFin=fopen(valPath,"r");
  FILE* tokenFin=fopen(tokenPath,"r");
  FILE* fout=fopen(tokenOutPath,"w");
  if(typeFin==NULL||funcFin==NULL||valFin==NULL||tokenFin==NULL||fout==NULL){
    isRight=0;
  }
  if(!isRight){
    if(typeFin!=NULL) fclose(typeFin);
    if(funcFin!=NULL) fclose(funcFin);
    if(valFin!=NULL) fclose(valFin);
    if(tokenFin!=NULL) fclose(tokenFin);
    if(fout!=NULL) fclose(fout);
    return out;
  }
  //���������ȷ,���г�ʼ������������
  init_token_reader(tokenFin);

  out.gloabalTypeTbl=malloc(sizeof(TypeTbl));
  *(out.gloabalTypeTbl)=getTypeTbl();
  loadFile_typeTbl(out.gloabalTypeTbl,typeFin);

  out.funcTbl=malloc(sizeof(FuncTbl));
  *(out.funcTbl)=getFuncTbl(out.gloabalTypeTbl);
  loadFile_functbl(out.funcTbl,funcFin);

  out.ValTbl=malloc(sizeof(FuncTbl));
  *(out.ValTbl)=getValTbl(*(out.gloabalTypeTbl));
  loadFile_valtbl(out.ValTbl,valFin);

  out.fout=fout;

  //��ɿռ������ͷſռ�
  free(typeFin);
  free(funcFin);
  free(valFin);
  return out;
}



//ʹ�ú�����������ʼ����
int func_translate(FuncTranslator* funcTranslator,char*){

}


//��������ͷź���������
int release_FuncTranslator(FuncTranslator* funcTranslator){

}




//�жϾ��ӵķ�������,��ѡ��ͬ�ķ������
FTK getTokenLineKind(TBNode* tokens){

}

//���빦���Ӵ���
int translateVarDef(TBNode* tokens){

}
//���볣���������
int translateConstDef(TBNode* tokens){

}

//�����������
int translateCountDef(TBNode* tokens){

}

//����ָ�붨�����
int translateFuncPointerDef(TBNode* tokens){

}

//typedef�������ͱ������
int translateTypedef(TBNode* tokens){

}

//���뺯���������
int translateFuncUse(TBNode* tokens){

}

//���븳ֵ���
int translateAssign(TBNode* tokens){

}

//�������ͷ����������
int translateTypeMethodUse(TBNode* tokens){
  
}




