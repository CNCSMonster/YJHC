#include "func_tbl.h"




//获得个初始函数表,
FuncTbl getFuncTbl(TypeTbl* typeTbl){
  FuncTbl funcTbl;
  funcTbl.funcKeys=getVector(sizeof(char*)); //保存函数名字的顺序表
  funcTbl.globalTypeTbl=typeTbl;
  funcTbl.funcs=getHashTbl(100,sizeof(char*),sizeof(Func*),typeFieldNameHash,typeFieldEq);
  return funcTbl;
}

//加载函数文件
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

//加载一个字符串
int loadLine_functbl(FuncTbl* funcTbl,char* str){
  //首先读取到函数的名字
  char tmp[1000];
  Func* toAdd=malloc(sizeof(Func));
  toAdd->args=getVector(sizeof(Arg));
  toAdd->func_name=NULL;
  toAdd->owner=NULL;
  toAdd->retTypeId=0; //如果retTypeId小于0,则属于未知类型
  //首先读取第一个内容函数名字
  char end=mysgets(tmp,"|",str);
  str+=strlen(tmp)+1;
  toAdd->func_name=strcpy(malloc(strlen(tmp)+1),tmp);
  end=mysgets(tmp,"|",str);
  str+=strlen(tmp)+1;
  if(strcmp(tmp,"NULL")!=0)
    toAdd->owner=strcpy(malloc(strlen(tmp)+1),tmp);
  else
    toAdd->owner=NULL;
  //获取返回值对应类型编码
  end=mysgets(tmp,"|",str);
  str+=strlen(tmp)+1;
  long long retId;
  int layer;
  int typeIndex=findType(funcTbl->globalTypeTbl,tmp,&layer);
  //如果是未知类型,则加入
  if(typeIndex==0){
    retId=getTypeId(typeIndex,0);
  }
  else{
    retId=getTypeId(typeIndex,layer);
  }
  toAdd->retTypeId=retId;
  //然后获得参数列表  
  str++;
  // 读取到遇到)结束
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
  //然后注册函数
  //如果没有主人
  char* key=NULL;
  if(toAdd->owner==NULL){
    key=getFuncKey(toAdd->func_name,0);
  }
  else{
    //获取主人id
    long long ownerId;
    int ownerLayer;
    int ownerTypeIndex=findType(funcTbl->globalTypeTbl,toAdd->owner,&ownerLayer);
    ownerId=getTypeId(ownerTypeIndex,ownerLayer);
    key=getFuncKey(toAdd->func_name,ownerId);
  }

  hashtbl_put(&funcTbl->funcs,&key,&toAdd);
  //把字符串也加到funcName里面
  vector_push_back(&funcTbl->funcKeys,&key);
  return 1;
}

//获取某个成员函数的改名函数
Func* get_rename_member_func(FuncTbl* funcTbl,Func* func){
  //TODO
  if(func->owner==NULL) return NULL;
  //获取主人函数名
  Type ownerType;
  int layer;
  if(!findType(funcTbl->globalTypeTbl,func->owner,&layer)){
    return NULL;
  }
  if(layer!=0) return 0;
  //获取复制
  Func* out=malloc(sizeof(Func));
  out->args=getVector(sizeof(Arg));
  out->owner=NULL;
  out->retTypeId=func->retTypeId;
  out->func_name=yjhc_rename_member_method(func->func_name,func->owner);
  Arg targ;
  targ.isConst=0;
  targ.name=strcpy(malloc(strlen(SELF_STRING_VALUE)+1),SELF_STRING_VALUE);
  //查找onwerid
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
  //然后加入原函数各种参数
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

//从一个字符串里面提取参数定义信息,使用到一个类型表
int extractArgFromLine(TypeTbl *typeTbl, Arg *retArg, char* argStr)
{
  // 首先把这个字符串分成两个部分,一个部分是参数名,一个部分是参数类型
  char tmp2[100];
  char end2 = mysgets(tmp2, " ", argStr);
  int isConst = 0;
  if (strcmp(tmp2, "const") == 0)
  {
    strcpy(argStr, argStr + strlen(tmp2) + 1);
    isConst=1;
  }
  int i = strlen(argStr) - 1;
  //然后往前移动越过参数名
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
  int baseLayer=0;  //考虑到传入数组的情况
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

//查找函数,查找到返回函数指针,没有查找到返回NULL
Func* findFunc(FuncTbl* funcTbl,char* funcName,char* owner){
  //查找主人的编号
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


//释放函数表空间
void del_functbl(FuncTbl* funcTbl){
  //从funcs中取出所有func释放
  char** keys=malloc(sizeof(char*)*funcTbl->funcs.size);
  Func** funcs=malloc(sizeof(Func*)*funcTbl->funcs.size);
  hashtbl_toArr(&funcTbl->funcs,keys,funcs);
  for(int i=0;i<funcTbl->funcs.size;i++){
    //调试语句,在删除的时候查看信息
    // printf("keys:%s,",keys[i]);
    free(keys[i]);
    del_func(funcs[i]);
    free(funcs[i]);
    // printf("\n");
  }
  free(keys);
  free(funcs);
  hashtbl_release(&funcTbl->funcs);
  //释放函数名空间
  vector_clear(&funcTbl->funcKeys);
}

