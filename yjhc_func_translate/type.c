#include "type.h"


//���ļ��ж�ȡ�������ͱ�,ע��,���ͱ��ǰ������Ҫ����Ϊ��������
TypeTbl getTypeTbl(FILE* fin){
  TypeTbl out;
  out.strIds=getStrIdTable();
  out.size=0;
  out.types=NULL;
  //��ȡtypeTbl���������
  vector vec=getVector(sizeof(Type));
  //�ȼ����δ֪���ͷ��ڵײ�
  Type unknownType;
  unknownType.fields=getHashTbl(0,sizeof(char*),sizeof(char*),typeFieldNameHash,typeFieldEq);
  unknownType.kind=UNKNOWN;
  unknownType.funcs=getStrSet(myStrHash);  //����ʹ�õĹ�ϣ����ΪNULL,��Ϊ�����õ�
  vector_push_back(&vec,&unknownType);
  //���������������
  for(int i=0;i<sizeof(baseTypeNames)/sizeof(baseTypeNames[0]);i++){
    
    if(baseTypeNames[i]==NULL) continue;
    Type toAdd;
    toAdd.kind=i;
    toAdd.funcs=getStrSet(myStrHash);
    toAdd.fields=getHashTbl(0,sizeof(char*),sizeof(char*),typeFieldNameHash,typeFieldEq);
    //�Ž�ȥ���ַ���Ҫ��̬����ռ�
    char* key=strcpy(malloc(strlen(baseTypeNames[i])+1),baseTypeNames[i]);
    putStrId(out.strIds,key,getTypeId(vec.size,0));
    vector_push_back(&vec,&toAdd);
  }
  int isErr=0;  //������¼��ȡ�������Ƿ�����쳣
  do{
    //�����ж��Ƿ���typedef,�����һλ��typedef,����滹Ҫ��ȡ����
    char tmp[2000];
    char end=myfgets(tmp,"\n",fin);
    if(end==EOF&&tmp[0]=='\0') break;
    if(strlen(tmp)==0) continue;
    char tmp2[100];
    end=mysgets(tmp2," ",tmp);
    if(strcmp(tmp2,"typedef")==0){
      //����ȶ��������ţ�˵����������
      //����ȶ����ֺ�,˵���Ƕ���֪���ͽ���������
      end=mysgets(tmp2,"{",tmp);
      char* track=tmp+strlen("typedef")+1;
      //���û��{��,˵������typedef�������Ǹ���֪���������
      if(end=='\0'){
        char oldName[400];
        char newName[400];
        //��ȡ����,Ȼ���ȡ����
        int last=strlen(tmp)-1;
        //���ȴӺ���ǰ��ȡ�����пո�������һ���ǿո���ַ���
        while(tmp[last]==' ') last--;
        tmp[last+1]='\0';
        //������ǰ�ҵ����һ������ǰ���һ���ո�
        while(tmp[last]!=' ') last--;
        //���λ�������������������
        strcpy(newName,tmp+last+1);
        tmp[last]='\0';
        strcpy(oldName,tmp+strlen("typedef")+1);
        refectorTypeName(oldName);  //��ʽ���ɵ�����
        //Ȼ����Ҿɵ����ֶ�Ӧ��id�Ƿ����
        int id=strToId(out.strIds,oldName);
        //�������-1,˵�����id������,����Ϊ��Ӧ��unknownid
        if(id<0){
          putStrId(out.strIds,oldName,0); //���ð󶨸��ַ�Ϊ0�±�
          id=0;
        }
        //������ص�id�ǷǸ���,��˵������,���µ����ֵ��ɵ����ֶ�Ӧ��id��
        putStrId(out.strIds,newName,id);
      }
      //����{��,��{ǰ������Ϊ���ͱ���
      else if(end=='{'){
        //���ȶ�ȡ��������ǰ��,�õ����͵ı���
        char baseName[400];
        end=mysgets(baseName,"{",track);
        track+=strlen(baseName)+1;
        refectorTypeName(baseName);
        //Ȼ��ע���������
        putStrId(out.strIds,baseName,getTypeId(vec.size,0));
        //Ȼ���ж�����,��ȡ�����������Ϣ
        mysgets(tmp2," ",baseName);
        Type type1; //׼��һ��type
        char typeStr[800];
        end=mysgets(typeStr,"}",track);
        if(end!='}'){
          isErr=1;
          break;
        }
        track+=strlen(typeStr)+1;
        if(strcmp(tmp2,"enum")==0){
          //��ȡ�������м�enum����Ϣ
          type1=extractEnum(typeStr);
        }
        else if(strcmp(tmp2,"struct")==0){
          //��ȡ�������м�struct����Ϣ
          type1=extractStruct(typeStr);
        }
        else if(strcmp(tmp2,"union")==0){
          //��ȡ�����ż�union����Ϣ
          type1=extractUnion(typeStr);
        }
        else{
          isErr=1;
          break;
        }
        //Ȼ���ȡtype�ı����Ϳ��ܵ�ָ�����,ֱ��������ͷΪֹ
        while((end=mysgets(tmp2,",",track))!='\0'){
          //�����ж�ǰ����ٸ�*��,�ж��Ƕ��ٲ�ı���
          //���ȶ�ȡǰ��*������
          int layer=0;
          int i=0;
          while(tmp2[i]!='\0'&&(tmp2[i]==' '||tmp2[i]=='*')){
            if(tmp2[i]=='*') layer++;
            i++;
          }
          //��ʽ������
          refectorTypeName(tmp2+i);
          //Ȼ��ע��
          int id=getTypeId(vec.size,layer);
          putStrId(out.strIds,tmp2+i,id);
          track+=strlen(tmp2)+1;
        }
        //�������һ������
        if(strlen(tmp2)!=0){
          int layer=0;
          int i=0;
          while(tmp2[i]!='\0'&&(tmp2[i]==' '||tmp2[i]=='*')){
            if(tmp2[i]=='*') layer++;
            i++;
          }
          //��ʽ������
          refectorTypeName(tmp2+i);
          //Ȼ��ע��
          int id=getTypeId(vec.size,layer);
          putStrId(out.strIds,tmp2+i,id);
          track+=strlen(tmp2)+1;
        }
        vector_push_back(&vec,&type1);
      }
    }
    //�����ж��Ƿ��ǽṹ��,Ȼ���ж��Ƿ�����ͨ���Ͷ���
    else{
      char name[300];
      end=mysgets(name,"{",tmp);
      char* typeStr=tmp+strlen(name)+1;
      refectorTypeName(name);
      putStrId(out.strIds,name,getTypeId(vec.size,0));
      char* end=tmp;
      while(end<tmp+strlen(tmp)&&*end!='}') end++;
      *end='\0';
      Type toAdd;
      if(strcmp(tmp2,"enum")==0){
        toAdd=extractEnum(typeStr);
      }
      else if(strcmp(tmp2,"struct")==0){
        toAdd=extractStruct(typeStr);
      }
      //Ȼ���ж��Ƿ���
      else if(strcmp(tmp2,"union")==0){
        toAdd=extractUnion(typeStr);
      }
      else{
        isErr=1;
        break;
      }
      vector_push_back(&vec,&toAdd);
    }
    //���ǰ��ȡһλ�ж��Ƿ��ȡ����
    char c=fgetc(fin);
    if(c==EOF) break;
    else ungetc(c,fin);
  }while(1);
  //������������г����쳣,ɾ��
  if(isErr){
    printf("err in get typeTbl\n");
    //ɾ��ÿ��type
    Type* types=vector_toArr(&vec);
    for(int i=0;i<vec.size;i++) delType(types+i);
    free(types);
    vector_clear(&vec);
    return out;
  }
  out.types=vector_toArr(&vec);
  out.size=vec.size;
  vector_clear(&vec); //ɾ���������ݵı���
  return out;
}

