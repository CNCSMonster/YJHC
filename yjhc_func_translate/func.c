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

char* getFuncKey(char* funcName,long long typeId){
  //使用个中间符号隔开两个内容
  int len=strlen(funcName)+20;
  char* out=malloc(len);
  sprintf(out,"%s#%lld",funcName,typeId);
  return out;
}


//根据funcKey获取func的名字，和主人,成功返回非0值，失败返回0
int extractFuncNameAndOwnerFromKey(char* funcKey,char* funcName,long long* retOwnerId){
  //读取到井号
  char end=mysgets(funcName,"#",funcKey);
  if(end!='#') return 0;
  char tmpS[200];
  end=mysgets(tmpS,"",funcName+strlen(funcName)+1);
  if(tmpS[0]=='0'&&tmpS[1]=='\0'){
    *retOwnerId=0;
    return 1;
  }
  *retOwnerId=atoll(tmpS);
  return 1;
}

