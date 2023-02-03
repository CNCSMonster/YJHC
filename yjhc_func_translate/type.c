#include "type.h"

//获取全局类型表,里面有基础数据类型
TypeTbl getGlobalTypeTbl(){
  TypeTbl out=getTypeTbl();
  //加入基础数据类型
  for(int i=0;i<sizeof(baseTypeNames)/sizeof(baseTypeNames[0]);i++){
    if(i==TYPE_STRUCT||i==TYPE_ENUM||i==TYPE_UNION||i==TYPE_UNKNOW||i==TYPE_FUNC_POINTER) continue;
    char* defaultName=strcpy(malloc(strlen(baseTypeNames[i])+1),baseTypeNames[i]);
    Type toAdd=getType(defaultName,i);
    //放进去的字符串要动态分配空间
    char* key=strcpy(malloc(strlen(baseTypeNames[i])+1),baseTypeNames[i]);
    putStrId(out.strIds,key,getTypeId(out.types.size,0));
    vector_push_back(&out.types,&toAdd);
  }
  return out;
}

//生成类型结构体变量
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
  //out的第一位保存unknown
  Type unknownType=getType(NULL,TYPE_UNKNOW);
  vector_push_back(&out.types,&unknownType);
  return out;
}


//从文件中读取建立类型表,注意,类型表的前面内容要设置为基础类型
int loadFile_typeTbl(TypeTbl* tbl, FILE* fin){
  int isErr=0;  //用来记录读取过程中是否出现异常
  do{
    //首先判断是否是typedef,如果第一位是typedef,则后面还要读取别名
    char str[2000];
    char end=myfgets(str,"\n",fin);
    if(end==EOF&&str[0]=='\0') break;
    if(strlen(str)==0) continue;
    int jud=loadLine_typetbl(tbl,str);
    if(!jud){
      isErr=1;
      break;
    }
    //最后超前读取一位判断是否读取结束
    char c=fgetc(fin);
    if(c==EOF) break;
    else ungetc(c,fin);
  }while(1);
  //如果分析过程中出现异常,删除
  if(isErr){
    printf("err in loadFile\n");
    return 0;
  }
  return 1;
}

//根据从strid中取出的long long id分解出对应的kind下标以及指针维数
//指针维数为0表示就是这个类型本身
//long long前32位为对应基础type的下标,后32位为这个类型对应的指针层次
//long long
void extractTypeIndexAndPointerLayer(long long code,int* typeIndex,int* pointerLayer ){
  if(typeIndex!=NULL) *typeIndex=code>>32;
  if(pointerLayer!=NULL) *pointerLayer=(code<<32)>>32;
}

//根据kindIndex和pointerLayer获得对应的long long编码
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

