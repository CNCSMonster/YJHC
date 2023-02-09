#include "func_tbl.h"




//��ø���ʼ������,
FuncTbl getFuncTbl(TypeTbl* typeTbl){
  FuncTbl funcTbl;
  funcTbl.funcKeys=getVector(sizeof(char*)); //���溯�����ֵ�˳���
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
    Arg arg;
    extractArgFromLine(funcTbl->globalTypeTbl,&arg,tmp);
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
  //���ַ���Ҳ�ӵ�funcName����
  vector_push_back(&funcTbl->funcKeys,&key);
  return 1;
}

//��ȡĳ����Ա�����ĸ�������
Func* get_rename_member_func(FuncTbl* funcTbl,Func* func){
  //TODO
  if(func->owner==NULL) return NULL;
  //��ȡ���˺�����
  Type ownerType;
  int layer;
  if(!findType(funcTbl->globalTypeTbl,func->owner,&layer)){
    return NULL;
  }
  if(layer!=0) return 0;
  //��ȡ����
  Func* out=malloc(sizeof(Func));
  out->args=getVector(sizeof(Arg));
  out->owner=NULL;
  out->retTypeId=func->retTypeId;
  out->func_name=yjhc_rename_member_method(func->func_name,func->owner);
  Arg targ;
  targ.isConst=0;
  targ.name=strcpy(malloc(strlen(SELF_STRING_VALUE)+1),SELF_STRING_VALUE);
  //����onwerid
  int selfIndex;
  int selfLayer;
  selfIndex=findType(funcTbl->globalTypeTbl,func->owner,&selfLayer);
  if(selfLayer!=0){
    free(targ.name);
    del_func(out);
    return NULL;
  }
  targ.typeId=getTypeId(selfIndex,1);
  vector_push_back(&out->args,&targ);
  //Ȼ�����ԭ�������ֲ���
  for(int i=0;i<func->args.size;i++){
    Arg old;
    vector_get(&func->args,i,&old);
    Arg new;
    new.name=strcpy(malloc(strlen(old.name)+1),old.name);
    new.typeId=old.typeId;
    new.isConst=old.isConst;
    vector_push_back(&out->args,&new);
  }
  return out;
}

//��һ���ַ���������ȡ����������Ϣ,ʹ�õ�һ�����ͱ�
int extractArgFromLine(TypeTbl *typeTbl, Arg *retArg, char* argStr)
{
  // ���Ȱ�����ַ����ֳ���������,һ�������ǲ�����,һ�������ǲ�������
  char tmp2[100];
  char end2 = mysgets(tmp2, " ", argStr);
  int isConst = 0;
  if (strcmp(tmp2, "const") == 0)
  {
    strcpy(argStr, argStr + strlen(tmp2) + 1);
    isConst=1;
  }
  int i = strlen(argStr) - 1;
  //Ȼ����ǰ�ƶ�Խ��������
  while (argStr[i] != ' ' && argStr[i] != '*')
    i--;
  char typeName[200];
  char oldChar = argStr[i + 1];
  argStr[i + 1] = '\0';
  strcpy(typeName, argStr);
  formatTypeName(typeName);
  long long typeId;
  int retLayer;
  int index = findType(typeTbl, typeName, &retLayer);
  argStr[i + 1] = oldChar;
  char valName[200];
  strcpy(valName, argStr + i + 1);
  i=strlen(valName)-1;
  int baseLayer=0;  //���ǵ�������������
  if(valName[i]==']'){
    int rightBracketNum=1;
    valName[i]='\0';
    i--;
    while(i>=0&&(rightBracketNum!=0||((valName[i]==']'||valName[i]==' ')&&rightBracketNum==0))){
      if(valName[i]=='['){
        baseLayer++;
        rightBracketNum--;
      }
      else if(valName[i]==']') rightBracketNum++;
      i--;
    }
    valName[i+1]='\0';
  }
  retLayer+=baseLayer;
  if (index == 0)
  {
    typeId = getTypeId(index, 0);
  }
  else
  {
    typeId = getTypeId(index, retLayer);
  }
  retArg->name=strcpy(malloc(strlen(valName) + 1), valName);
  retArg->isConst=isConst;
  retArg->typeId=typeId;
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


//�ͷź�����ռ�
void del_functbl(FuncTbl* funcTbl){
  //��funcs��ȡ������func�ͷ�
  char** keys=malloc(sizeof(char*)*funcTbl->funcs.size);
  Func** funcs=malloc(sizeof(Func*)*funcTbl->funcs.size);
  hashtbl_toArr(&funcTbl->funcs,keys,funcs);
  for(int i=0;i<funcTbl->funcs.size;i++){
    //�������,��ɾ����ʱ��鿴��Ϣ
    // printf("keys:%s,",keys[i]);
    free(keys[i]);
    del_func(funcs[i]);
    free(funcs[i]);
    // printf("\n");
  }
  free(keys);
  free(funcs);
  hashtbl_release(&funcTbl->funcs);
  //�ͷź������ռ�
  vector_clear(&funcTbl->funcKeys);
}

