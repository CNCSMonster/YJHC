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

//չʾһ������
void showFunc(Func* func){
  printf("name:%s,onwer:%s\n",func->func_name,func->owner);
  //��ӡ��������
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
  //ʹ�ø��м���Ÿ�����������
  int len=strlen(funcName)+20;
  char* out=malloc(len);
  sprintf(out,"%s#%lld",funcName,typeId);
  return out;
}


//����funcKey��ȡfunc�����֣�������,�ɹ����ط�0ֵ��ʧ�ܷ���0
int extractFuncNameAndOwnerFromKey(char* funcKey,char* funcName,long long* retOwnerId){
  //��ȡ������
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

