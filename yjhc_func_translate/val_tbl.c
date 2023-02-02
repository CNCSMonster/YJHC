#include "val_tbl.h"


//创建一个量表,创建该量表时要指定使用的类型表
ValTbl getValTbl(TypeTbl typeTbl){
  ValTbl valTbl;
  valTbl.next=NULL;
  valTbl.pre=NULL;
  valTbl.vals=getVector(sizeof(Val));
  valTbl.valIds=getStrIdTable();
  valTbl.valToType=getHashTbl(100,sizeof(char*),sizeof(char*),typeFieldNameHash,typeFieldEq);
  valTbl.typeTbl=typeTbl;
  return valTbl;
}

//从文件加载全局变量信息表
int loadFile_valtbl(ValTbl* valTbl, FILE* fin){
  //每次读取一行信息
  char tmp[1000];
  char end;
  do{
    end=myfgets(tmp,"\n",fin);
    if(strlen(tmp)==0) continue;
    int jud=loadLine_valtbl(valTbl,tmp);
    if(!jud){
      return 0;
    }
  }while(end!=EOF);
  return 1;
}

//判断是否是函数指针变量定义语句,包括函数指针常量和函数指针变量两种形式,如果是返回非0值,如果不是返回0
int isFuncPointerValDef_valtbl(const char* str){
  //如果读取不到(,则一定不是函数指针定义语句
  char end;
  char tmp[1000];
  end= mysgets(tmp,"(=",str);
  if(end!='(') return 0;
  //因为如果是函数指针定义 则，格式如 fpDef 或者fpDef = val 的形式 ,其中fpDef中含有(
  return 1;
}

//加载函数指针变量定义语句,如果加载成功返回非0值,如果加载失败返回0
int loadFuncPointerValDef_valtbl(ValTbl* valTbl,const char* str){
  //首先,读取到等号位置,分割等号前后内容
  char tmp[1000];
  strcpy(tmp,str);  //首先复制下来再分析
  char* track=tmp;  
  //查找直到等号位置
  while(*track!='\0'){
    if(*track=='=') break;
    track++;
  }
  char* defaultVal=NULL;
  //则说明赋值对象不存在
  if(*track=='\0'){
    defaultVal=NULL;
  }
  else{
    *track='\0';
    track++;
    defaultVal=strcpy(malloc(strlen(track)+1),track);
    myStrStrip(defaultVal," "," "); //去除前后多余空格
  }
  //然后对默认指针值进行判断,如果有初始化值的话
  if(defaultVal!=NULL){
    if(!isLegalId(defaultVal)) return 0;
  }
  
  //到达这个位置的defaultVal都是正常的
  //接下来获取函数指针定义
  track=tmp;
  char fpName[300];
  //获取函数指针变量名
  //由提取的变量名判断是否含有const
  int isConst=0;
  if(! extractFuncPointerFieldName(track,fpName,&isConst)) return 0;
  //格式化函数指针定义,也就是获取函数指针类型
  if(!formatFuncPointerTypeName(tmp)) return 0;
  //如果获取类型和量名都成功了,则可以加入量表了
  addVal_valtbl(valTbl,fpName,defaultVal,isConst,tmp,0);
  if(defaultVal!=NULL) free(defaultVal);
  return 1;
}