//类型表加载字符串的类型信息,成功返回非0值,失败返回0
int loadLine_typetbl(TypeTbl* tbl,char* str){
  char tmp[100];
  char end = mysgets(tmp, " ", str);
  if (strcmp(tmp, "typedef") == 0)
  {
    if(!loadTypedefLine_typetbl(tbl,str)) return 0;
    else return 1;
  }
  // 首先判断是否是结构体,然后判断是否是普通类型定义
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
    // 然后判断是否是
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

//提取出加载typedefline信息的内容
int loadTypedefLine_typetbl(TypeTbl* tbl,char* str){
  char tmpStr[1000];
  strcpy(tmpStr,str);
  char* typeDefLine=tmpStr;
  char tmp[200];
  char end;
  // 如果先读到左括号，说明是重命名
  // 如果先读到结尾符,说明是对已知类型进行重命名
  end = mysgets(tmp, "{(", typeDefLine);
  char *track = typeDefLine + strlen("typedef") + 1;
  // 如果先遇到(,说明是给函数指针类型起别名
  if(end=='('){
    //给函数指针起别名,
    //提取名
    if(!loadTypedefFuncPointer_typetbl(tbl,typeDefLine)) return 0;
    return 1;
  }
  //如果不能找到(或者{,说明是给已知类型起别名
  else if (end == '\0')
  {
    char oldName[400];
    char newName[400];
    // 截取旧名,然后读取新名
    int last = strlen(typeDefLine) - 1;
    // 首先从后往前读取掉所有空格来到第一个非空格的字符处
    while (typeDefLine[last] == ' ')
      last--;
    typeDefLine[last + 1] = '\0';
    // 继续往前找到最后一个单词前面第一个空格
    while (typeDefLine[last] != ' ')
      last--;
    // 这个位置往后就是新名的名字
    strcpy(newName, typeDefLine + last + 1);
    typeDefLine[last] = '\0';
    strcpy(oldName, typeDefLine + strlen("typedef") + 1);
    formatTypeName(oldName); // 格式化旧的名字
    // 然后查找旧类型名对应的类型下标以及指针层次
    int layer;
    int typeIndex=findType(tbl,oldName,&layer);
    long long id=getTypeId(typeIndex,layer);
    // 如果typeIndex等于0,说明对应函数名不存在
    if (typeIndex == 0)
    {
      putStrId(tbl->strIds, oldName, 0); // 设置绑定该字符为0下标
      id = 0;
    }
    // 如果返回的id是非负数,则说明存在,绑定新的名字到旧的名字对应的id上
    putStrId(tbl->strIds, newName, id);
  }
  // 读到{先,则{前的名字为类型本名
  else if (end == '{')
  {
    // 首先读取到类型名前面,得到类型的本名
    char baseName[400];
    end = mysgets(baseName, "{", track);
    track += strlen(baseName) + 1;
    formatTypeName(baseName);
    // 然后注册基础名字
    putStrId(tbl->strIds, baseName, getTypeId(tbl->types.size, 0));
    // 然后判断类型,读取后面的类型信息
    mysgets(tmp, " ", baseName);
    Type type1; // 准备一个type
    char typeStr[800];
    end = mysgets(typeStr, "}", track);
    if (end != '}')
    {
      return 0;
    }
    track += strlen(typeStr) + 1;
    if (strcmp(tmp, "enum") == 0)
    {
      // 读取花括号中间enum的信息
      type1 = extractEnum(typeStr);
    }
    else if (strcmp(tmp, "struct") == 0)
    {
      // 读取花括号中间struct的信息
      type1 = extractStruct(typeStr);
    }
    else if (strcmp(tmp, "union") == 0)
    {
      // 读取花括号间union的信息
      type1 = extractUnion(typeStr);
    }
    else
    {
      return 0;
    }

    // 然后读取type的别名和可能的指针别名,直到读到尽头为止
    while ((end = mysgets(tmp, ",", track)) != '\0')
    {
      // 首先判断前面多少个*号,判断是多少层的别名
      // 首先读取前面*的数量
      int layer = 0;
      int i = 0;
      while (tmp[i] != '\0' && (tmp[i] == ' ' || tmp[i] == '*'))
      {
        if (tmp[i] == '*')
          layer++;
        i++;
      }
      // 格式化名字
      formatTypeName(tmp + i);
      // 获取别名的对应的typeId
      long long id= getTypeId(tbl->types.size, layer);
      //注册类型别名
      putStrId(tbl->strIds, tmp + i, id);
      track += strlen(tmp) + 1;
    }
    // 处理最后一个部分
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
      // 格式化名字
      formatTypeName(tmp + i);
      // 然后注册
      long long id = getTypeId(tbl->types.size, layer);
      putStrId(tbl->strIds, tmp + i, id);
      track += strlen(tmp) + 1;
    }
    type1.defaultName=strcpy(malloc(strlen(baseName)+1),baseName);
    vector_push_back(&tbl->types, &type1);
  }
  return 1;
}