//���ݴ�strid��ȡ����long long id�ֽ����Ӧ��kind�±��Լ�ָ��ά��
//ָ��ά��Ϊ0��ʾ����������ͱ���
//long longǰ32λΪ��Ӧ����type���±�,��32λΪ������Ͷ�Ӧ��ָ����
//long long
void getTypeIndexAndPointerLayer(long long code,int* typeIndex,int* pointerLayer ){
  *typeIndex=code>>32;
  *pointerLayer=(code<<32)>>32;
}

//����kindIndex��pointerLayer��ö�Ӧ��long long����
long long getTypeId(int typeIndex,int pointerLayer){
  long long out;
  out=0;
  out+=((long long)typeIndex)<<32;
  out+=pointerLayer;
  return out;
}


int typeFieldNameHash(const void* name){
  return myStrHash(*(char**) name);
}

int typeFieldEq(const void* name1,const void* name2){
  char* s1=*(char**)name1;
  char* s2=*(char**)name2;
  return strcmp(s1,s2)==0?1:0;
}

//չʾtype
void showType(Type* type){
  //ȡ�����б������������Ա��ڴ�ӡ
  char** fields=malloc(sizeof(char*)*type->fields.size);
  char** fieldTypes=malloc(sizeof(char*)*type->fields.size);
  hashtbl_toArr(&type->fields,fields,fieldTypes);
  printf("fields:\n");
  for(int i=0;i<type->fields.size;i++){
    printf("%s",fields[i]);
    if(fieldTypes[i]!=NULL)
      printf(" %s",fieldTypes[i]);
    printf("\n");
  }
  printf("funcs:\n");
  char** funcs=toStrArr_StrSet(&type->funcs);
  for(int i=0;i<type->funcs.num;i++){
    printf("%s\n",funcs[i]);
    free(funcs[i]);
  }
  free(funcs);
}

