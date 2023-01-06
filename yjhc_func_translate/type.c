#include "type.h"

//��ȡȫ�����ͱ�,�����л�����������
TypeTbl getGlobalTypeTbl(){
  TypeTbl out=getTypeTbl();
  //���������������
  for(int i=0;i<sizeof(baseTypeNames)/sizeof(baseTypeNames[0]);i++){
    if(i==TYPE_STRUCT||i==TYPE_ENUM||i==TYPE_UNION||i==TYPE_UNKNOW) continue;
    Type toAdd;
    toAdd.kind=i;
    toAdd.funcs=getStrSet(myStrHash);
    toAdd.fields=getHashTbl(0,sizeof(char*),sizeof(char*),typeFieldNameHash,typeFieldEq);
    //�Ž�ȥ���ַ���Ҫ��̬����ռ�
    char* key=strcpy(malloc(strlen(baseTypeNames[i])+1),baseTypeNames[i]);
    putStrId(out.strIds,key,getTypeId(out.types.size,0));
    vector_push_back(&out.types,&toAdd);
  }
  return out;
}



TypeTbl getTypeTbl(){
  TypeTbl out;
  out.strIds=getStrIdTable();
  out.types=getVector(sizeof(Type));
  //out�ĵ�һλ����unknown
  Type unknownType;
  unknownType.fields=getHashTbl(0,sizeof(char*),sizeof(char*),typeFieldNameHash,typeFieldEq);
  unknownType.kind=TYPE_UNKNOW;
  unknownType.funcs=getStrSet(myStrHash);  
  vector_push_back(&out.types,&unknownType);
  return out;
}