//函数指针的重命名
int loadTypedefFuncPointer_typetbl(TypeTbl* tbl,char* str){
  //首先来到typedef之后的位置
  str+=strlen("typedef")+1; //越过第一个位置的typedef,并越过typedef后面的一个空格
  char funcPointerType[1000];
  strcpy(funcPointerType,str);  //用来保存函数指针的别名
  // 首先提取字符串
  char newName[300]; // 用来保存函数指针的别名
  extractFuncPointerFieldName(funcPointerType, newName, NULL);
  if (!formatTypeName(funcPointerType))
    return 0;
  // 或者是函数指针类型产生个新类型加入
  char *fpType = strcpy(malloc(strlen(funcPointerType) + 1), funcPointerType);
  Type newFpType = getType(fpType, TYPE_FUNC_POINTER);
  putStrId(tbl->strIds, newName, getTypeId(tbl->types.size, 0));
  vector_push_back(&tbl->types, &newFpType);
  return 1;
}


//获得规格化的函数指针类型名
char* formatFuncPointerTypeName(char* str){
  myStrStrip(str," "," ");  //去掉前后空格
  vector argTypes=getVector(sizeof(char*)); //保存参数类型名称
  //保存返回类型名
  char retType[200];
  char end=mysgets(retType," (",str);
  formatTypeName(retType); //规范设定返回类型不能以函数指针的形式给出,
  //开始搜索类型名
  if(str[strlen(str)-1]!=')') return NULL;
  int toIndex=strlen(str)-2;
  int fromIndex=toIndex-1;
  //修改fromIndex的内容
  int except=1; //是否期待遇到一个参数类型
  int isRight=1;
  while(except){
    //往左找到参数类型的尽头
    int rightPar=0; //一个完整的类型字符串,哪怕是函数指针类型字符串
    //里面的左右括号应该是成对出现的
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
    //如果是左括号,说明前面已经加入完了
    if(str[fromIndex]=='('){
      except=0;
    }
    //否则加入这次要加入的字符串
    char tmpType[300];
    strncpy(tmpType,str+fromIndex+1,toIndex-fromIndex);
    tmpType[toIndex-fromIndex]='\0';
    formatTypeName(tmpType);
    char* tmpStr=strcpy(malloc(strlen(tmpType)+1),tmpType);
    vector_push_back(&argTypes,&tmpStr);
    toIndex=fromIndex-1;
    fromIndex=toIndex;
  }
  //如果没有正确执行该语句,进行错误处理
  if(!isRight){
    //错误处理
    printf("err in refector funcpointerName\n");  //打印提示语句
    //释放空间
    for(int i=0;i<argTypes.size;i++){
      char* t;
      vector_get(&argTypes,i,&t);
      free(t);
    }
    vector_clear(&argTypes);
    return 0;
  }
  //正确情形,把函数指针类型名重组完成
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


//从规格化的函数指针类型中提取返回类型名以及参数类型列表,提取成功返回非0值，提取失败返回0
int extractRetTypeNameAndArgTypes(const char* str,char* retTypeName,vector* argTypes){
  if(retTypeName==NULL||argTypes==NULL||argTypes->valSize!=sizeof(char*)) return 0;
  //保存返回类型名
  char end=mysgets(retTypeName," (",str);
  formatTypeName(retTypeName); //规范设定返回类型不能以函数指针的形式给出,
  //开始搜索类型名
  if(str[strlen(str)-1]!=')') return 0;
  int toIndex=strlen(str)-2;
  int fromIndex=toIndex-1;
  //修改fromIndex的内容
  int except=1; //是否期待遇到一个参数类型
  int isRight=1;
  while(except){
    //往左找到参数类型的尽头
    int rightPar=0; //一个完整的类型字符串,哪怕是函数指针类型字符串
    //里面的左右括号应该是成对出现的
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
    //如果是左括号,说明前面已经加入完了
    if(str[fromIndex]=='('){
      except=0;
    }
    //否则加入这次要加入的字符串
    char tmpType[300];
    strncpy(tmpType,str+fromIndex+1,toIndex-fromIndex);
    tmpType[toIndex-fromIndex]='\0';
    formatTypeName(tmpType);
    char* tmpStr=strcpy(malloc(strlen(tmpType)+1),tmpType);
    vector_push_back(argTypes,&tmpStr);
    toIndex=fromIndex-1;
    fromIndex=toIndex;
  }
  //如果没有正确执行该语句,进行错误处理
  if(!isRight){
    //错误处理
    printf("err in extractRetTypeNameAndArgTypes\n");  //打印提示语句
    //释放空间
    for(int i=0;i<argTypes->size;i++){
      char* t;
      vector_get(argTypes,i,&t);
      free(t);
    }
    vector_clear(argTypes);
    return 0;
  }
  
  //因为这个时候数组的参数类型属性是倒排的，进行一个倒置
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


//展示type
void showType(Type* type){
  //打印类型种类名字
  printf("typeKind:%s\n",typeKindName[type->kind]);
  printf("typeName:%s\n",type->defaultName);
  //然后打印类型拥有的函数指针类型变量
  printf("func pointer fields:\n");
  //打印函数指针属性
  char** strArr=toStrArr_StrSet(&type->funcPointerFields);
  for(int i=0;i<type->funcPointerFields.num;i++){
    printf("%d.%s\n",i+1,strArr[i]);
  }
  free(strArr);
  //取出所有变量还有类型以便于打印
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

//extract enum,如果extract失败,返回个释放了空间的out
Type extractEnum(char* str){
  Type out;
  out.kind=TYPE_ENUM;
  out.fields=getHashTbl(10,sizeof(char*),sizeof(char*),typeFieldNameHash,typeFieldEq);
  out.funcs=getStrSet(myStrHash);
  out.funcPointerFields=getStrSet(myStrHash);
  //enum里面只能有成员
  char tmp[1000];
  char end;
  int isWrong=0;
  do{
    end=mysgets(tmp," ,",str);
    str+=strlen(tmp)+1;
    if(strlen(tmp)==0) continue;
    char* toAdd=strcpy(malloc(strlen(tmp)+1),tmp);
    hashtbl_put(&out.fields,&toAdd,NULL);  //获得的是名称
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
  //enum里面只能有成员
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
    //判断是否是函数指针定义,如果是返回1
    if(isFuncPointerFieldDef(tmp)){
      if(!loadFuncPointerFieldDef(&out,tmp)){
        isWrong=1;
        break;
      }
      continue;
    }
    //如果末尾是括号
    while(tmp[strlen(tmp)-1]==' ') tmp[strlen(tmp)-1]='\0';
    //如果是函数
    if(tmp[strlen(tmp)-1]==')'){
      //往左找到左边对应的括号
      int right=1;  //记录右括号数量
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
        //加入找到对应函数名
        if(left<first){
          addStr_StrSet(&out.funcs,left);
        }
      }
    }
    //否则是数组定义,这里把数组党成是一个指针处理
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
      //获取名字
      //首先读变量名
      char* fieldName;
      char* fieldType;
      tmp[i+1]='\0';
      //然后提取变量名
      i=strlen(tmp)-1;
      while (tmp[i]==' ') i--;
      tmp[i+1]='\0';
      while(tmp[i]!=' ') i--;
      fieldName=strcpy(malloc(strlen(tmp+i+1)+1),tmp+i+1);
      tmp[i]='\0';
      //格式化变类型名
      formatTypeName(tmp);
      fieldType=strcpy(malloc(strlen(tmp)+1+pointerLayer),tmp);
      char* ts=fieldType+strlen(fieldType);
      for(int j=0;j<pointerLayer;j++){
        *ts='*';
        ts++;
      }
      *ts='\0';
      //加入变量名和类型名
      hashtbl_put(&out.fields,&fieldName,&fieldType);
    }
    //否则是属性定义
    else{
      //首先读变量名
      char* fieldName;
      char* fieldType;
      //然后提取变量名
      int i=strlen(tmp)-1;
      while (tmp[i]==' ') i--;
      tmp[i+1]='\0';
      while(tmp[i]!=' ') i--;
      fieldName=strcpy(malloc(strlen(tmp+i+1)+1),tmp+i+1);
      tmp[i]='\0';
      //格式化变量名
      formatTypeName(tmp);
      fieldType=strcpy(malloc(strlen(tmp)+1),tmp);
      //加入变量名和类型名
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
  //enum里面只能有成员
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
    //处理可能是函数指针定义语句
    if(isFuncPointerFieldDef(tmp)){
      if(!loadFuncPointerFieldDef(&out,tmp)){
        isWrong=1;
        break;
      }
      continue;
    }
    //首先去掉末尾多余空格
    while(tmp[strlen(tmp)-1]==' ') tmp[strlen(tmp)-1]='\0';
    //如果末尾是右括号,说明是函数
    if(tmp[strlen(tmp)-1]==')'){
      //往左找到左边对应的括号
      int right=1;  //记录右括号数量
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
        //加入找到对应函数名
        if(left<first){
          addStr_StrSet(&out.funcs,left);
        }
      }
    }
    //否则是数组定义,这里把数组党成是一个指针处理
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
      //获取名字
      //首先读变量名
      char* fieldName;
      char* fieldType;
      tmp[i+1]='\0';
      //然后提取变量名
      i=strlen(tmp)-1;
      while (tmp[i]==' ') i--;
      tmp[i+1]='\0';
      while(tmp[i]!=' ') i--;
      fieldName=strcpy(malloc(strlen(tmp+i+1)+1),tmp+i+1);
      tmp[i]='\0';
      //格式化变类型名
      formatTypeName(tmp);
      fieldType=strcpy(malloc(strlen(tmp)+1+pointerLayer),tmp);
      char* ts=fieldType+strlen(fieldType);
      for(int j=0;j<pointerLayer;j++){
        *ts='*';
        ts++;
      }
      *ts='\0';
      //加入变量名和类型名
      hashtbl_put(&out.fields,&fieldName,&fieldType);
    }
    //否则是属性定义
    else{
      //首先读变量名
      char* fieldName;
      char* fieldType;
      //然后提取变量名
      int i=strlen(tmp)-1;
      while (tmp[i]==' ') i--;
      tmp[i+1]='\0';
      while(tmp[i]!=' ') i--;
      fieldName=strcpy(malloc(strlen(tmp+i+1)+1),tmp+i+1);
      tmp[i]='\0';
      //格式化变量名
      formatTypeName(tmp);
      fieldType=strcpy(malloc(strlen(tmp)+1),tmp);
      //加入变量名和类型名
      hashtbl_put(&out.fields,&fieldName,&fieldType);
    }
  }while(end!='\0');
  if(isWrong){
    printf("gg");
    delType(&out);
  }
  return out;
}


