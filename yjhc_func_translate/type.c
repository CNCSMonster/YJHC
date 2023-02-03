#include "type.h"

//��ȡȫ�����ͱ�,�����л�����������
TypeTbl getGlobalTypeTbl(){
  TypeTbl out=getTypeTbl();
  //���������������
  for(int i=0;i<sizeof(baseTypeNames)/sizeof(baseTypeNames[0]);i++){
    if(i==TYPE_STRUCT||i==TYPE_ENUM||i==TYPE_UNION||i==TYPE_UNKNOW||i==TYPE_FUNC_POINTER) continue;
    char* defaultName=strcpy(malloc(strlen(baseTypeNames[i])+1),baseTypeNames[i]);
    Type toAdd=getType(defaultName,i);
    //�Ž�ȥ���ַ���Ҫ��̬����ռ�
    char* key=strcpy(malloc(strlen(baseTypeNames[i])+1),baseTypeNames[i]);
    putStrId(out.strIds,key,getTypeId(out.types.size,0));
    vector_push_back(&out.types,&toAdd);
  }
  return out;
}

//�������ͽṹ�����
Type getType(char* defaultName,TypeKind kind){
  Type out;
  out.kind = kind;
  out.funcs = getStrSet(myStrHash);
  out.defaultName = defaultName;
  out.fields = getHashTbl(0, sizeof(char *), sizeof(char *), typeFieldNameHash, typeFieldEq);
  out.funcPointerFields = getStrSet(myStrHash);
  out.fpFieldToType=getHashTbl(8,sizeof(char*),sizeof(char*),typeFieldNameHash, typeFieldEq);
  return out;
}


TypeTbl getTypeTbl(){
  TypeTbl out;
  out.strIds=getStrIdTable();
  out.types=getVector(sizeof(Type));
  //out�ĵ�һλ����unknown
  Type unknownType=getType(NULL,TYPE_UNKNOW);
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
void extractTypeIndexAndPointerLayer(long long code,int* typeIndex,int* pointerLayer ){
  if(typeIndex!=NULL) *typeIndex=code>>32;
  if(pointerLayer!=NULL) *pointerLayer=(code<<32)>>32;
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
    if(!loadTypedefLine_typetbl(tbl,str)) return 0;
    else return 1;
  }
  // �����ж��Ƿ��ǽṹ��,Ȼ���ж��Ƿ�����ͨ���Ͷ���
  else
  {
    char name[300];
    end = mysgets(name, "{", str);
    char *typeStr = str + strlen(name) + 1;
    formatTypeName(name);
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
    toAdd.defaultName=strcpy(malloc(strlen(name)+1),name);
    vector_push_back(&tbl->types, &toAdd);
  }
  return 1;
}

//��ȡ������typedefline��Ϣ������
int loadTypedefLine_typetbl(TypeTbl* tbl,char* str){
  char tmpStr[1000];
  strcpy(tmpStr,str);
  char* typeDefLine=tmpStr;
  char tmp[200];
  char end;
  // ����ȶ��������ţ�˵����������
  // ����ȶ�����β��,˵���Ƕ���֪���ͽ���������
  end = mysgets(tmp, "{(", typeDefLine);
  char *track = typeDefLine + strlen("typedef") + 1;
  // ���������(,˵���Ǹ�����ָ�����������
  if(end=='('){
    //������ָ�������,
    //��ȡ��
    if(!loadTypedefFuncPointer_typetbl(tbl,typeDefLine)) return 0;
    return 1;
  }
  //��������ҵ�(����{,˵���Ǹ���֪���������
  else if (end == '\0')
  {
    char oldName[400];
    char newName[400];
    // ��ȡ����,Ȼ���ȡ����
    int last = strlen(typeDefLine) - 1;
    // ���ȴӺ���ǰ��ȡ�����пո�������һ���ǿո���ַ���
    while (typeDefLine[last] == ' ')
      last--;
    typeDefLine[last + 1] = '\0';
    // ������ǰ�ҵ����һ������ǰ���һ���ո�
    while (typeDefLine[last] != ' ')
      last--;
    // ���λ�������������������
    strcpy(newName, typeDefLine + last + 1);
    typeDefLine[last] = '\0';
    strcpy(oldName, typeDefLine + strlen("typedef") + 1);
    formatTypeName(oldName); // ��ʽ���ɵ�����
    // Ȼ����Ҿ���������Ӧ�������±��Լ�ָ����
    int layer;
    int typeIndex=findType(tbl,oldName,&layer);
    long long id=getTypeId(typeIndex,layer);
    // ���typeIndex����0,˵����Ӧ������������
    if (typeIndex == 0)
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
    formatTypeName(baseName);
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
      formatTypeName(tmp + i);
      // ��ȡ�����Ķ�Ӧ��typeId
      long long id= getTypeId(tbl->types.size, layer);
      //ע�����ͱ���
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
      formatTypeName(tmp + i);
      // Ȼ��ע��
      long long id = getTypeId(tbl->types.size, layer);
      putStrId(tbl->strIds, tmp + i, id);
      track += strlen(tmp) + 1;
    }
    type1.defaultName=strcpy(malloc(strlen(baseName)+1),baseName);
    vector_push_back(&tbl->types, &type1);
  }
  return 1;
}