//���ļ��ж�ȡ�������ͱ�,ע��,���ͱ��ǰ������Ҫ����Ϊ��������
int loadFile_typeTbl(TypeTbl* tbl, FILE* fin){
  int isErr=0;  //������¼��ȡ�������Ƿ�����쳣
  do{
    //�����ж��Ƿ���typedef,�����һλ��typedef,����滹Ҫ��ȡ����
    char str[2000];
    char end=myfgets(str,"\n",fin);
    if(end==EOF&&str[0]=='\0') break;
    if(strlen(str)==0) continue;
    int jud=loadLine_typetbl(tbl,str);
    if(!jud){
      isErr=1;
      break;
    }
    //���ǰ��ȡһλ�ж��Ƿ��ȡ����
    char c=fgetc(fin);
    if(c==EOF) break;
    else ungetc(c,fin);
  }while(1);
  //������������г����쳣,ɾ��
  if(isErr){
    printf("err in loadFile\n");
    return 0;
  }
  return 1;
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

//���ͱ�����ַ�����������Ϣ,�ɹ����ط�0ֵ,ʧ�ܷ���0
int loadLine_typetbl(TypeTbl* tbl,char* str){
  char tmp[100];
  char end = mysgets(tmp, " ", str);
  if (strcmp(tmp, "typedef") == 0)
  {
    // ����ȶ��������ţ�˵����������
    // ����ȶ����ֺ�,˵���Ƕ���֪���ͽ���������
    end = mysgets(tmp, "{", str);
    char *track = str + strlen("typedef") + 1;
    // ���û��{��,˵������typedef�������Ǹ���֪���������
    if (end == '\0')
    {
      char oldName[400];
      char newName[400];
      // ��ȡ����,Ȼ���ȡ����
      int last = strlen(str) - 1;
      // ���ȴӺ���ǰ��ȡ�����пո�������һ���ǿո���ַ���
      while (str[last] == ' ')
        last--;
      str[last + 1] = '\0';
      // ������ǰ�ҵ����һ������ǰ���һ���ո�
      while (str[last] != ' ')
        last--;
      // ���λ�������������������
      strcpy(newName, str + last + 1);
      str[last] = '\0';
      strcpy(oldName, str + strlen("typedef") + 1);
      refectorTypeName(oldName); // ��ʽ���ɵ�����
      // Ȼ����Ҿɵ����ֶ�Ӧ��id�Ƿ����
      int id = strToId(tbl->strIds, oldName);
      // �������-1,˵�����id������,����Ϊ��Ӧ��unknownid
      if (id < 0)
      {
        putStrId(tbl->strIds, oldName, 0); // ���ð󶨸��ַ�Ϊ0�±�
        id = 0;
      }
      // ������ص�id�ǷǸ���,��˵������,���µ����ֵ��ɵ����ֶ�Ӧ��id��
      putStrId(tbl->strIds, newName, id);
    }
    // ����{��,��{ǰ������Ϊ���ͱ���
    else if (end == '{')
    {
      // ���ȶ�ȡ��������ǰ��,�õ����͵ı���
      char baseName[400];
      end = mysgets(baseName, "{", track);
      track += strlen(baseName) + 1;
      refectorTypeName(baseName);
      // Ȼ��ע���������
      putStrId(tbl->strIds, baseName, getTypeId(tbl->types.size, 0));
      // Ȼ���ж�����,��ȡ�����������Ϣ
      mysgets(tmp, " ", baseName);
      Type type1; // ׼��һ��type
      char typeStr[800];
      end = mysgets(typeStr, "}", track);
      if (end != '}')
      {
        return 0;
      }
      track += strlen(typeStr) + 1;
      if (strcmp(tmp, "enum") == 0)
      {
        // ��ȡ�������м�enum����Ϣ
        type1 = extractEnum(typeStr);
      }
      else if (strcmp(tmp, "struct") == 0)
      {
        // ��ȡ�������м�struct����Ϣ
        type1 = extractStruct(typeStr);
      }
      else if (strcmp(tmp, "union") == 0)
      {
        // ��ȡ�����ż�union����Ϣ
        type1 = extractUnion(typeStr);
      }
      else
      {
        return 0;
      }
      // Ȼ���ȡtype�ı����Ϳ��ܵ�ָ�����,ֱ��������ͷΪֹ
      while ((end = mysgets(tmp, ",", track)) != '\0')
      {
        // �����ж�ǰ����ٸ�*��,�ж��Ƕ��ٲ�ı���
        // ���ȶ�ȡǰ��*������
        int layer = 0;
        int i = 0;
        while (tmp[i] != '\0' && (tmp[i] == ' ' || tmp[i] == '*'))
        {
          if (tmp[i] == '*')
            layer++;
          i++;
        }
        // ��ʽ������
        refectorTypeName(tmp + i);
        // Ȼ��ע��
        int id = getTypeId(tbl->types.size, layer);
        putStrId(tbl->strIds, tmp + i, id);
        track += strlen(tmp) + 1;
      }
      // �������һ������
      if (strlen(tmp) != 0)
      {
        int layer = 0;
        int i = 0;
        while (tmp[i] != '\0' && (tmp[i] == ' ' || tmp[i] == '*'))
        {
          if (tmp[i] == '*')
            layer++;
          i++;
        }
        // ��ʽ������
        refectorTypeName(tmp + i);
        // Ȼ��ע��
        int id = getTypeId(tbl->types.size, layer);
        putStrId(tbl->strIds, tmp + i, id);
        track += strlen(tmp) + 1;
      }
      vector_push_back(&tbl->types, &type1);
    }
  }
  // �����ж��Ƿ��ǽṹ��,Ȼ���ж��Ƿ�����ͨ���Ͷ���
  else
  {
    char name[300];
    end = mysgets(name, "{", str);
    char *typeStr = str + strlen(name) + 1;
    refectorTypeName(name);
    putStrId(tbl->strIds, name, getTypeId(tbl->types.size, 0));
    char *end = str;
    while (end < str + strlen(str) && *end != '}')
      end++;
    *end = '\0';
    Type toAdd;
    if (strcmp(tmp, "enum") == 0)
    {
      toAdd = extractEnum(typeStr);
    }
    else if (strcmp(tmp, "struct") == 0)
    {
      toAdd = extractStruct(typeStr);
    }
    // Ȼ���ж��Ƿ���
    else if (strcmp(tmp, "union") == 0)
    {
      toAdd = extractUnion(typeStr);
    }
    else
    {
      return 0;
    }
    vector_push_back(&tbl->types, &toAdd);
  }
  return 1;
}

//չʾtype
void showType(Type* type){
  //��ӡ��������
  printf("%s\n",typeKindName[type->kind]);

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


int findType(TypeTbl* tbl,char* typeName,int* layerRet){
  long long id;
  int typeIndex;
  int pLayer;
  //����ֱ�����Ż�ȡ��κ��±�
  id=strToId(tbl->strIds,typeName);
  if(id>=0){
    getTypeIndexAndPointerLayer(id,&typeIndex,&pLayer);
    if(layerRet!=NULL) *layerRet=pLayer;
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
  //����Ҳ�������ַ�����Ӧ��id,���unknow
  if(id<0){
    id=getTypeId(0,pLayer);
  }else{
    getTypeIndexAndPointerLayer(id,&typeIndex,&pLayer);
    pLayer+=pointerLayer;
    id=getTypeId(typeIndex,pLayer);
  }
  //�󶨸�ʽ����
  putStrId(tbl->strIds,ts,id);
  //��ԭʼ����id
  putStrId(tbl->strIds,typeName,id);
  free(ts);
  if(layerRet!=NULL) *layerRet=pLayer;
  return typeIndex;
}



//���һ��type����������
void delType(Type* type){
  initStrSet(&type->funcs);
  //TODO,�ͷ�fields��hashtbl fields������
  char** keys=malloc(type->fields.keySize*type->fields.size);
  char**  vals=malloc(type->fields.valSize*type->fields.size);
  hashtbl_toArr(&type->fields,keys,vals);
  for(int i=0;i<type->fields.size;i++){
    free(keys[i]);
    free(vals[i]);
  }
  free(keys);
  free(vals);
  hashtbl_release(&type->fields);
}


//ɾ�����ͱ�
int delTypeTbl(TypeTbl* tbl){
  delStrIdTable(tbl->strIds);
  free(tbl->strIds);
  tbl->strIds=NULL;
  Type* types=vector_toArr(&tbl->types);
  for(int i=0;i<tbl->types.size;i++){
    delType(types+i);
  }
  free(types);
  vector_clear(&tbl->types);
  return 1;
}