//格式化类型字符串
int formatTypeName(char* str){
  //首先去除前后空格
  myStrStrip(str," "," ");
  //如果判断是函数指针类型字符串,则格式函数指针类型字符串
  if(isFuncPointerType(str)){
    return formatFuncPointerTypeName(str)==NULL?0:1;
  }

  vector vec=getVector(sizeof(char));
  int i=0;
  //首先越过前面所有空格
  while(str[i]==' ') i++;
  //减去后面的所有空格
  while(str[strlen(str)-1]==' ') str[strlen(str)-1]='\0';
  for(;i<strlen(str);i++){
    //只写入一个空格
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
    else break; //否则是异常格式,只读取前面部分
  }
  char c='\0';
  vector_push_back(&vec,&c);
  char* s=vector_toArr(&vec);
  strcpy(str,s);
  free(s);
  vector_clear(&vec);
  return 1;
}

//判断语句是否是函数指针类型属性定义语句,是返回非0值,不是返回0
int isFuncPointerFieldDef(const char* str){
  //函数指针属性定义语句可以用一个自动机来识别
  //字符串->左括号->星号->右括号->左括号->类型列表->右括号
  //void (*p2) (int,int)
  //结构体类型变量定义和一般变量定义没有括号,
  //结构体方法定义有一对括号
  //函数指针属性才会有两对括号
  char* stops="(";
  char tmp[1000];
  char end=mysgets(tmp,stops,str);
  if(end!='(') return 0;
  str+=strlen(tmp)+1;
  end=mysgets(tmp,stops,str);
  if(end!='(') return 0;
  return 1;
}

