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

//从一个量定义语句中加载量到量表中,TODO,debug
int loadLine_valtbl(ValTbl* valTbl,char* str){
  //首先读取第一个词,判断是否是常量定义语句
  char tmp[1000];
  char end=mysgets(tmp," ",str);
  int isConst=0;
  vector* vec=&valTbl->vals;
  if(strcmp(tmp,"const")==0){
    isConst=1;
    str+=strlen(tmp)+1;
  }
  int st=vec->size;
  do{
    Val toAdd;
    toAdd.isConst=isConst;
    toAdd.val=NULL;
    //从后往前依次获取是否存在的初始化值以及对应的量名
    //然后获取类型和量名
    //首先判断是否有等号,如果有等号说明有赋值
    int eqId=strlen(str)-1; //记录等号坐标
    while(eqId>=0&&str[eqId]!='=') eqId--;
    //如果找到等号,则等号后面是值
    if(eqId>=0){
      eqId++;
      myStrStrip(str+eqId," "," ");
      toAdd.val=strcpy(malloc(strlen(str+eqId)+1),str+eqId);
      str[--eqId]='\0';
      eqId--;
    }else{
      eqId=strlen(str)-1;
    }
    //然后往前是变量名
    //首先越过等号前所有空格
    while(eqId>=0&&str[eqId]==' ') str[eqId--]='\0';
    //然后越过所有非空格来到最后一个词前的第一个空格处
    while(eqId>=0&&str[eqId]!=' '&&str[eqId]!=',') eqId--;
    eqId++;
    toAdd.name=strcpy(malloc(strlen(str+eqId)+1),str+eqId);
    str[eqId]='\0';
    vector_push_back(vec,&toAdd);
    //判断前面是否还有空格逗号
    int isEnd=1;
    eqId--;
    while(str[eqId]==' '||str[eqId]==','){
      if(str[eqId]==','){
        isEnd=0;
        str[eqId]='\0';
        break;
      }
      str[eqId]='\0';
      eqId--;
    }
    if(isEnd) break;
  }while(1);
  //然后获得vec的类型
  //剩下的str就是新加入量的类型
  refectorTypeName(str);  //首先进行一下格式化
  //查找类型,如果类型没有初始化的话,而且是常量的话,则设置其默认值
  Type type={
    .kind=UNKNOWN
  };
  //查找类型
  ValTbl* tmpTbl=valTbl;
  int layer;
  do{
    int index=findType(&tmpTbl->typeTbl,str,&layer);
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
        val.val=malloc(strlen(str)+5);
        sprintf(val.val,"(%s)0",str);
      }
      vector_set(vec,i,&val,NULL);
    }
    putStrId(valTbl->valIds,val.name,i);
    //注册量名与类型的关系,TODO
    char* typeName=strcpy(malloc(strlen(str)+1),str);
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
  //首先查找变量,从当前表开始往上逐层查找
  while(curTbl!=NULL){
    //第一步,根据变量名字查找对应的下标
    int id=strToId(curTbl->valIds,valName);
    //如果id小于0,说明这个表没有这个量
    if(id<0){
      //往之前的表查找这个量
      curTbl=curTbl->pre;
      continue;
    }
    vector_get(&curTbl->vals,id,retVal);
    //获取类型名
    char* typeName; //获取类型名
    hashtbl_get(&curTbl->valToType,&valName,&typeName);
    //从类型表查找类型,首先查找类型名,然后根据类型名查找
    int typeIndex=0;
    do{
      int typeIndex=findType(&curTbl->typeTbl,typeName,typeLayer);
      vector_get(&curTbl->typeTbl.types,typeIndex,retType);
      curTbl=curTbl->pre;
    }while(retType->kind==UNKNOWN&&curTbl!=NULL);
    return 1;
  }
  return 0; //没有查找成功
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
//其中
void addVal_valtbl(ValTbl* valTbl,char* valName,char* defaultVal,const int isConst,char* typeName){
  //首先查找类型
  Type find={ //结构体类型的具名初始化
    .kind=UNKNOWN
  };
  ValTbl* tbl=valTbl;
  do{
    int index=findType(&tbl->typeTbl,typeName,NULL);
    if(index>0){
      vector_get(&valTbl->typeTbl.types,index,&find);
      break;
    }
    tbl=tbl->pre;
  }while(tbl!=NULL);
  //否则根据类型获取默认值或者采取默认值
  if(defaultVal==NULL) defaultVal=defaultValueOfBaseTypes[find.kind];
  //然后生成val结构体
  Val toAdd=getVal(valName,isConst,defaultVal);
  putStrId(valTbl->valIds,toAdd.name,valTbl->vals.size);
  vector_push_back(&valTbl->vals,&toAdd);
  //绑定类型
  char* val=strcpy(malloc(strlen(valName)+1),valName);
  char* type=strcpy(malloc(strlen(typeName)+1),typeName);
  hashtbl_put(&valTbl->valToType,&val,&type);
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
  valTbl = tmp->next;
  if (valTbl != NULL)
  {
    valTbl->pre = tmp->pre;
  }
  if (tmp->pre != NULL)
    tmp->pre->next = valTbl;
}

//删除一个变量
void delVal(Val* val){
  free(val->name);
  if(val->val!=NULL) free(val->val);
}