//����ָ���������
int loadTypedefFuncPointer_typetbl(TypeTbl* tbl,char* str){
  //��������typedef֮���λ��
  str+=strlen("typedef")+1; //Խ����һ��λ�õ�typedef,��Խ��typedef�����һ���ո�
  char funcPointerType[1000];
  strcpy(funcPointerType,str);  //�������溯��ָ��ı���
  // ������ȡ�ַ���
  char newName[300]; // �������溯��ָ��ı���
  extractFuncPointerFieldName(funcPointerType, newName, NULL);
  if (!formatTypeName(funcPointerType))
    return 0;
  // �����Ǻ���ָ�����Ͳ����������ͼ���
  char *fpType = strcpy(malloc(strlen(funcPointerType) + 1), funcPointerType);
  Type newFpType = getType(fpType, TYPE_FUNC_POINTER);
  putStrId(tbl->strIds, newName, getTypeId(tbl->types.size, 0));
  vector_push_back(&tbl->types, &newFpType);
  return 1;
}


//��ù�񻯵ĺ���ָ��������
char* formatFuncPointerTypeName(char* str){
  myStrStrip(str," "," ");  //ȥ��ǰ��ո�
  vector argTypes=getVector(sizeof(char*)); //���������������
  //���淵��������
  char retType[200];
  char end=mysgets(retType," (",str);
  formatTypeName(retType); //�淶�趨�������Ͳ����Ժ���ָ�����ʽ����,
  //��ʼ����������
  if(str[strlen(str)-1]!=')') return NULL;
  int toIndex=strlen(str)-2;
  int fromIndex=toIndex-1;
  //�޸�fromIndex������
  int except=1; //�Ƿ��ڴ�����һ����������
  int isRight=1;
  while(except){
    //�����ҵ��������͵ľ�ͷ
    int rightPar=0; //һ�������������ַ���,�����Ǻ���ָ�������ַ���
    //�������������Ӧ���ǳɶԳ��ֵ�
    while(fromIndex>=0&&(str[fromIndex]!=','&&(str[fromIndex]!='('))||rightPar!=0){
      if(str[fromIndex]=='(') rightPar--;
      else if(str[fromIndex]==')') rightPar++;
      if(rightPar<0){
        isRight=0;
        break;
      }
      fromIndex--;
    }
    if(fromIndex==0){
      isRight=0;
    }
    if(!isRight){
      break;
    }
    //�����������,˵��ǰ���Ѿ���������
    if(str[fromIndex]=='('){
      except=0;
    }
    //����������Ҫ������ַ���
    char tmpType[300];
    strncpy(tmpType,str+fromIndex+1,toIndex-fromIndex);
    tmpType[toIndex-fromIndex]='\0';
    formatTypeName(tmpType);
    char* tmpStr=strcpy(malloc(strlen(tmpType)+1),tmpType);
    vector_push_back(&argTypes,&tmpStr);
    toIndex=fromIndex-1;
    fromIndex=toIndex;
  }
  //���û����ȷִ�и����,���д�����
  if(!isRight){
    //������
    printf("err in refector funcpointerName\n");  //��ӡ��ʾ���
    //�ͷſռ�
    for(int i=0;i<argTypes.size;i++){
      char* t;
      vector_get(&argTypes,i,&t);
      free(t);
    }
    vector_clear(&argTypes);
    return 0;
  }
  //��ȷ����,�Ѻ���ָ���������������
  char* track=str;
  sprintf(track,"%s(*)(",retType);
  track+=strlen(retType)+strlen("(*)(");
  char* argType=NULL;
  for(int i=argTypes.size-1;i>0;i--){
    vector_get(&argTypes,i,&argType);
    myStrStrip(argType," "," ");
    sprintf(track,"%s,",argType);
    track+=strlen(track);
    free(argType);
  }
  if(argTypes.size>=1){
    vector_get(&argTypes,0,&argType);
    myStrStrip(argType," "," ");
    sprintf(track,"%s)",argType);
    free(argType);
  }
  vector_clear(&argTypes);
  return str;
}


