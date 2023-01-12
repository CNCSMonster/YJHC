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
  //Ȼ����vec������
  //ʣ�µ�str�����¼�����������
  refectorTypeName(str);  //���Ƚ���һ�¸�ʽ��
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
    //ע�����������͵Ĺ�ϵ,TODO
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
  //���Ȳ��ұ���,�ӵ�ǰ��ʼ����������
  while(curTbl!=NULL){
    //��һ��,���ݱ������ֲ��Ҷ�Ӧ���±�
    int id=strToId(curTbl->valIds,valName);
    //���idС��0,˵�������û�������
    if(id<0){
      //��֮ǰ�ı���������
      curTbl=curTbl->pre;
      continue;
    }
    vector_get(&curTbl->vals,id,retVal);
    //��ȡ������
    char* typeName; //��ȡ������
    hashtbl_get(&curTbl->valToType,&valName,&typeName);
    //�����ͱ��������,���Ȳ���������,Ȼ���������������
    int typeIndex=0;
    do{
      int typeIndex=findType(&curTbl->typeTbl,typeName,typeLayer);
      vector_get(&curTbl->typeTbl.types,typeIndex,retType);
      curTbl=curTbl->pre;
    }while(retType->kind==UNKNOWN&&curTbl!=NULL);
    return 1;
  }
  return 0; //û�в��ҳɹ�
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
//����
void addVal_valtbl(ValTbl* valTbl,char* valName,char* defaultVal,const int isConst,char* typeName){
  //���Ȳ�������
  Type find={ //�ṹ�����͵ľ�����ʼ��
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
  //����������ͻ�ȡĬ��ֵ���߲�ȡĬ��ֵ
  if(defaultVal==NULL) defaultVal=defaultValueOfBaseTypes[find.kind];
  //Ȼ������val�ṹ��
  Val toAdd=getVal(valName,isConst,defaultVal);
  putStrId(valTbl->valIds,toAdd.name,valTbl->vals.size);
  vector_push_back(&valTbl->vals,&toAdd);
  //������
  char* val=strcpy(malloc(strlen(valName)+1),valName);
  char* type=strcpy(malloc(strlen(typeName)+1),typeName);
  hashtbl_put(&valTbl->valToType,&val,&type);
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
  valTbl = tmp->next;
  if (valTbl != NULL)
  {
    valTbl->pre = tmp->pre;
  }
  if (tmp->pre != NULL)
    tmp->pre->next = valTbl;
}

//ɾ��һ������
void delVal(Val* val){
  free(val->name);
  if(val->val!=NULL) free(val->val);
}