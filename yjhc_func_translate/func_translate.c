#include "func_translate.h"



//创建函数翻译器,翻译结果是把未翻译的yjhc的函数代码token转为c的函数代码token序列
FuncTranslator getFuncTranslator(char* typePath,char* funcHeadPath,char* valPath,char* tokenPath,char* tokenOutPath){
  FuncTranslator out={
    .fout=NULL,
    .funcTbl=NULL,
    .gloabalTypeTbl=NULL,
    .partialValTbl=NULL,
    .ValTbl=NULL
  };
  if(typePath==NULL||funcHeadPath==NULL||valPath==NULL||tokenPath==NULL||tokenOutPath==NULL) return out;
  int isRight=1;  //记录是正确的
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
  //否则参数正确,进行初始化翻译器操作
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

  //完成空间分配后释放空间
  free(typeFin);
  free(funcFin);
  free(valFin);
  return out;
}



//使用函数翻译器开始翻译
int func_translate(FuncTranslator* funcTranslator,char*){

}


//翻译结束释放函数翻译器
int release_FuncTranslator(FuncTranslator* funcTranslator){

}




//判断句子的翻译类型,以选择不同的翻译语句
FTK getTokenLineKind(TBNode* tokens){

}

//翻译功能子代码
int translateVarDef(TBNode* tokens){

}
//翻译常量定义语句
int translateConstDef(TBNode* tokens){

}

//翻译运算语句
int translateCountDef(TBNode* tokens){

}

//函数指针定义语句
int translateFuncPointerDef(TBNode* tokens){

}

//typedef命名类型别名语句
int translateTypedef(TBNode* tokens){

}

//翻译函数调用语句
int translateFuncUse(TBNode* tokens){

}

//翻译赋值语句
int translateAssign(TBNode* tokens){

}

//翻译类型方法调用语句
int translateTypeMethodUse(TBNode* tokens){
  
}