//extract enum,���extractʧ��,���ظ��ͷ��˿ռ��out
Type extractEnum(char* str){
  Type out;
  out.kind=TYPE_ENUM;
  out.fields=getHashTbl(10,sizeof(char*),sizeof(char*),typeFieldNameHash,typeFieldEq);
  out.funcs=getStrSet(myStrHash);
  //enum����ֻ���г�Ա
  char tmp[1000];
  char end;
  int isWrong=0;
  do{
    end=mysgets(tmp," ,",str);
    str+=strlen(tmp)+1;
    if(strlen(tmp)==0) continue;
    char* toAdd=strcpy(malloc(strlen(tmp)+1),tmp);
    hashtbl_put(&out.fields,&toAdd,NULL);  //��õ�������
  }while(end!='\0');
  if(isWrong){
    printf("gg");
    delType(&out);
  }
  return out;
}

//extract union
Type extractUnion(char* str){
  Type out;
  out.kind=TYPE_UNION;
  out.fields=getHashTbl(30,sizeof(char*),sizeof(char*),typeFieldNameHash,typeFieldEq);
  out.funcs=getStrSet(myStrHash);
  //enum����ֻ���г�Ա
  char tmp[1000];
  char end;
  int isWrong=0;
  while(*str==' '&&*str!='\0') str++;
  do{
    end=mysgets(tmp,";",str);
    str+=strlen(tmp)+1;
    while(*str==' '&&*str!='\0') str++;
    int len=strlen(tmp);
    if(len==0) continue;
    //���ĩβ������
    while(tmp[strlen(tmp)-1]==' ') tmp[strlen(tmp)-1]='\0';
    //����Ǻ���
    if(tmp[strlen(tmp)-1]==')'){
      //�����ҵ���߶�Ӧ������
      int right=1;  //��¼����������
      char* first=tmp+strlen(tmp)-2;
      while(first>=tmp){
        if(*first==')') right++;
        else if(*first=='(') right--;
        if(right==0) break;
        first--;
      }
      if(first<=tmp) 
        isWrong=1;
      else{
        char* left=first-1;
        while(left>=tmp&&*left!=' ') left--;
        left++;
        (*first)='\0';
        //�����ҵ���Ӧ������
        if(left<first){
          addStr_StrSet(&out.funcs,left);
        }
      }
    }
    //���������鶨��,��������鵳����һ��ָ�봦��
    else if(tmp[strlen(tmp)-1]==']'){
      int i=strlen(tmp)-2;
      int rightBracket=1;
      int pointerLayer=0;
      while(tmp[i]==' '||rightBracket!=0||tmp[i]==']'){
        if(tmp[i]==']'&&rightBracket==0) rightBracket=1;
        else if(tmp[i]=='['&&rightBracket==1){
          rightBracket=0;
          pointerLayer++;
        }
        i--;
        continue;
      }
      //��ȡ����
      //���ȶ�������
      char* fieldName;
      char* fieldType;
      tmp[i+1]='\0';
      //Ȼ����ȡ������
      i=strlen(tmp)-1;
      while (tmp[i]==' ') i--;
      tmp[i+1]='\0';
      while(tmp[i]!=' ') i--;
      fieldName=strcpy(malloc(strlen(tmp+i+1)+1),tmp+i+1);
      tmp[i]='\0';
      //��ʽ����������
      refectorTypeName(tmp);
      fieldType=strcpy(malloc(strlen(tmp)+1+pointerLayer),tmp);
      char* ts=fieldType+strlen(fieldType);
      for(int j=0;j<pointerLayer;j++){
        *ts='*';
        ts++;
      }
      *ts='\0';
      //�����������������
      hashtbl_put(&out.fields,&fieldName,&fieldType);
    }
    //���������Զ���
    else{
      //���ȶ�������
      char* fieldName;
      char* fieldType;
      //Ȼ����ȡ������
      int i=strlen(tmp)-1;
      while (tmp[i]==' ') i--;
      tmp[i+1]='\0';
      while(tmp[i]!=' ') i--;
      fieldName=strcpy(malloc(strlen(tmp+i+1)+1),tmp+i+1);
      tmp[i]='\0';
      //��ʽ��������
      refectorTypeName(tmp);
      fieldType=strcpy(malloc(strlen(tmp)+1),tmp);
      //�����������������
      hashtbl_put(&out.fields,&fieldName,&fieldType);
    }
  }while(end!='\0');
  if(isWrong){
    printf("gg");
    delType(&out);
  }
  return out;
}

