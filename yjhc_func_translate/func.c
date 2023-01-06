#include "func.h"



//��ø���̬����ռ���ַ���,��Ϊָ�����ļ�
char* getFuncKey(char* funcName,long long typeId){
  //ʹ�ø��м���Ÿ�����������
  int len=strlen(funcName)+20;
  char* out=malloc(len);
  sprintf(out,"%s#%lld",funcName,typeId);
  return out;
}

//��ø���ʼ������,
FuncTbl getFuncTbl(TypeTbl* typeTbl){
  FuncTbl funcTbl;
  funcTbl.globalTypeTbl=typeTbl;
  funcTbl.funcs=getHashTbl(100,sizeof(char*),sizeof(Func*),typeFieldNameHash,typeFieldEq);
  return funcTbl;
}

//���غ����ļ�
int loadFile_functbl(FuncTbl* funcTbl,FILE* fin){
  char tmp[1000];
  char end;
  do{
    end=myfgets(tmp,"\n",fin);
    if(strlen(tmp)==0) continue;
    if(!loadLine_functbl(funcTbl,tmp)) return 0;
  }while(end!=EOF);
  return 1;
}

//����һ���ַ���
int loadLine_functbl(FuncTbl* funcTbl,char* str){
  //���ȶ�ȡ������������
  char tmp[1000];
  Func* toAdd=malloc(sizeof(Func));
  toAdd->args=getVector(sizeof(Arg));
  toAdd->func_name=NULL;
  toAdd->owner=NULL;
  toAdd->retTypeId=0; //���retTypeIdС��0,������δ֪����
  //���ȶ�ȡ��һ�����ݺ�������
  char end=mysgets(tmp,"|",str);
  str+=strlen(tmp)+1;
  toAdd->func_name=strcpy(malloc(strlen(tmp)+1),tmp);
  end=mysgets(tmp,"|",str);
  str+=strlen(tmp)+1;
  if(strcmp(tmp,"NULL")!=0)
    toAdd->owner=strcpy(malloc(strlen(tmp)+1),tmp);
  else
    toAdd->owner=NULL;
  //��ȡ����ֵ��Ӧ���ͱ���
  end=mysgets(tmp,"|",str);
  str+=strlen(tmp)+1;
  long long retId;
  int layer;
  int typeIndex=findType(funcTbl->globalTypeTbl,tmp,&layer);
  //�����δ֪����,�����
  if(typeIndex==0){
    retId=getTypeId(typeIndex,0);
  }
  else{
    retId=getTypeId(typeIndex,layer);
  }
  toAdd->retTypeId=retId;
  //Ȼ���ò����б�  
  str++;
  // ��ȡ������)����
  do
  {
    end = mysgets(tmp, ",)", str);
    str += strlen(tmp) + 1;
    if (strlen(tmp) == 0)
      continue;
    // ���Ȱ�����ַ����ֳ���������,һ�������ǲ�����,һ�������ǲ�������
    char tmp2[100];
    char end2=mysgets(tmp2," ",tmp);
    int isConst=0;
    if(strcmp(tmp2,"const")==0){
      strcpy(tmp,tmp+strlen(tmp2)+1);
    }
    int i=strlen(tmp)-1;
    while(tmp[i]!=' '&&tmp[i]!='*') i--;
    char typeName[200];
    char oldChar=tmp[i+1];
    tmp[i+1]='\0';
    strcpy(typeName,tmp);
    refectorTypeName(typeName);
    long long typeId;
    int retLayer;
    int index=findType(funcTbl->globalTypeTbl,typeName,&retLayer);
    if(index==0){
      typeId=getTypeId(index,0);
    }
    else{
      typeId=getTypeId(index,retLayer);
    }
    tmp[i+1]=oldChar;
    char valName[200];
    strcpy(valName,tmp+i+1);
    Arg arg={
      .name=strcpy(malloc(strlen(valName)+1),valName),
      .isConst=isConst,
      .typeId=typeId
    };
    vector_push_back(&toAdd->args,&arg);
  } while (end != ')');
  //Ȼ��ע�ắ��
  //���û������
  char* key=NULL;
  if(toAdd->owner==NULL){
    key=getFuncKey(toAdd->func_name,0);
  }
  else{
    //��ȡ����id
    long long ownerId;
    int ownerLayer;
    int ownerTypeIndex=findType(funcTbl->globalTypeTbl,toAdd->owner,&ownerLayer);
    ownerId=getTypeId(ownerTypeIndex,ownerLayer);
    key=getFuncKey(toAdd->func_name,ownerId);
  }
  hashtbl_put(&funcTbl->funcs,&key,&toAdd);
  return 1;
}


//���Һ���,���ҵ����غ���ָ��,û�в��ҵ�����NULL
Func* findFunc(FuncTbl* funcTbl,char* funcName,char* owner){
  //�������˵ı��
  long long typeId;
  int retLayer;
  int typeIndex;
  if(owner!=NULL)
    typeIndex=findType(funcTbl->globalTypeTbl,owner,&retLayer);
  else{
    typeIndex=0;
  }
  if(typeIndex==0){
    typeId=getTypeId(typeIndex,0);
  }
  else{
    typeId=getTypeId(typeIndex,retLayer);
  }
  //
  char* key=getFuncKey(funcName,typeId);
  Func* out;
  if(!hashtbl_get(&funcTbl->funcs,&key,&out)) out=NULL;
  free(key);
  return out;
}

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
  for(int i=0;i<func->args.size;i++){
    Arg arg;
    vector_get(&func->args,i,&arg);
    printf("arg%d. name:%s,isConst:%d,typeId:%lld\n",i+1,arg.name,arg.isConst,arg.typeId);
  }
}


//�ͷź�����ռ�
void del_functbl(FuncTbl* funcTbl){
  //��funcs��ȡ������func�ͷ�
  char** keys=malloc(sizeof(char*)*funcTbl->funcs.size);
  Func** funcs=malloc(sizeof(Func*)*funcTbl->funcs.size);
  hashtbl_toArr(&funcTbl->funcs,keys,funcs);
  for(int i=0;i<funcTbl->funcs.size;i++){
    //TODO���������,��ɾ����ʱ��鿴��Ϣ
    // printf("keys:%s,",keys[i]);
    free(keys[i]);
    del_func(funcs[i]);
    free(funcs[i]);
    // printf("\n");
  }
  free(keys);
  free(funcs);
  hashtbl_release(&funcTbl->funcs);
}