//从一个量定义语句中加载量到量表中
int loadLine_valtbl(ValTbl* valTbl,char* line){
  //TODO,首先判断是否是函数指针变量定义语句,如果是,则加入函数指针
  if(isFuncPointerValDef_valtbl(line)) {
    return loadFuncPointerValDef_valtbl(valTbl,line);
  }
  char tmpStr[1000];
  strcpy(tmpStr,line);
  line=tmpStr;
  //首先读取第一个词,判断是否是常量定义语句
  char tmp[1000];
  char end=mysgets(tmp," ",line);
  int isConst=0;
  vector* vec=&valTbl->vals;
  if(strcmp(tmp,"const")==0){
    isConst=1;
    line+=strlen(tmp)+1;
  }
  int st=vec->size;
  do{
    Val toAdd;
    toAdd.isConst=isConst;
    toAdd.val=NULL;
    //从后往前依次获取是否存在的初始化值以及对应的量名
    //然后获取类型和量名
    //首先判断是否有等号,如果有等号说明有赋值
    int eqId=strlen(line)-1; //记录等号坐标
    while(eqId>=0&&line[eqId]!='=') eqId--;
    //如果找到等号,则等号后面是值
    if(eqId>=0){
      eqId++;
      myStrStrip(line+eqId," "," ");
      toAdd.val=strcpy(malloc(strlen(line+eqId)+1),line+eqId);
      line[--eqId]='\0';
      eqId--;
    }else{
      eqId=strlen(line)-1;
    }
    //然后往前是变量名
    //首先越过等号前所有空格
    while(eqId>=0&&line[eqId]==' ') line[eqId--]='\0';
    //然后越过所有非空格来到最后一个词前的第一个空格处
    while(eqId>=0&&line[eqId]!=' '&&line[eqId]!=',') eqId--;
    eqId++;
    toAdd.name=strcpy(malloc(strlen(line+eqId)+1),line+eqId);
    line[eqId]='\0';
    vector_push_back(vec,&toAdd);
    //判断前面是否还有空格逗号
    int isEnd=1;
    eqId--;
    while(line[eqId]==' '||line[eqId]==','){
      if(line[eqId]==','){
        isEnd=0;
        line[eqId]='\0';
        break;
      }
      line[eqId]='\0';
      eqId--;
    }
    if(isEnd) break;
  }while(1);
  //剩下的str就是新加入量的类型
  formatTypeName(line);  //首先进行一下格式化
  //查找类型,如果类型没有初始化的话,而且是常量的话,则设置其默认值
  Type type={
    .kind=UNKNOWN
  };
  //查找类型
  ValTbl* tmpTbl=valTbl;
  int layer;
  do{
    int index=findType(&tmpTbl->typeTbl,line,&layer);
    if(index>0){
      vector_get(&tmpTbl->typeTbl.types,index,&type);
      break;
    }
    tmpTbl=tmpTbl->pre;
  }while(tmpTbl!=NULL);
  //然后查找类型的位置,从而判断类型是否是已加载类型
  //然后注册这些变量和对应类型的关系
  for(int i=st;i<vec->size;i++){
    //注册量名与量结构体的关系
    Val val;
    vector_get(vec,i,&val);
    if(val.isConst&&val.val==NULL&&type.kind!=UNKNOWN){
      if(layer==0)
        val.val=strcpy(malloc(strlen(defaultValueOfBaseTypes[type.kind])+1),defaultValueOfBaseTypes[type.kind]);
      else{
        val.val=malloc(strlen(line)+5);
        sprintf(val.val,"(%s)0",line);
      }
      vector_set(vec,i,&val,NULL);
    }
    putStrId(valTbl->valIds,val.name,i);
    //注册量名与类型的关系
    char* typeName=strcpy(malloc(strlen(line)+1),line);
    char* valName=strcpy(malloc(strlen(val.name)+1),val.name);
    hashtbl_put(&valTbl->valToType,&valName,&typeName);
  }
}


//从局部变量表开始从局部变量根据变量名查找变量的信息,包括变量类型以及变量的名字,值等信息
//查找成功返回非0值,查找失败返回0
//通过retVal指针返回val的基本信息,通过retType返回type的基本信息,通过typeLayer返回type对应的基本type的指针层次
//比如查找int* a;语句定义的变量a
//返回的基本类型为int,返回的指针层次为0
int findVal(ValTbl* curTbl,char* valName,Val* retVal,Type* retType,int* typeLayer){
  if(curTbl==NULL) return 0;
  long long id=-1;
  //首先查找变量,从当前表开始往上逐层查找
  while(curTbl!=NULL){
    //第一步,根据变量名字查找对应的下标
    id=strToId(curTbl->valIds,valName);
    //如果id小于0,说明这个表没有这个量
    if(id<0){
      //往之前的表查找这个量
      curTbl=curTbl->pre;
      continue;
    }
    else{
      break;
    }
  }
  if(id<0) return 0;
  vector_get(&curTbl->vals,id,retVal);
  //获取类型名
  char *typeName; // 获取类型名
  // 获取类型名失败,说明加入的是未知类型
  if (!hashtbl_get(&curTbl->valToType, &valName, &typeName))
  {
    *typeLayer=0;
    vector_get(&curTbl->typeTbl.types, 0, retType);
    return 1;
  }
  //又或者类型名是函数指针类型名,说明该名字的量为函数指针变量,则查找一般变量过程失败
  if(isFuncPointerType(typeName)) return 0;

  // 否则根据类型名在类型表内查找
  int typeIndex = 0;
  do
  {
    int typeIndex = findType(&curTbl->typeTbl, typeName, typeLayer);
    vector_get(&curTbl->typeTbl.types, typeIndex, retType);
    curTbl = curTbl->pre;
  } while (retType->kind == TYPE_UNKNOW && curTbl != NULL);
  return 1;
}