//判断是否是函数指针类型名或者函数指针参数名,
int isFuncPointerType(const char* str){
  if(str==NULL) return 0;
  //进行粗糙判断
  //TODO进行更精细判断
  //如果有两个括号则是函数指针类型名
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
  //不能重复定义同名变量
  if(containsStr_StrSet(&typep->funcPointerFields,fieldName)){
    return 0;
  }
  addStr_StrSet(&typep->funcPointerFields,fieldName); //提取函数指针属性名
  //绑定函数指针属性名与函数指针类型名
  char* key=strcpy( malloc(strlen(fieldName)+1),fieldName);
  char* val=strcpy( malloc(strlen(typeName)+1),typeName);
  if(!hashtbl_put(&typep->fpFieldToType,&key,&val)) return 0;
  return 1;
}


int findType(TypeTbl* tbl,char* typeName,int* layerRet){
  //首先判断是否是函数指针类型,如果是返回函数指针
  if(isFuncPointerType(typeName)){
    *layerRet=0;
    return 1;   //类型表的第一个位置为FUNC类型
  }
  long long id;
  int typeIndex;
  int pLayer;
  //首先直接试着获取层次和下标
  id=strToId(tbl->strIds,typeName);
  if(id>=0){
    extractTypeIndexAndPointerLayer(id,&typeIndex,&pLayer);
    if(layerRet!=NULL) *layerRet=pLayer;
    return typeIndex;
  }
  //获取不到则获取类型名字的格式化
  char* ts=strcpy(malloc(strlen(typeName)+1),typeName);
  formatTypeName(ts);
  int i=strlen(ts)-1;
  //然后统计指针层次，以及找到原始名字
  int pointerLayer=0;
  while(ts[i]=='*'){
    pointerLayer++;
    i--;
  }
  char old=ts[i+1];
  ts[i+1]='\0';
  id=strToId(tbl->strIds,ts);
  ts[i+1]=old;
  //如果找不到这个字符串对应的id,则绑定unknow
  if(id<0){
    typeIndex=0;
    id=getTypeId(typeIndex,pLayer);
  }else{
    extractTypeIndexAndPointerLayer(id,&typeIndex,&pLayer);
    pLayer+=pointerLayer;
    id=getTypeId(typeIndex,pLayer);
  }
  //绑定格式化串
  putStrId(tbl->strIds,ts,id);
  //绑定原始串与id
  putStrId(tbl->strIds,typeName,id);
  free(ts);
  if(layerRet!=NULL) *layerRet=pLayer;
  return typeIndex;
}