//�ӹ�񻯵ĺ���ָ����������ȡ�����������Լ����������б�,��ȡ�ɹ����ط�0ֵ����ȡʧ�ܷ���0
int extractRetTypeNameAndArgTypes(const char* str,char* retTypeName,vector* argTypes){
  if(retTypeName==NULL||argTypes==NULL||argTypes->valSize!=sizeof(char*)) return 0;
  //���淵��������
  char end=mysgets(retTypeName," (",str);
  formatTypeName(retTypeName); //�淶�趨�������Ͳ����Ժ���ָ�����ʽ����,
  //��ʼ����������
  if(str[strlen(str)-1]!=')') return 0;
  int toIndex=strlen(str)-2;
  int fromIndex=toIndex-1;
  //�޸�fromIndex������
  int except=1; //�Ƿ��ڴ�����һ����������
  int isRight=1;
  while(except){
    //�����ҵ��������͵ľ�ͷ
    int rightPar=0; //һ�������������ַ���,�����Ǻ���ָ�������ַ���
    //�������������Ӧ���ǳɶԳ��ֵ�
    while(fromIndex>=0&&(str[fromIndex]!=','&&(str[fromIndex]!='('))||rightPar!=0){
      if(str[fromIndex]=='(') rightPar--;
      else if(str[fromIndex]==')') rightPar++;
      if(rightPar<0){
        isRight=0;
        break;
      }
      fromIndex--;
    }
    if(fromIndex==0){
      isRight=0;
    }
    if(!isRight){
      break;
    }
    //�����������,˵��ǰ���Ѿ���������
    if(str[fromIndex]=='('){
      except=0;
    }
    //����������Ҫ������ַ���
    char tmpType[300];
    strncpy(tmpType,str+fromIndex+1,toIndex-fromIndex);
    tmpType[toIndex-fromIndex]='\0';
    formatTypeName(tmpType);
    char* tmpStr=strcpy(malloc(strlen(tmpType)+1),tmpType);
    vector_push_back(argTypes,&tmpStr);
    toIndex=fromIndex-1;
    fromIndex=toIndex;
  }
  //���û����ȷִ�и����,���д�����
  if(!isRight){
    //������
    printf("err in extractRetTypeNameAndArgTypes\n");  //��ӡ��ʾ���
    //�ͷſռ�
    for(int i=0;i<argTypes->size;i++){
      char* t;
      vector_get(argTypes,i,&t);
      free(t);
    }
    vector_clear(argTypes);
    return 0;
  }
  
  //��Ϊ���ʱ������Ĳ������������ǵ��ŵģ�����һ������
  for(int i=0;i<argTypes->size/2;i++){
    char* s1=NULL;
    char* s2=NULL;
    int i1=i;
    int i2=argTypes->size-1-i1;
    vector_get(argTypes,i1,&s1);
    vector_set(argTypes,i2,&s1,&s2);
    vector_set(argTypes,i1,&s2,NULL);
  }
  return 1;
}


