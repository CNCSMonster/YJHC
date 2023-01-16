#include "func.h"


void del_func(Func* func){
  // showFunc(func);
  if(func->owner!=NULL) free(func->owner);
  free(func->func_name);
  for(int i=0;i<func->args.size;i++){
    Arg arg;
    vector_get(&func->args,i,&arg);
    if(arg.name!=NULL) free(arg.name);
  }
  vector_clear(&func->args);
}

//展示一个函数
void showFunc(Func* func){
  printf("name:%s,onwer:%s\n",func->func_name,func->owner);
  //打印返回类型
  int retIndex;
  int retLayer;
  extractTypeIndexAndPointerLayer(func->retTypeId,&retIndex,&retLayer);
  printf("retTypeIndex:%d,retTypeLayer:%d\n",retIndex,retLayer);
  for(int i=0;i<func->args.size;i++){
    Arg arg;
    vector_get(&func->args,i,&arg);
    int typeIndex;
    int pointerLayer;
    extractTypeIndexAndPointerLayer(arg.typeId,&typeIndex,&pointerLayer);
    printf("arg%d. name:%s,isConst:%d,typeIndex:%d,pointerLayer:%d\n",i+1,arg.name,arg.isConst,typeIndex,pointerLayer);
  }
}