//插入新的局部表,返回新的局部表的指针,扩展失败返回NULL
ValTbl* extendValTbl(ValTbl* curTbl){
  if(curTbl==NULL) return NULL;
  ValTbl* toExtend=malloc(sizeof(ValTbl));
  (*toExtend)=getValTbl(getTypeTbl());
  curTbl->next=toExtend;
  toExtend->pre=curTbl;
  return toExtend;
}

//回收某个局部表,注意,该局部表一定要是最后一个表,返回新的局部表
ValTbl* recycleValTbl(ValTbl* partialTbl){
  if(partialTbl==NULL) return NULL;
  if(partialTbl->next!=NULL) return NULL; //如果不是最后一个局部表,则返回NULL,提示错误
  ValTbl* toRet=partialTbl->pre;  //要返回的是该表前一个
  toRet->next=NULL;
  del_valTbl(partialTbl);
  free(partialTbl);
  return toRet;
}




//获取一个量
Val getVal(char* name,int isConst,char* defaultVal){
  Val val;
  val.name=strcpy(malloc(strlen(name)+1),name);
  val.isConst=isConst;
  if(defaultVal==NULL) val.val=NULL;
  else val.val=strcpy(malloc(strlen(defaultVal)+1),defaultVal);
  return val;
}


//往量表中加入值
void addVal_valtbl(ValTbl* valTbl,char* valName,char* defaultVal,const int isConst,char* typeName,int typeLayer){
  //typeName先暂存到一个位置,
  char tmpTypeName[1000];
  strcpy(tmpTypeName,typeName);
  typeName=tmpTypeName;
  //s首先判断是否是未确定类型量,如果是则不绑定类型,只是加入量
  int isfpt=isFuncPointerType(typeName);
  if(typeName==NULL||isfpt){
    if(typeName==NULL) defaultVal=NULL;
    Val toAdd=getVal(valName,isConst,defaultVal); //函数指针变量的默认值可能是函数名,要看定义的时候是否有初始化
    putStrId(valTbl->valIds,toAdd.name,valTbl->vals.size);
    vector_push_back(&valTbl->vals,&toAdd);
  }
  //然后判断是否是函数类型
  if(isfpt){
    //加入类型
    char tmp[1000];
    strcpy(tmp,typeName);
    //如果格式化类型名成功,加入类型
    if(formatTypeName(tmp)){
      char* val=strcpy(malloc(strlen(valName)+1),valName);
      char* type=strcpy(malloc(strlen(tmp)+1),tmp);
      hashtbl_put(&valTbl->valToType,&val,&type);
    }
  }
  if(typeName==NULL||isfpt) return;
  //首先查找类型
  Type find={ //结构体类型的具名初始化
    .kind=TYPE_UNKNOW
  };
  ValTbl* tbl=valTbl;
  //在量表中查找类型名
  while(tbl!=NULL&&typeName!=NULL){
    int index=findType(&tbl->typeTbl,typeName,NULL);
    if(index>0){
      vector_get(&tbl->typeTbl.types,index,&find);
      break;
    }
    tbl=tbl->pre;
  }
  //否则根据类型获取默认值或者采取默认值
  if(defaultVal==NULL) defaultVal=defaultValueOfBaseTypes[find.kind];
  //然后生成val结构体
  Val toAdd=getVal(valName,isConst,defaultVal);
  putStrId(valTbl->valIds,toAdd.name,valTbl->vals.size);
  vector_push_back(&valTbl->vals,&toAdd);
  //绑定类型
  if(find.kind!=TYPE_UNKNOW&&find.kind!=TYPE_FUNC_POINTER){
    //如果类型不是未知的,绑定类型
    char* val=strcpy(malloc(strlen(valName)+1),valName);
    char* type=getTypeName(typeName,typeLayer);
    hashtbl_put(&valTbl->valToType,&val,&type);
  }
  else if(find.kind==TYPE_UNKNOW){
    //TODO

  }
  else if(find.kind==TYPE_FUNC_POINTER){
    //TODO
    //绑定古老的指针类型
  }
}