//չʾtype
void showType(Type* type){
  //��ӡ������������
  printf("typeKind:%s\n",typeKindName[type->kind]);
  printf("typeName:%s\n",type->defaultName);
  //Ȼ���ӡ����ӵ�еĺ���ָ�����ͱ���
  printf("func pointer fields:\n");
  //��ӡ����ָ������
  char** strArr=toStrArr_StrSet(&type->funcPointerFields);
  for(int i=0;i<type->funcPointerFields.num;i++){
    printf("%d.%s\n",i+1,strArr[i]);
  }
  free(strArr);
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
  out.funcPointerFields=getStrSet(myStrHash);
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
  out.funcPointerFields=getStrSet(myStrHash);
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
    //�ж��Ƿ��Ǻ���ָ�붨��,����Ƿ���1
    if(isFuncPointerFieldDef(tmp)){
      if(!loadFuncPointerFieldDef(&out,tmp)){
        isWrong=1;
        break;
      }
      continue;
    }
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
      formatTypeName(tmp);
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
      formatTypeName(tmp);
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
  out.funcPointerFields=getStrSet(myStrHash);
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
    //��������Ǻ���ָ�붨�����
    if(isFuncPointerFieldDef(tmp)){
      if(!loadFuncPointerFieldDef(&out,tmp)){
        isWrong=1;
        break;
      }
      continue;
    }
    //����ȥ��ĩβ����ո�
    while(tmp[strlen(tmp)-1]==' ') tmp[strlen(tmp)-1]='\0';
    //���ĩβ��������,˵���Ǻ���
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
      formatTypeName(tmp);
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
      formatTypeName(tmp);
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
int formatTypeName(char* str){
  //����ȥ��ǰ��ո�
  myStrStrip(str," "," ");
  //����ж��Ǻ���ָ�������ַ���,���ʽ����ָ�������ַ���
  if(isFuncPointerType(str)){
    return formatFuncPointerTypeName(str)==NULL?0:1;
  }

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
    else if(str[i]=='*'){
      vector_push_back(&vec,&str[i]);
      i++;
    }
    else break; //�������쳣��ʽ,ֻ��ȡǰ�沿��
  }
  char c='\0';
  vector_push_back(&vec,&c);
  char* s=vector_toArr(&vec);
  strcpy(str,s);
  free(s);
  vector_clear(&vec);
  return 1;
}

//�ж�����Ƿ��Ǻ���ָ���������Զ������,�Ƿ��ط�0ֵ,���Ƿ���0
int isFuncPointerFieldDef(const char* str){
  //����ָ�����Զ�����������һ���Զ�����ʶ��
  //�ַ���->������->�Ǻ�->������->������->�����б�->������
  //void (*p2) (int,int)
  //�ṹ�����ͱ��������һ���������û������,
  //�ṹ�巽��������һ������
  //����ָ�����ԲŻ�����������
  char* stops="(";
  char tmp[1000];
  char end=mysgets(tmp,stops,str);
  if(end!='(') return 0;
  str+=strlen(tmp)+1;
  end=mysgets(tmp,stops,str);
  if(end!='(') return 0;
  return 1;
}

//�ж��Ƿ��Ǻ���ָ�����������ߺ���ָ�������,
int isFuncPointerType(const char* str){
  if(str==NULL) return 0;
  //���дֲ��ж�
  //TODO���и���ϸ�ж�
  //����������������Ǻ���ָ��������
  int num=0;
  for(int i=0;i<strlen(str);i++){
    if(str[i]=='(') num++;
    if(num==2) return 1;
  }
  return 0;
}



int loadFuncPointerFieldDef(Type* typep,char* str){
  char fieldName[1000];
  int isConst;
  if(!extractFuncPointerFieldName(str,fieldName,&isConst)) return 0;
  char typeName[1000];
  strcpy(typeName,str);
  //
  if(!formatTypeName(typeName)) return 0;
  //�����ظ�����ͬ������
  if(containsStr_StrSet(&typep->funcPointerFields,fieldName)){
    return 0;
  }
  addStr_StrSet(&typep->funcPointerFields,fieldName); //��ȡ����ָ��������
  //�󶨺���ָ���������뺯��ָ��������
  char* key=strcpy( malloc(strlen(fieldName)+1),fieldName);
  char* val=strcpy( malloc(strlen(typeName)+1),typeName);
  if(!hashtbl_put(&typep->fpFieldToType,&key,&val)) return 0;
  return 1;
}


