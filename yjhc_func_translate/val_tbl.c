#include "val_tbl.h"


//����һ������,����������ʱҪָ��ʹ�õ����ͱ�
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

//���ļ�����ȫ�ֱ�����Ϣ��
int loadFile_valtbl(ValTbl* valTbl, FILE* fin){
  //ÿ�ζ�ȡһ����Ϣ
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

//��һ������������м�������������,TODO,debug
int loadLine_valtbl(ValTbl* valTbl,char* str){
  //���ȶ�ȡ��һ����,�ж��Ƿ��ǳ����������
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
    //�Ӻ���ǰ���λ�ȡ�Ƿ���ڵĳ�ʼ��ֵ�Լ���Ӧ������
    //Ȼ���ȡ���ͺ�����
    //�����ж��Ƿ��еȺ�,����еȺ�˵���и�ֵ
    int eqId=strlen(str)-1; //��¼�Ⱥ�����
    while(eqId>=0&&str[eqId]!='=') eqId--;
    //����ҵ��Ⱥ�,��Ⱥź�����ֵ
    if(eqId>=0){
      eqId++;
      myStrStrip(str+eqId," "," ");
      toAdd.val=strcpy(malloc(strlen(str+eqId)+1),str+eqId);
      str[--eqId]='\0';
      eqId--;
    }else{
      eqId=strlen(str)-1;
    }
    //Ȼ����ǰ�Ǳ�����
    //����Խ���Ⱥ�ǰ���пո�
    while(eqId>=0&&str[eqId]==' ') str[eqId--]='\0';
    //Ȼ��Խ�����зǿո��������һ����ǰ�ĵ�һ���ո�
    while(eqId>=0&&str[eqId]!=' '&&str[eqId]!=',') eqId--;
    eqId++;
    toAdd.name=strcpy(malloc(strlen(str+eqId)+1),str+eqId);
    str[eqId]='\0';
    vector_push_back(vec,&toAdd);
    //�ж�ǰ���Ƿ��пո񶺺�
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
  //ʣ�µ�str�����¼�����������
  formatTypeName(str);  //���Ƚ���һ�¸�ʽ��
  //��������,�������û�г�ʼ���Ļ�,�����ǳ����Ļ�,��������Ĭ��ֵ
  Type type={
    .kind=UNKNOWN
  };
  //��������
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
  //Ȼ��������͵�λ��,�Ӷ��ж������Ƿ����Ѽ�������
  //Ȼ��ע����Щ�����Ͷ�Ӧ���͵Ĺ�ϵ
  for(int i=st;i<vec->size;i++){
    //ע�����������ṹ��Ĺ�ϵ
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
    //ע�����������͵Ĺ�ϵ
    char* typeName=strcpy(malloc(strlen(str)+1),str);
    char* valName=strcpy(malloc(strlen(val.name)+1),val.name);
    hashtbl_put(&valTbl->valToType,&valName,&typeName);
  }
}


//�Ӿֲ�������ʼ�Ӿֲ��������ݱ��������ұ�������Ϣ,�������������Լ�����������,ֵ����Ϣ
//���ҳɹ����ط�0ֵ,����ʧ�ܷ���0
//ͨ��retValָ�뷵��val�Ļ�����Ϣ,ͨ��retType����type�Ļ�����Ϣ,ͨ��typeLayer����type��Ӧ�Ļ���type��ָ����
//�������int* a;��䶨��ı���a
//���صĻ�������Ϊint,���ص�ָ����Ϊ0
int findVal(ValTbl* curTbl,char* valName,Val* retVal,Type* retType,int* typeLayer){
  if(curTbl==NULL) return 0;
  int id=-1;
  //���Ȳ��ұ���,�ӵ�ǰ��ʼ����������
  while(curTbl!=NULL){
    //��һ��,���ݱ������ֲ��Ҷ�Ӧ���±�
    id=strToId(curTbl->valIds,valName);
    //���idС��0,˵�������û�������
    if(id<0){
      //��֮ǰ�ı���������
      curTbl=curTbl->pre;
      continue;
    }
    else{
      break;
    }
  }
  if(id<0) return 0;
  vector_get(&curTbl->vals,id,retVal);
  //��ȡ������
  char *typeName; // ��ȡ������
  // ��ȡ������ʧ��,˵���������δ֪����
  if (!hashtbl_get(&curTbl->valToType, &valName, &typeName))
  {
    vector_get(&curTbl->typeTbl.types, 0, retType);
    return 1;
  }
  // ������������������ͱ��ڲ���
  int typeIndex = 0;
  do
  {
    int typeIndex = findType(&curTbl->typeTbl, typeName, typeLayer);
    vector_get(&curTbl->typeTbl.types, typeIndex, retType);
    curTbl = curTbl->pre;
  } while (retType->kind == TYPE_UNKNOW && curTbl != NULL);
  return 1;
}

//�����µľֲ���,�����µľֲ����ָ��,��չʧ�ܷ���NULL
ValTbl* extendValTbl(ValTbl* curTbl){
  if(curTbl==NULL) return NULL;
  ValTbl* toExtend=malloc(sizeof(ValTbl));
  (*toExtend)=getValTbl(getTypeTbl());
  curTbl->next=toExtend;
  toExtend->pre=curTbl;
  return toExtend;
}

//����ĳ���ֲ���,ע��,�þֲ���һ��Ҫ�����һ����,�����µľֲ���
ValTbl* recycleValTbl(ValTbl* partialTbl){
  if(partialTbl==NULL) return NULL;
  if(partialTbl->next!=NULL) return NULL; //����������һ���ֲ���,�򷵻�NULL,��ʾ����
  ValTbl* toRet=partialTbl->pre;  //Ҫ���ص��Ǹñ�ǰһ��
  toRet->next=NULL;
  del_valTbl(partialTbl);
  free(partialTbl);
  return toRet;
}




//��ȡһ����
Val getVal(char* name,int isConst,char* defaultVal){
  Val val;
  val.name=strcpy(malloc(strlen(name)+1),name);
  val.isConst=isConst;
  if(defaultVal==NULL) val.val=NULL;
  else val.val=strcpy(malloc(strlen(defaultVal)+1),defaultVal);
  return val;
}


//�������м���ֵ
void addVal_valtbl(ValTbl* valTbl,char* valName,char* defaultVal,const int isConst,char* typeName,int typeLayer){
  //���Ȳ�������
  Type find={ //�ṹ�����͵ľ�����ʼ��
    .kind=TYPE_UNKNOW
  };
  ValTbl* tbl=valTbl;
  //�������в���������
  while(tbl!=NULL&&typeName!=NULL){
    int index=findType(&tbl->typeTbl,typeName,NULL);
    if(index>0){
      vector_get(&tbl->typeTbl.types,index,&find);
      break;
    }
    tbl=tbl->pre;
  }
  //����������ͻ�ȡĬ��ֵ���߲�ȡĬ��ֵ
  if(defaultVal==NULL) defaultVal=defaultValueOfBaseTypes[find.kind];
  //Ȼ������val�ṹ��
  Val toAdd=getVal(valName,isConst,defaultVal);
  putStrId(valTbl->valIds,toAdd.name,valTbl->vals.size);
  vector_push_back(&valTbl->vals,&toAdd);
  //������
  //���������ΪNULL,��ʾδ֪,�򲻰�
  //���������������NULL,���
  if(typeName!=NULL){
    char* val=strcpy(malloc(strlen(valName)+1),valName);
    char* type=getTypeName(typeName,typeLayer);
    hashtbl_put(&valTbl->valToType,&val,&type);
  }
}



//�������м���һ�������Ĳ���
void loadArgs_valtbl(ValTbl* valTbl,FuncTbl* funcTbl,Func* func){
  //����ȡ������
  for(int i=0;i<func->args.size;i++){
    Arg arg;
    vector_get(&func->args,i,&arg);
    //��ȡ����������
    long long typeId=arg.typeId;
    //����ҵ�������Ϊunknown
    if(typeId==0){
      addVal_valtbl(valTbl,arg.name,NULL,arg.isConst,NULL,0);
      continue;
    }
    //��ȡ��Ӧglobal�ж�Ӧ������
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

//ͨ�������������,���ҳɹ����ط�0ֵ������ʧ�ܷ���0
int findType_valtbl(ValTbl* topValTbl,char* typeName,Type* retType,int* retLayer){
  //
  if(retType==NULL||topValTbl==NULL) return 0;
  do{
    int typeIndex=findType(&topValTbl->typeTbl,typeName,retLayer);
    if(typeIndex!=0){
      break;
    }
    topValTbl=topValTbl->pre;
  }while(topValTbl!=NULL);
  return 1;
}

//ɾ��һ������
void del_valTbl(ValTbl* valTbl){
  ValTbl* tmp=valTbl;
  // ����Ӧ��ɾ��valToType������
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
  //ɾ�������ֻ�ǵ���ɾ��һ������
}


//չʾһ����
void show_val(Val* val){
  printf("name:%s, isConst:%d, val:%s\n",val->name,val->isConst,val->val);
}

//ɾ��һ������
void delVal(Val* val){
  free(val->name);
  if(val->val!=NULL) free(val->val);
}

//��ȡ��ָ���ε�������
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
