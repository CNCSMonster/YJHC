#include "type.h"


//从文件中读取建立类型表,注意,类型表的前面内容要设置为基础类型
TypeTbl getTypeTbl(FILE* fin){
  TypeTbl out;
  out.strIds=getStrIdTable();
  out.size=0;
  out.types=NULL;
  //读取typeTbl里面的内容
  vector vec=getVector(sizeof(Type));
  //先加入个未知类型放在底部
  Type unknownType;
  unknownType.fields=getHashTbl(0,sizeof(char*),sizeof(char*),typeFieldNameHash,typeFieldEq);
  unknownType.kind=UNKNOWN;
  unknownType.funcs=getStrSet(myStrHash);  //设置使用的哈希函数为NULL,因为不会用到
  vector_push_back(&vec,&unknownType);
  //加入基础数据类型
  for(int i=0;i<sizeof(baseTypeNames)/sizeof(baseTypeNames[0]);i++){
    
    if(baseTypeNames[i]==NULL) continue;
    Type toAdd;
    toAdd.kind=i;
    toAdd.funcs=getStrSet(myStrHash);
    toAdd.fields=getHashTbl(0,sizeof(char*),sizeof(char*),typeFieldNameHash,typeFieldEq);
    //放进去的字符串要动态分配空间
    char* key=strcpy(malloc(strlen(baseTypeNames[i])+1),baseTypeNames[i]);
    putStrId(out.strIds,key,getTypeId(vec.size,0));
    vector_push_back(&vec,&toAdd);
  }
  int isErr=0;  //用来记录读取过程中是否出现异常
  do{
    //首先判断是否是typedef,如果第一位是typedef,则后面还要读取别名
    char tmp[2000];
    char end=myfgets(tmp,"\n",fin);
    if(end==EOF&&tmp[0]=='\0') break;
    if(strlen(tmp)==0) continue;
    char tmp2[100];
    end=mysgets(tmp2," ",tmp);
    if(strcmp(tmp2,"typedef")==0){
      //如果先读到左括号，说明是重命名
      //如果先读到分号,说明是对已知类型进行重命名
      end=mysgets(tmp2,"{",tmp);
      char* track=tmp+strlen("typedef")+1;
      //如果没有{符,说明这里typedef语句仅仅是给已知类型起别名
      if(end=='\0'){
        char oldName[400];
        char newName[400];
        //截取旧名,然后读取新名
        int last=strlen(tmp)-1;
        //首先从后往前读取掉所有空格来到第一个非空格的字符处
        while(tmp[last]==' ') last--;
        tmp[last+1]='\0';
        //继续往前找到最后一个单词前面第一个空格
        while(tmp[last]!=' ') last--;
        //这个位置往后就是新名的名字
        strcpy(newName,tmp+last+1);
        tmp[last]='\0';
        strcpy(oldName,tmp+strlen("typedef")+1);
        refectorTypeName(oldName);  //格式化旧的名字
        //然后查找旧的名字对应的id是否存在
        int id=strToId(out.strIds,oldName);
        //如果返回-1,说明这个id不存在,设置为对应的unknownid
        if(id<0){
          putStrId(out.strIds,oldName,0); //设置绑定该字符为0下标
          id=0;
        }
        //如果返回的id是非负数,则说明存在,绑定新的名字到旧的名字对应的id上
        putStrId(out.strIds,newName,id);
      }
      //读到{先,则{前的名字为类型本名
      else if(end=='{'){
        //首先读取到类型名前面,得到类型的本名
        char baseName[400];
        end=mysgets(baseName,"{",track);
        track+=strlen(baseName)+1;
        refectorTypeName(baseName);
        //然后注册基础名字
        putStrId(out.strIds,baseName,getTypeId(vec.size,0));
        //然后判断类型,读取后面的类型信息
        mysgets(tmp2," ",baseName);
        Type type1; //准备一个type
        char typeStr[800];
        end=mysgets(typeStr,"}",track);
        if(end!='}'){
          isErr=1;
          break;
        }
        track+=strlen(typeStr)+1;
        if(strcmp(tmp2,"enum")==0){
          //读取花括号中间enum的信息
          type1=extractEnum(typeStr);
        }
        else if(strcmp(tmp2,"struct")==0){
          //读取花括号中间struct的信息
          type1=extractStruct(typeStr);
        }
        else if(strcmp(tmp2,"union")==0){
          //读取花括号间union的信息
          type1=extractUnion(typeStr);
        }
        else{
          isErr=1;
          break;
        }
        //然后读取type的别名和可能的指针别名,直到读到尽头为止
        while((end=mysgets(tmp2,",",track))!='\0'){
          //首先判断前面多少个*号,判断是多少层的别名
          //首先读取前面*的数量
          int layer=0;
          int i=0;
          while(tmp2[i]!='\0'&&(tmp2[i]==' '||tmp2[i]=='*')){
            if(tmp2[i]=='*') layer++;
            i++;
          }
          //格式化名字
          refectorTypeName(tmp2+i);
          //然后注册
          int id=getTypeId(vec.size,layer);
          putStrId(out.strIds,tmp2+i,id);
          track+=strlen(tmp2)+1;
        }
        //处理最后一个部分
        if(strlen(tmp2)!=0){
          int layer=0;
          int i=0;
          while(tmp2[i]!='\0'&&(tmp2[i]==' '||tmp2[i]=='*')){
            if(tmp2[i]=='*') layer++;
            i++;
          }
          //格式化名字
          refectorTypeName(tmp2+i);
          //然后注册
          int id=getTypeId(vec.size,layer);
          putStrId(out.strIds,tmp2+i,id);
          track+=strlen(tmp2)+1;
        }
        vector_push_back(&vec,&type1);
      }
    }
    //首先判断是否是结构体,然后判断是否是普通类型定义
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
      //然后判断是否是
      else if(strcmp(tmp2,"union")==0){
        toAdd=extractUnion(typeStr);
      }
      else{
        isErr=1;
        break;
      }
      vector_push_back(&vec,&toAdd);
    }
    //最后超前读取一位判断是否读取结束
    char c=fgetc(fin);
    if(c==EOF) break;
    else ungetc(c,fin);
  }while(1);
  //如果分析过程中出现异常,删除
  if(isErr){
    printf("err in get typeTbl\n");
    //删除每个type
    Type* types=vector_toArr(&vec);
    for(int i=0;i<vec.size;i++) delType(types+i);
    free(types);
    vector_clear(&vec);
    return out;
  }
  out.types=vector_toArr(&vec);
  out.size=vec.size;
  vector_clear(&vec); //删除里面内容的表面
  return out;
}