int findType(TypeTbl* tbl,char* typeName,int* layerRet){
  //�����ж��Ƿ��Ǻ���ָ������,����Ƿ��غ���ָ��
  if(isFuncPointerType(typeName)){
    *layerRet=0;
    return 1;   //���ͱ�ĵ�һ��λ��ΪFUNC����
  }
  long long id;
  int typeIndex;
  int pLayer;
  //����ֱ�����Ż�ȡ��κ��±�
  id=strToId(tbl->strIds,typeName);
  if(id>=0){
    extractTypeIndexAndPointerLayer(id,&typeIndex,&pLayer);
    if(layerRet!=NULL) *layerRet=pLayer;
    return typeIndex;
  }
  //��ȡ�������ȡ�������ֵĸ�ʽ��
  char* ts=strcpy(malloc(strlen(typeName)+1),typeName);
  formatTypeName(ts);
  int i=strlen(ts)-1;
  //Ȼ��ͳ��ָ���Σ��Լ��ҵ�ԭʼ����
  int pointerLayer=0;
  while(ts[i]=='*'){
    pointerLayer++;
    i--;
  }
  char old=ts[i+1];
  ts[i+1]='\0';
  id=strToId(tbl->strIds,ts);
  ts[i+1]=old;
  //����Ҳ�������ַ�����Ӧ��id,���unknow
  if(id<0){
    typeIndex=0;
    id=getTypeId(typeIndex,pLayer);
  }else{
    extractTypeIndexAndPointerLayer(id,&typeIndex,&pLayer);
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
  if(type->defaultName!=NULL) free(type->defaultName);
  type->defaultName=NULL;
  initStrSet(&type->funcs);
  initStrSet(&type->funcPointerFields);
  //�ͷ�fields��hashtbl fields������
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

//�Ӻ���ָ�����Զ�������ȡ����ָ������
int extractFuncPointerFieldName(const char* funcPointerFieldDef,char* retName,int* isConst){
  if(!isFuncPointerFieldDef(funcPointerFieldDef)) return 0;
  
  //���ҵ���һ��(�ĳ���
  char end;
  char tmp[1000];
  end=mysgets(tmp," (",funcPointerFieldDef);
  char* track=(char*)funcPointerFieldDef+strlen(tmp);
  while(*track!='\0'){
    char c=*track;
    if(c!='('&&c!=')'&&c!='*'&&c!=' ') break;
    track++;
  }
  if(*track=='\0') return 0;
  int i=0;
  while(track[i]!='\0'){
    if(track[i]=='(') break;
    if(track[i]==')') break;
    if(track[i]=='*') break;
    retName[i++]=track[i];
  }
  if(track[i]=='\0') return 0;
  retName[i]='\0';
  //�ж��Ƿ��ǳ�������
  end=mysgets(tmp," ",retName);
  if(end==' '&&strcmp(tmp,"const")==0){
    if(isConst!=NULL) *isConst=1;
    strcpy(tmp,retName+strlen("const")+1);
    //�ж��Ƿ��ǺϷ���id,TODO
    strcpy(retName,tmp);
  }else{
    if(isConst!=NULL) *isConst=0;
  }
  myStrStrip(retName," "," ");
  //�ж�retName�Ƿ��ǺϷ���id
  if(!isLegalId(retName)) return 0;
  return 1;
}

//��ȡ���Ͷ�Ӧ��Ĭ��ֵ,�����һ����̬����ռ���ַ���,��ȡʧ�ܷ���NULL,ֻ��unknown���Ͳ���Ĭ��ΪNULL��ֵ
int getDefaultValOfCertainType(char** retDefaultVal,TypeKind typeKind,int layer){
  if(typeKind>=TYPE_KIND_NUM||typeKind<0) return 0;
  *retDefaultVal= defaultValueOfBaseTypes[typeKind];
  return 1;
}