//extract struct
Type extractStruct(char* str){
  Type out;
  out.kind=TYPE_STRUCT;
  out.fields=getHashTbl(30,sizeof(char*),sizeof(char*),typeFieldNameHash,typeFieldEq);
  out.funcs=getStrSet(myStrHash);
  //enum����ֻ���г�Ա
  char tmp[1000];
  char end;
  int isWrong=0;
  while(*str==' '&&*str!='\0') str++;
  do{
    end=mysgets(tmp,";",str);
    str+=strlen(tmp)+1;
    while(*str==' '&&*str!='\0') str++;
    int len=strlen(tmp);
    if(len==0) continue;
    //���ĩβ������
    while(tmp[strlen(tmp)-1]==' ') tmp[strlen(tmp)-1]='\0';
    //����Ǻ���
    if(tmp[strlen(tmp)-1]==')'){
      //�����ҵ���߶�Ӧ������
      int right=1;  //��¼����������
      char* first=tmp+strlen(tmp)-2;
      while(first>=tmp){
        if(*first==')') right++;
        else if(*first=='(') right--;
        if(right==0) break;
        first--;
      }
      if(first<=tmp) 
        isWrong=1;
      else{
        char* left=first-1;
        while(left>=tmp&&*left!=' ') left--;
        left++;
        (*first)='\0';
        //�����ҵ���Ӧ������
        if(left<first){
          addStr_StrSet(&out.funcs,left);
        }
      }
    }
    //���������鶨��,��������鵳����һ��ָ�봦��
    else if(tmp[strlen(tmp)-1]==']'){
      int i=strlen(tmp)-2;
      int rightBracket=1;
      int pointerLayer=0;
      while(tmp[i]==' '||rightBracket!=0||tmp[i]==']'){
        if(tmp[i]==']'&&rightBracket==0) rightBracket=1;
        else if(tmp[i]=='['&&rightBracket==1){
          rightBracket=0;
          pointerLayer++;
        }
        i--;
        continue;
      }
      //��ȡ����
      //���ȶ�������
      char* fieldName;
      char* fieldType;
      tmp[i+1]='\0';
      //Ȼ����ȡ������
      i=strlen(tmp)-1;
      while (tmp[i]==' ') i--;
      tmp[i+1]='\0';
      while(tmp[i]!=' ') i--;
      fieldName=strcpy(malloc(strlen(tmp+i+1)+1),tmp+i+1);
      tmp[i]='\0';
      //��ʽ����������
      refectorTypeName(tmp);
      fieldType=strcpy(malloc(strlen(tmp)+1+pointerLayer),tmp);
      char* ts=fieldType+strlen(fieldType);
      for(int j=0;j<pointerLayer;j++){
        *ts='*';
        ts++;
      }
      *ts='\0';
      //�����������������
      hashtbl_put(&out.fields,&fieldName,&fieldType);
    }
    //���������Զ���
    else{
      //���ȶ�������
      char* fieldName;
      char* fieldType;
      //Ȼ����ȡ������
      int i=strlen(tmp)-1;
      while (tmp[i]==' ') i--;
      tmp[i+1]='\0';
      while(tmp[i]!=' ') i--;
      fieldName=strcpy(malloc(strlen(tmp+i+1)+1),tmp+i+1);
      tmp[i]='\0';
      //��ʽ��������
      refectorTypeName(tmp);
      fieldType=strcpy(malloc(strlen(tmp)+1),tmp);
      //�����������������
      hashtbl_put(&out.fields,&fieldName,&fieldType);
    }
  }while(end!='\0');
  if(isWrong){
    printf("gg");
    delType(&out);
  }
  return out;
}