//根据从strid中取出的long long id分解出对应的kind下标以及指针维数
//指针维数为0表示就是这个类型本身
//long long前32位为对应基础type的下标,后32位为这个类型对应的指针层次
//long long
void getTypeIndexAndPointerLayer(long long code,int* typeIndex,int* pointerLayer ){
  *typeIndex=code>>32;
  *pointerLayer=(code<<32)>>32;
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

//展示type
void showType(Type* type){
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
      refectorTypeName(tmp);
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
      refectorTypeName(tmp);
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
      refectorTypeName(tmp);
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
      refectorTypeName(tmp);
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
void refectorTypeName(char* str){
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
    else if(str[i]=='*') vector_push_back(&vec,&str[i]);
    else break; //否则是异常格式,只读取前面部分
  }
  char c='\0';
  vector_push_back(&vec,&c);
  char* s=vector_toArr(&vec);
  strcpy(str,s);
  free(s);
  vector_clear(&vec);
}



//根据类型名字查询一个类型,返回该类型在类型表中的下标
int findType(TypeTbl* tbl,char* typeName,int* layerRet){
  long long id;
  int typeIndex;
  int pLayer;
  //首先直接试着获取层次和下标
  id=strToId(tbl->strIds,typeName);
  if(id>=0){
    getTypeIndexAndPointerLayer(id,&typeIndex,&pLayer);
    *layerRet=pLayer;
    return typeIndex;
  }

  //获取不到则获取类型名字的格式化
  char* ts=strcpy(malloc(strlen(typeName)+1),typeName);
  refectorTypeName(ts);
  int i=strlen(ts)-1;
  //然后统计指针层次，以及找到原始名字
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



//清空一个type的所有内容
void delType(Type* type){
  initStrSet(&type->funcs);
  hashtbl_release(&type->fields);
}


//删除类型表
int delTypeTbl(TypeTbl* tbl){
  delStrIdTable(tbl->strIds);
  free(tbl->strIds);
  for(int i=0;i<tbl->size;i++){
    delType(&tbl->types[i]);
  }
  free(tbl->types);
  return 1;
}