//清空一个type的所有内容
void delType(Type* type){
  if(type->defaultName!=NULL) free(type->defaultName);
  type->defaultName=NULL;
  initStrSet(&type->funcs);
  initStrSet(&type->funcPointerFields);
  //释放fields中hashtbl fields的内容
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


//删除类型表
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

//从函数指针属性定义中提取函数指针量名
int extractFuncPointerFieldName(const char* funcPointerFieldDef,char* retName,int* isConst){
  if(!isFuncPointerFieldDef(funcPointerFieldDef)) return 0;
  
  //查找到第一个(的出现
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
  //判断是否是常量类型
  end=mysgets(tmp," ",retName);
  if(end==' '&&strcmp(tmp,"const")==0){
    if(isConst!=NULL) *isConst=1;
    strcpy(tmp,retName+strlen("const")+1);
    //判断是否是合法的id,TODO
    strcpy(retName,tmp);
  }else{
    if(isConst!=NULL) *isConst=0;
  }
  myStrStrip(retName," "," ");
  //判断retName是否是合法的id
  if(!isLegalId(retName)) return 0;
  return 1;
}

//获取类型对应的默认值,结果是一个动态分配空间的字符串,获取失败返回NULL,只有unknown类型才有默认为NULL的值
int getDefaultValOfCertainType(char** retDefaultVal,TypeKind typeKind,int layer){
  if(typeKind>=TYPE_KIND_NUM||typeKind<0) return 0;
  *retDefaultVal= defaultValueOfBaseTypes[typeKind];
  return 1;
}