//往量表中加载一个函数的参数
void loadArgs_valtbl(ValTbl* valTbl,FuncTbl* funcTbl,Func* func){
  //依次取出参数
  for(int i=0;i<func->args.size;i++){
    Arg arg;
    vector_get(&func->args,i,&arg);
    //获取参数的类型
    long long typeId=arg.typeId;
    //如果找到的类型为unknown
    if(typeId==0){
      addVal_valtbl(valTbl,arg.name,NULL,arg.isConst,NULL,0);
      continue;
    }
    //获取对应global中对应的内容
    int typeIndex;
    int typeLayer;
    extractTypeIndexAndPointerLayer(typeId,&typeIndex,&typeLayer);
    Type type;
    vector_get(&funcTbl->globalTypeTbl->types,typeIndex,&type);
    char* typeName=malloc(strlen(type.defaultName)+typeLayer+1);
    strcpy(typeName,type.defaultName);
    for(int i=strlen(typeName);i<typeLayer;i++){
      typeName[i]='*';
    }
    typeName[strlen(type.defaultName)+typeLayer]='\0';
    addVal_valtbl(valTbl,arg.name,NULL,arg.isConst,typeName,0);
    free(typeName);
  }
}

//通过量表查找类型,查找成功返回非0值，查找失败返回0
int findType_valtbl(ValTbl* topValTbl,char* typeName,Type* retType,int* retLayer){
  //
  if(retType==NULL||topValTbl==NULL) return 0;
  int typeIndex;
  do{
    typeIndex=findType(&topValTbl->typeTbl,typeName,retLayer);
    if(typeIndex!=0){
      break;
    }
    topValTbl=topValTbl->pre;
  }while(topValTbl!=NULL);
  if(topValTbl==NULL) return 0;
  vector_get(&topValTbl->typeTbl.types,typeIndex,retType);
  return 1;
}

//从量表中查找函数指针名,返回的结果为对应的函数指针的名字和类型,通过val返回其量属性,通过func返回其类型属性
//查找成功返回1,查找失败返回0
int findFuncPointer_valtbl(ValTbl* topValTbl,char* fpName,Val* val,char* retTypeName,vector* args){
  //TODO
  if(topValTbl==NULL) return 0;
  long long id=-1;
  //首先查找变量,从当前表开始往上逐层查找
  while(topValTbl!=NULL){
    //第一步,根据变量名字查找对应的下标
    id=strToId(topValTbl->valIds,fpName);
    //如果id小于0,说明这个表没有这个量
    if(id<0){
      //往之前的表查找这个量
      topValTbl=topValTbl->pre;
      continue;
    }
    else{
      break;
    }
  }
  if(id<0) return 0;
  vector_get(&topValTbl->vals,id,val);
  //获取类型名
  char *typeName; // 获取类型名
  // 获取类型名失败,说明加入的是未知类型,说明不存在该名字的函数指针量
  if (!hashtbl_get(&topValTbl->valToType, &fpName, &typeName))
  {
    return 0;
  }
  //然后判断类型名是否是函数指针类型名
  if(!isFuncPointerType(typeName)){
    return 0;
  }
  vector_clear(args);
  if(args->valSize!=sizeof(char*)) {
    //如果传入参数异常,TODO
    return 0;
  }
  //否则是正常的args,进行获取信息
  vector_clear(args); //首先清掉args里的内容
  //然后加载类型信息里的内容
  if(!extractRetTypeNameAndArgTypes(typeName,retTypeName,args)) return 0;
  return 1;
}



//删除一个量表
void del_valTbl(ValTbl* valTbl){
  ValTbl* tmp=valTbl;
  // 首先应该删除valToType的内容
  char **keys = malloc(sizeof(char *) * tmp->valToType.size);
  char **vals = malloc(sizeof(char *) * tmp->valToType.size);
  hashtbl_toArr(&tmp->valToType, keys, vals);
  for (int i = 0; i < tmp->valToType.size; i++)
  {
    free(keys[i]);
    free(vals[i]);
  }
  free(keys);
  free(vals);
  hashtbl_release(&tmp->valToType);
  delStrIdTable(tmp->valIds);
  delTypeTbl(&tmp->typeTbl);
  for (int i = 0; i < tmp->vals.size; i++)
  {
    Val toDel;
    vector_get(&tmp->vals, i, &toDel);
    delVal(&toDel);
  }
  vector_clear(&tmp->vals);
  //删除量表就只是单独删除一个量表
}


//展示一个量
void show_val(Val* val){
  printf("name:%s, isConst:%d, val:%s\n",val->name,val->isConst,val->val);
}

//删除一个变量
void delVal(Val* val){
  free(val->name);
  if(val->val!=NULL) free(val->val);
}

//获取带指针层次的类型名
char* getTypeName(const char* baseTypeName,int layer){
  char* out=malloc(strlen(baseTypeName)+1+layer);
  strcpy(out,baseTypeName);
  if(layer<0){
    fprintf(stderr,"error layer arguement to getTypeName()!\n");
    layer=0;
  }
  char* track=out+strlen(out);
  for(int i=0;i<layer;i++){
    strcpy(track,"*");
    track++;
  }
  return out;
}