//��ʽ�������ַ���
void refectorTypeName(char* str){
  vector vec=getVector(sizeof(char));
  int i=0;
  //����Խ��ǰ�����пո�
  while(str[i]==' ') i++;
  //��ȥ��������пո�
  while(str[strlen(str)-1]==' ') str[strlen(str)-1]='\0';
  for(;i<strlen(str);i++){
    //ֻд��һ���ո�
    if(str[i]==' '){
      vector_push_back(&vec,&str[i]);
      while(i+1<strlen(str)&&str[i+1]==' ') i++;
    }
    else if(str[i]=='*') break;
    else vector_push_back(&vec,&str[i]);
  }
  if(str[i]=='*'){
    char c;
    vector_get(&vec,vec.size-1,&c);
    if(c==' ') vector_set(&vec,vec.size-1,&str[i],NULL);
    else vector_push_back(&vec,&str[i]);
    i++;
  }
  while (i<strlen(str))
  {
    if(str[i]==' ') i++;
    else if(str[i]=='*') vector_push_back(&vec,&str[i]);
    else break; //�������쳣��ʽ,ֻ��ȡǰ�沿��
  }
  char c='\0';
  vector_push_back(&vec,&c);
  char* s=vector_toArr(&vec);
  strcpy(str,s);
  free(s);
  vector_clear(&vec);
}



//�����������ֲ�ѯһ������,���ظ����������ͱ��е��±�
int findType(TypeTbl* tbl,char* typeName,int* layerRet){
  long long id;
  int typeIndex;
  int pLayer;
  //����ֱ�����Ż�ȡ��κ��±�
  id=strToId(tbl->strIds,typeName);
  if(id>=0){
    getTypeIndexAndPointerLayer(id,&typeIndex,&pLayer);
    *layerRet=pLayer;
    return typeIndex;
  }

  //��ȡ�������ȡ�������ֵĸ�ʽ��
  char* ts=strcpy(malloc(strlen(typeName)+1),typeName);
  refectorTypeName(ts);
  int i=strlen(ts)-1;
  //Ȼ��ͳ��ָ���Σ��Լ��ҵ�ԭʼ����
  int pointerLayer=0;
  while(ts[i]=='*'){
    pointerLayer++;
    i--;
  }
  ts[i+1]='\0';
  id=strToId(tbl->strIds,ts);
  ts[i+1]='*';
  getTypeIndexAndPointerLayer(id,&typeIndex,&pLayer);
  pLayer+=pointerLayer;
  id=getTypeId(typeIndex,pLayer);
  putStrId(tbl->strIds,ts,id);
  putStrId(tbl->strIds,typeName,id);
  free(ts);
  *layerRet=pLayer;
  return typeIndex;
}



//���һ��type����������
void delType(Type* type){
  initStrSet(&type->funcs);
  hashtbl_release(&type->fields);
}


//ɾ�����ͱ�
int delTypeTbl(TypeTbl* tbl){
  delStrIdTable(tbl->strIds);
  free(tbl->strIds);
  for(int i=0;i<tbl->size;i++){
    delType(&tbl->types[i]);
  }
  free(tbl->types);
  return 1;
}



