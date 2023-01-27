#include "func_translate.h"



//创建函数翻译器,翻译结果是把未翻译的yjhc的函数代码token转为c的函数代码token序列
FuncTranslator getFuncTranslator(char* typePath,char* funcHeadPath,char* valPath){
  FuncTranslator out={
    .funcTbl=NULL,
    .gloabalTypeTbl=NULL,
    .partialValTbl=NULL,
    .globalValTbl=NULL,
    .curFunc=NULL,
    .judSentenceKindErr=0,
    .warningFout=stdout
  };
  if(typePath==NULL||funcHeadPath==NULL||valPath==NULL) return out;
  int isRight=1;  //记录是正确的
  FILE* typeFin=fopen(typePath,"r");
  FILE* funcFin=fopen(funcHeadPath,"r");
  FILE* valFin=fopen(valPath,"r");
  if(typeFin==NULL||funcFin==NULL||valFin==NULL){
    isRight=0;
  }
  if(!isRight){
    if(typeFin!=NULL) fclose(typeFin);
    if(funcFin!=NULL) fclose(funcFin);
    if(valFin!=NULL) fclose(valFin);
    return out;
  }
  out.gloabalTypeTbl=malloc(sizeof(TypeTbl));
  *(out.gloabalTypeTbl)=getTypeTbl();
  loadFile_typeTbl(out.gloabalTypeTbl,typeFin);

  out.funcTbl=malloc(sizeof(FuncTbl));
  *(out.funcTbl)=getFuncTbl(out.gloabalTypeTbl);
  loadFile_functbl(out.funcTbl,funcFin);

  out.globalValTbl=malloc(sizeof(ValTbl));
  *(out.globalValTbl)=getValTbl(*(out.gloabalTypeTbl));
  loadFile_valtbl(out.globalValTbl,valFin);
  out.partialValTbl=out.globalValTbl;
  //完成空间分配后释放空间
  fclose(typeFin);
  fclose(valFin);
  fclose(funcFin);
  return out;
}

//设置funcTranslator的输出路径,设置成功返回1,设置失败返回0
int functranslator_setFout(FuncTranslator* translator,char* warningOutPath){
  if(warningOutPath==NULL) return 0;
  FILE* newFout=fopen(warningOutPath,"w");
  if(newFout==NULL) return 0;
  translator->warningFout=newFout;
  return 1;
}


//进行翻译前检查,比如函数是否全都实现
int pre_translate_check(FuncTranslator* translator){
  //翻译前检查
  //检查1,检查结构体方法是否全部实现
  //首先,取出所有结构体方法到一个数组中
  StrSet strset=getStrSet(myStrHash);
  int isRight=1;  //标识
  //先把所有函数加入strset
  for(int i=0;i<translator->funcTbl->funcKeys.size&&isRight;i++){
    //根据函数名字取出函数标识串
    char* funcKey;
    vector_get(&translator->funcTbl->funcKeys,i,&funcKey);
    //加入标识串
    if(!addStr_StrSet(&strset,funcKey)){
      //获取主人和typeid
      long long typeId;
      char funcName[200];
      extractFuncNameAndOwnerFromKey(funcKey,funcName,&typeId);
      if(typeId==0)
        fprintf(translator->warningFout,"redefinition of function %s\n",funcName);
      else{
        Type type;
        int typeIndex;
        extractTypeIndexAndPointerLayer(typeId,&typeIndex,NULL);
        vector_get(&translator->gloabalTypeTbl->types,typeIndex,&type);
        fprintf(translator->warningFout,"redefinition of member function %s of %s type\n",funcName,type.defaultName);
      }
      isRight=0;
    }
  }
  if(!isRight){
    initStrSet(&strset);
    return 0;
  }
  //然后进行结构体的方法遍历.判断是否有未实现的
  for(int i=2;i<translator->gloabalTypeTbl->types.size&&isRight;i++){
    Type type;
    vector_get(&translator->gloabalTypeTbl->types,i,&type);
    if(type.kind!=TYPE_UNION&&type.kind!=TYPE_STRUCT) continue;
    //获取typeId
    long long typeId=getTypeId(i,0);
    char** funcsArr=toStrArr_StrSet(&type.funcs);
    for(int j=0;j<type.funcs.num&&isRight;j++){
        char* tfk=getFuncKey(funcsArr[j],typeId);
        if(!delStr_StrSet(&strset,tfk)){
          //打印没有实现的函数
          fprintf(translator->warningFout,"member func %s of %s type not find definition\n",funcsArr[j],type.defaultName);
          isRight=0;
        }
        free(tfk);
    }
    free(funcsArr);
  }
  if(!isRight){
    initStrSet(&strset);
    return 0;
  }
  //找到没有声明但是实现了的函数,也就是strset中剩下的函数
  char** keys=toStrArr_StrSet(&strset);
  for(int i=0;i<strset.num;i++){
    //去判断该函数是否是全局函数,如果是全局的函数,则不进行检查
    char funcName[300];
    long long retOwnerId;
    extractFuncNameAndOwnerFromKey(keys[i],funcName,&retOwnerId);
    //根据retOwnerId判断是否是全局函数
    free(keys[i]);
    if(retOwnerId==0){
      continue;
    }
    //获取对应类型的名字
    int index;
    extractTypeIndexAndPointerLayer(retOwnerId,&index,NULL);
    Type type;
    vector_get(&translator->gloabalTypeTbl->types,index,&type);
    if(type.kind!=TYPE_STRUCT&&type.kind!=TYPE_UNION) continue;
    fprintf(translator->warningFout,"definition of member function %s but without declaration in \"%s\"'s definition\n",funcName,type.defaultName);
  }
  free(keys);
  initStrSet(&strset);
  if(!isRight) return 0;
  return 1;
}



//进行是否有主函数检查,如果有主函数,返回非0值,如果没有，返回0
int check_main_function(FuncTranslator* translator){
  //在函数表中查找主人为NULL,函数名为main的函数
  char* funcKey=getFuncKey("main",0);
  int out=0;
  if(hashtbl_get(&translator->funcTbl->funcs,&funcKey,NULL)){
    out=1;
  }
  free(funcKey);
  return out;
}


//使用函数翻译器开始翻译
int func_translate(FuncTranslator* funcTranslator,char* tokenInPath,char* tokenOutPath){
  if(tokenInPath==NULL || tokenOutPath==NULL ) return 0;
  //首先进行选择性检查
  //进行主函数检查
  if(!check_main_function(funcTranslator)){
    printf("main function miss!\n");
    return 0;
  }
  //进行函数完整性检查
  if(!pre_translate_check(funcTranslator)){
    return 0;
  }
  //初始化输入输出工具
  FILE* fin=fopen(tokenInPath,"r");
  FILE* fout=fopen(tokenOutPath,"w");
  int isRight=1;
  if(fin==NULL||fout==NULL) isRight=0;
  if(!isRight){
    if(!fin) fclose(fin);
    if(!fout) fclose(fout);
    return 0;
  }
  init_token_reader(fin);
  //正式工作
  TBNode* nodes;
  ActionSet actionSet;
  int preBlocks=0;
  //先全部进行翻译,然后格式化的工作另外再完成,
  //翻译的结果应该是token序列
  int funcIndex=0;  //记录翻译到的函数的下标
  while((nodes=readTokenSentence(&actionSet))!=NULL){
    //先进行处理
    //进行方法内自身缺失调度补充
    nodes=member_use_complement(funcTranslator,nodes);

    // 对一个句子补充函数
    nodes=process_singleLine(funcTranslator,nodes);

    //然后把处理结果写入文件
    if(nodes!=NULL) 
      fput_tokenLine(fout,nodes);

    //debug用的打印语句
    // if(nodes!=NULL) fshow_tokenLine(stdout,nodes);

    //进行块的进出更新
    if(preBlocks==actionSet.blocks-1){
      //判断是否要加载函数
      if(preBlocks==0){ //加载函数
        //查找对应函数
        char* funcKey;
        vector_get(&funcTranslator->funcTbl->funcKeys,funcIndex,&funcKey);
        funcIndex++;
        //根据名字查找函数
        hashtbl_get(&funcTranslator->funcTbl->funcs,&funcKey,&funcTranslator->curFunc);
        //TODO加入安全检查
        if(funcTranslator->curFunc==NULL){
          isRight=0;
          break;
        }
      }
      //增加新的局部量表
      funcTranslator->partialValTbl=extendValTbl(funcTranslator->partialValTbl);
      //加入函数的参数表到局部变量中
      loadArgs_valtbl(funcTranslator->partialValTbl,funcTranslator->funcTbl,funcTranslator->curFunc);
    }
    else if(preBlocks==actionSet.blocks+1){
      //判断函数是否退出
      if(preBlocks==1){
        funcTranslator->curFunc=NULL;
      }
      funcTranslator->partialValTbl=recycleValTbl(funcTranslator->partialValTbl);
    }
    preBlocks=actionSet.blocks;
    del_tokenLine(nodes);
    nodes=NULL;
  }
  if(nodes!=NULL) del_tokenLine(nodes);
  release_token_reader();
  fclose(fin);
  fclose(fout);
  if(!isRight) return 0;
  return 1;
}


//补全成员方法中自身方法或者属性的调用
TBNode* member_use_complement(FuncTranslator* funcTranslator,TBNode* nodes){
  //如果不是成员方法则不用补全类型
  if(funcTranslator->curFunc==NULL) return nodes;
  //查找有没有成员方法
  TBNode* track=nodes;
  TBNode* pre=NULL;
  TBNode* prePre=NULL;
  TBNode head;
  head.next=nodes;
  nodes->last=&head;
  // 取出函数主人
  char* ownerName=funcTranslator->curFunc->owner;
  if(ownerName==NULL) return nodes; //如果函数没有主人名,则不是自身调用函数
  //取出函数主人，找到对应类型
  int typeIndex=findType(funcTranslator->gloabalTypeTbl,ownerName,NULL);
  Type type;
  if(typeIndex!=0){
    vector_get(&funcTranslator->gloabalTypeTbl->types,typeIndex,&type);
  }
  while (track!=NULL)
  {
    int ifToComplete=0; //判断是否去补全
    //如果func是比较单独的,则检查匹配判断是否需要前面插入self.
    //如果一个track既不是func也不是var，则不需要进行成员调用补全
    if(track->token.kind!=FUNC&&track->token.kind!=VAR){
      ifToComplete=0;
    }else if(pre!=NULL&&prePre!=NULL&&strcmp(pre->token.val,".")==0&&prePre->token.kind==SELF_KEYWORD){
      ifToComplete=0;
    }else if(track->token.kind==VAR&&hashtbl_get(&type.fields,&(track->token.val),NULL)){
      ifToComplete=1;
    }else if(track->token.kind==VAR){
      ifToComplete=0;
    }else if(containsStr_StrSet(&type.funcPointerFields,track->token.val)){
      ifToComplete=1;
    }else if(containsStr_StrSet(&type.funcs,track->token.val)){
      ifToComplete=1;
    }
    //如果判断要补全，则进行补全操作
    if(ifToComplete){
      // 则进行补全
      TBNode *tmpPre = track->last;
      // 增加一个..类型的空指针
      TBNode *newSelf;  // 新自身关键字节点
      TBNode *newVisit; // 新访问符点
      // 获取一个新的tokenNode
      newSelf = getTBNode(SELF_STRING_VALUE, SELF_KEYWORD);
      newVisit = getTBNode(".", OP);
      newSelf->next = newVisit;
      newVisit->last = newSelf;
      tmpPre->next = newSelf;
      newSelf->last = tmpPre;
      track->last = newVisit;
      newVisit->next = track;
    }
    track=track->next;
    if(track!=NULL) pre=track->last;
    if(pre!=NULL) prePre=pre->last;
  }
  nodes=head.next;
  nodes->last=NULL;
  return nodes;
}



//对nodeds进行处理,成功返回非0值,失败返回0
TBNode* process_singleLine(FuncTranslator* funcTranslator,TBNode* nodes){
  if(nodes==NULL) return NULL;
  //首先判断类型
  FTK kind=getTokenLineKind(funcTranslator,nodes);
  if(kind==NOT_LEAGAL_FTK){
    del_tokenLine(nodes);
    return NULL;
  }
  if(tranFuncs[kind]!=NULL)
    nodes=tranFuncs[kind](funcTranslator,nodes);
  return nodes;
}


//翻译结束释放函数翻译器
int release_funcTranslator(FuncTranslator* funcTranslator){
  //删除量表以及内嵌的类型表
  while(funcTranslator->globalValTbl!=NULL){
    ValTbl* tmpTbl=funcTranslator->globalValTbl;
    funcTranslator->globalValTbl=tmpTbl->next;
    del_valTbl(tmpTbl);
    free(tmpTbl);
  }
  del_functbl(funcTranslator->funcTbl); //删除函数表
  free(funcTranslator->funcTbl);
  free(funcTranslator->gloabalTypeTbl);
  funcTranslator->funcTbl=NULL;
  funcTranslator->gloabalTypeTbl=NULL;
  funcTranslator->partialValTbl=NULL;
  funcTranslator->curFunc=NULL;
  if(funcTranslator->warningFout!=stdout){
    fclose(funcTranslator->warningFout);
    funcTranslator->warningFout=stdout;
  }
  return 1;
}



//判断句子的翻译类型,以选择不同的翻译语句
FTK getTokenLineKind(FuncTranslator* funcTranslator,TBNode* nodes){
  if(nodes==NULL) return NOT_TRANSLATE_FTK;
  if(funcTranslator->curFunc==NULL) return NOT_TRANSLATE_FTK;
  
  //判断是否是不用进行方法调用翻译的句子
  if(ifNotNeedFuncTranslate(funcTranslator,nodes)){
    return NOT_TRANSLATE_FTK;
  }
  //如果出现了判断异常
  if(funcTranslator->judSentenceKindErr){
    //处理异常,并返回对判断:句子不合语法
    funcTranslator->judSentenceKindErr=0;
    return NOT_LEAGAL_FTK;
  }
  

  //判断是否是typedef语句
  if(nodes->token.kind==TYPEDEF_KEYWORD){
    return TYPEDEF_FTK;
  }

  //判断是否是类型强转语句，TODO
  if(isTypeChangeSentence(funcTranslator,nodes)){
    return TYPE_CHANGE_FTK;
  }
  //如果出现了判断异常
  if(funcTranslator->judSentenceKindErr){
    //处理异常,并返回对判断:句子不合语法
    funcTranslator->judSentenceKindErr=0;
    return NOT_LEAGAL_FTK;
  }

 
  //判断是否是变量定义语句,这里的变量可能指变量数组
  if(nodes->token.kind==TYPE&&nodes->next!=NULL&&nodes->next->token.kind==VAR){
    return VAR_DEFINE_FTK;
  }
  
  //判断是否是常量定义语句
  if(nodes->token.kind==CONST_KEYWORD){
    if(nodes->next!=NULL&&nodes->next->token.kind==TYPE) {
      return CONST_DEFINE_FTK;
    }
    else{
      return NOT_LEAGAL_FTK;
    }
  }
  
  //判断是否是函数调用语句
  if(nodes->token.kind==FUNC){
    return FUNC_USE_FTK;
  }

  //判断是否是成员属性调用
  if(nodes->token.kind==VAR&&nodes->next!=NULL&&
    strcmp(nodes->next->token.val,".")==0&&
    nodes->next->next!=NULL
    &&nodes->next->next->token.kind==VAR
  ){
    return MEMBER_FIELD_USE_FTK;
  }
  //判断是否是成员方法调用
  if(nodes->token.kind==VAR&&
    nodes->next!=NULL
    &&strcmp(nodes->next->token.val,".")==0
    &&nodes->next->next!=NULL
    &&nodes->next->next->token.kind==FUNC
  ){
    return MEMBER_FUNCTION_USE_FTK;
  }

  if(isArrVisitSentence(funcTranslator,nodes)){
    return ARR_VISIT_FTK;
  }
  //如果出现了判断异常
  if(funcTranslator->judSentenceKindErr){
    //处理异常,并返回对判断:句子不合语法
    funcTranslator->judSentenceKindErr=0;
    return NOT_LEAGAL_FTK;
  }

  //判断是否是函数指针定义语句,如果是函数指针定义语句,则进行函数指针定义语句的处理
  if(isFuncPointerDefSentence(funcTranslator,nodes)){
    return FUNCPOINTER_DEF_FTK;
  }
  //如果出现了判断异常
  if(funcTranslator->judSentenceKindErr){
    //处理异常,并返回对判断:句子不合语法
    funcTranslator->judSentenceKindErr=0;
    return NOT_LEAGAL_FTK;
  }

  //判断是否是赋值语句,要进行类型匹配检查
  if(isAssignSentence(funcTranslator,nodes)){
    return ASSIGN_FTK;
  }
  if(funcTranslator->judSentenceKindErr){
    //处理异常,并返回对判断:句子不合语法
    funcTranslator->judSentenceKindErr=0;
    return NOT_LEAGAL_FTK;
  }

  //否则是复合运算语句,对运算进行分解
  return COUNT_FTK;
}


//判断是否是不需要方法调用翻译的句子
int ifNotNeedFuncTranslate(FuncTranslator* translator,TBNode* nodes){
  if(nodes==NULL) return 1;
  //如果一个句子里面全部都是界符,则这个句子是不用翻译的
  //首先进行一次遍历,取出所有符号的类型
  //然后使用位运算来判断该符号是否正确
  //比如说0000_0000八个位置表示八种符号
  //而我们的句子的符号种类分布是 1100_0000，1表示有
  //要判断是否全部符号都在0001_1111内
  //则取反后者1110_0000,然后拿后者与前者做与运算
  //如果结果不为0,则说明目标句子含有其他符号,否则没有

  //如果只有运算表达式和的符号,则是不需要方法调用翻译的句子
  BitMapUtil bmu={
    .mapSize=sizeof(long long)
  };
  BitMap bm=getBitMap(&bmu);
  TBNode* track=nodes;
  while(track!=NULL){
    put_bitmap(&bmu,&bm,track->token.kind);
    track=track->next;
  }
  long long mode=COUNT_BITMAP|CONTROL_KEYWORD_BITMAP|SEP_BITMAP;
  long long cur;
  memcpy(&cur,bm.map,sizeof(cur));
  delBitMap(&bm);
  //如果nodes中含有mode以外的token类型,则不是不需要进行yjhc->c翻译的句子
  if(cur&(~mode)){
    return 0;
  }
  return 1;
}


//判断子函数
//判断是否是函数指针定义语句
int isFuncPointerDefSentence(FuncTranslator* funcTranslator,TBNode* nodes){
  //如果是函数指针定义语句,则格式入 type ( * fpName) (type1,type2,...)
  int state=0;
  while(nodes!=NULL){
    if(state==0){
      if(nodes->token.kind!=TYPE) return 0;
      else state=1;
    }
    else if(state==1){
      if(nodes->token.kind!=LEFT_PAR) return 0;
      else state=2;
    }
    else if(state==2){
      if(strcmp(nodes->token.val,"*")!=0) return 0;
      else state=3;
    }
    else if(state==3){
      if(nodes->token.kind!=RIGHT_PAR) return 0;
      else state=4;
    }
    else if(state==4){
      if(nodes->token.kind!=LEFT_PAR) return 0;
      else state=5;
    }
    else if(state==5){
      if(nodes->token.kind==RIGHT_PAR){
        state=6;  //6标记为终结状态
      }
      else if(nodes->token.kind==TYPE){
        state=7;
      }
      else return 0;
    }
    else if(state=6){
      //6是终结状态，到了函数指针定义语句的结尾，遇到什么token都是错误
      return 0;
    }
    //状态7为参数类型列表中参数类型后状态，期待逗号或右括号
    else if(state==7){
      if(nodes->token.kind==COMMA) state=8;
      else if(nodes->token.kind==RIGHT_PAR) state=6;
      else return 0;
    }
    //状态8为逗号后状态
    else if(state==8){
      if(nodes->token.kind==TYPE) state=7;
      else return 0;
    }
    else{
      //其他状态，说明是异常状态，停机并返回异常值
      return 0;
    }
    nodes=nodes->next;
  }
  if(state==6) return 1;
  return 0;
}


//判断是否是赋值语句,如果是赋值语句,则支持连续赋值语句
int isAssignSentence(FuncTranslator* funcTranslator,TBNode* nodes){
  //准备内容
  // 格式为:var=val,var2=val,...var3=val
  //准备个自动机来判断是否是赋值语句
  int state=0;
  const int fail_state=INT_MAX;
  const int accept_state=INT_MAX-1;
  int toNum=0;  //统计目的变量的数量
  while(nodes!=NULL&&state!=fail_state){
    //初始状态仅仅可能遇到VAR
    if(state==0&&nodes->token.kind==VAR){
      state=1;
    } 
    //读到var之后进入状态1,可能遇到,或者=
    //遇到','进入状态0,期待遇到下一个变量
    else if(state==1&&nodes->token.kind==COMMA){
      state=0;
      toNum++;
    }
    //遇到=号进入状态2,
    else if(state==1&&strcmp(nodes->token.val,"=")==0){
      state=accept_state;
    }
    //进入读取值状态
    else if(state==accept_state){
      //读取到逗号为止
      int count=0;
      if(nodes->token.kind==COMMA){
        state=fail_state;
        break;
      }
      toNum--;
      do{
        TokenKind kind=nodes->token.kind;
        if(kind==RIGHT_PAR||kind==RIGHT_BRACE||kind==RIGHT_BRACKET) count--;
        else if(kind==LEFT_PAR||kind==LEFT_BRACE||kind==LEFT_BRACKET) count++;
        else if(kind==COMMA&&count==0) break;
        nodes=nodes->next;
      }while(nodes!=NULL);
      if(nodes==NULL) break;
    }
    else{
      funcTranslator->judSentenceKindErr=1;
      return 0;
    }
    nodes=nodes->next;
  }
  //如果是异常情况停机
  if(state!=accept_state||toNum!=0){
    funcTranslator->judSentenceKindErr=1;
    return 0;
  }
  return 1;
}

//判断是否是数组成员访问
int isArrVisitSentence(FuncTranslator* funcTranslator,TBNode* nodes){
  if(nodes->token.kind!=VAR) return 0;
  nodes=nodes->next;
  if(nodes->token.kind!=LEFT_BRACKET) return 0;
  nodes=nodes->next;
  //获取中间内容
  int leftBra=1;
  while(nodes!=NULL){
    if(nodes->token.kind==LEFT_PAR||nodes->token.kind==LEFT_BRACKET||nodes->token.kind==LEFT_BRACE)
    {
      leftBra++;
    }
    else if(nodes->token.kind==RIGHT_PAR||nodes->token.kind==RIGHT_BRACE||nodes->token.kind==RIGHT_BRACKET)
    {
      leftBra--;
    }
    if(leftBra==0) break;
    nodes=nodes->next;
  }
  if(leftBra!=0){
    funcTranslator->judSentenceKindErr=1;
    return 0;
  }
  return 1;
}

//判断是否是类型强转语句
int isTypeChangeSentence(FuncTranslator* funcTranslator,TBNode* nodes){
  if(nodes==NULL) return 0;
  if(nodes->token.kind!=LEFT_PAR) return 0;
  nodes=nodes->next;
  if(nodes==NULL||nodes->token.kind!=TYPE) return 0;
  nodes=nodes->next;
  if(nodes==NULL||nodes->token.kind!=RIGHT_PAR) return 0;
  return 1;
}


//翻译功能子代码,翻译成功返回非NULL,翻译失败返回NULL


//翻译成员属性访问语句
TBNode* translateMemberFieldVisit(FuncTranslator* funcTranslator,TBNode* tokens){
  //需要保证传给这个函数的tokens一定是对应类型的
  //首先第一个量一定是VAR,否则则是异常情况
  //首先获取变量的类型
  Type type;
  Val val;
  int retLayer;
  char* newTypeName=NULL;  //新变量的类型名
  //TODO 如果表没有找到这个量，可能是因为是c标准库里定义的结构体量,因为还没有加载c库的定义
  if(!findVal(funcTranslator->partialValTbl,tokens->token.val,&val,&type,&retLayer)){
    //TODO 异常情况
    del_tokenLine(tokens);
    return NULL;
  }

  //否则如果找到的类型为结构体类型或者枚举类型的话
  if((type.kind==TYPE_STRUCT||type.kind==TYPE_UNION)
    &&(
      (retLayer==0&&strcmp(tokens->next->token.val,".")==0)
      ||
      (retLayer==1&&strcmp(tokens->next->token.val,"->")==0)
    )
  ){
    
    //获取该属性类型名,如果该类型没有该属性可访问,进行报错提示
    if(!hashtbl_get(&type.fields,&(tokens->next->next->token.val),&newTypeName)){
      fprintf(funcTranslator->warningFout,"visit unexist field %s of %s\n",tokens->next->next->token.val,tokens->token.val); 
      del_tokenLine(tokens);
      return NULL;
    }
  }
  //如果是其他类型，比如未知类型，则可能是标准库中的成员访问;又或者进行了错误指针层次的访问
  else{
    //暂时当作异常处理
    del_tokenLine(tokens);
    return NULL;
  }

  TBNode *tail = tokens->next->next;
  TBNode *sufTail = tail->next;
  tail->next = NULL;
  if (sufTail != NULL)
    sufTail->last = NULL;
  // 合并表达式
  TBNode *out = connect_tokens(tokens, VAR, "");
  // 如果合并异常
  if (out == NULL)
  {
    del_tokenLine(sufTail);
    return NULL;
  }
  // 检查合并结果是否是孤立节点
  if (out->next != NULL || out->last != NULL)
  {
    del_tokenLine(out);
    return NULL;
  }
  // 否则合并成功,
  
  //首先连接合成表达式和后面剩余节点
  out->next=sufTail;
  sufTail->last=out;
  //然后注册表达式字符值为目标类型
  addVal_valtbl(funcTranslator->partialValTbl,out->token.val,NULL,0,newTypeName,0);

  return process_singleLine(funcTranslator ,out);
}

//数组元素访问语句
TBNode* translateArrVisit(FuncTranslator* funcTranslator,TBNode* tokens){
  //首先截取后面两个方括号之间的内容
  TBNode* head=tokens->next->next;
  TBNode* preHead=head->last;
  TBNode* sufTail=head;
  int leftP=1;
  while(sufTail!=NULL){
    TokenKind kind=sufTail->token.kind;
    if(kind==LEFT_BRACE||kind==LEFT_PAR||kind==LEFT_BRACKET){
      leftP++;
    }
    else if(kind==RIGHT_BRACE||kind==RIGHT_BRACKET||kind==RIGHT_PAR)
    {
      leftP--;
    }
    if(leftP==0) break;
    sufTail=sufTail->next;
  }
  if(sufTail==NULL){
    del_tokenLine(tokens);
    return NULL;
  }
  //否则如果下标表达式为空,也就是方括号内为空内容,也就是非法情况
  else if(sufTail==head){
    del_tokenLine(tokens);
    return NULL;
  }
  //然后处理下标表达式,事实下标表达式应该是一个结果为整型变量的运算
  
  //首先分离出下标表达式
  head->last=NULL;
  preHead->next=NULL;
  sufTail->last->next=NULL;
  sufTail->last=NULL;


  //然后处理下标表达式
  head=process_singleLine(funcTranslator,head);
  //如果下标表达式分析错误
  if(head==NULL){
    del_tokenLine(tokens);
    del_tokenLine(sufTail);
    return NULL;
  }
  //否则判断下标表达式的类型
  Type type;
  Val val;
  int layer;
  //从表中搜索获取下标表达式类型
  findVal(funcTranslator->partialValTbl,head->token.val,&val,&type,&layer);
  //判断表达式的类类型
  //如果下标不是int类型或者不是long long类型,则为错误的下标表达式,返回异常值
  if(layer!=0||type.kind!=TYPE_INT){
    del_tokenLine(tokens);
    del_tokenLine(sufTail);
    del_tokenLine(head);
    return NULL;
  }
  //否则是正确类型的表达式,合并前后内容
  TBNode* sufSufTail=sufTail->next;
  preHead->next=head;
  head->last=preHead;
  head->next=sufTail;
  sufTail->last=head;

  //分离数组元素访问表达式,
  sufTail->next=NULL;
  if(sufSufTail!=NULL) sufSufTail->last=NULL;
  
  //处理数组访问表达式,数组表达式处理完后应该是一个var
  tokens=connect_tokens(tokens,VAR,"");

  //合并处理后的数组访问表达式和之前分离的内容
  tokens->next=sufSufTail;
  if(sufSufTail!=NULL) sufSufTail->last=tokens;

  //加入量表
  layer--;
  addVal_valtbl(funcTranslator->partialValTbl,tokens->token.val,NULL,0,type.defaultName,layer);

  //然后返回对剩下部分继续翻译的结果
  return process_singleLine(funcTranslator,tokens);
}


//翻译类型强转语句
TBNode* translateTypeChange(FuncTranslator* funcTranslator,TBNode* tokens){
  //首先第一个获取类型名
  char* typeName=tokens->next->token.val;

  //查找类型
  Type type;
  int layer;
  findType_valtbl(funcTranslator->partialValTbl,&type,&layer);

  //然后处理后面表达式
  TBNode* preHead=tokens->next->next;
  TBNode* head=tokens->next->next->next;

  //首先切割出后面表达式
  preHead->next=NULL;
  head->last=NULL;

  head=process_singleLine(head);  //后面应该是运算表达式
  if(head==NULL){
    del_tokenLine(tokens);
    return NULL;
  }


  //连接回后面表达式

  preHead->next=head;
  head->last=preHead;

  //然后把后面表达式合并表达成新类型
  tokens=connect_tokens(tokens,VAR,"");
  //加入变量表
  addVal_valtbl(funcTranslator->partialValTbl,tokens->token.val,NULL,0,type.defaultName,layer);
  return tokens;
}

//翻译变量定义语句
TBNode* translateVarDef(FuncTranslator* funcTranslator,TBNode* tokens){
  //首先获取类型
  char* typeName=tokens->token.val;
  Type type;
  int typeLayer;
  //查找类型
  findType_valtbl(funcTranslator->partialValTbl,typeName,&type,&typeLayer);

  /*变量定义语句可能的形式与效果
  int* a,b,*c;结果a->int*,b->int,c->int*
  int arr[2][3]; arr->const int**
  int a=2,b;
  int arr[]={2,3,4};
  */
 //TODO


  
  //合并tokens为const
  tokens=connect_tokens(tokens,CONST," ");
  return tokens;
}

//翻译常量定义语句
TBNode* translateConstDef(FuncTranslator* functranslator,TBNode* tokens){
  /*
  类似于变量定义语句,不过是把变量加入为常量
  */

  return tokens;
}

//翻译运算语句
TBNode* translateCountDef(FuncTranslator* functranslator,TBNode* tokens){
  //TODO

  return tokens;
}

//函数指针定义语句
TBNode* translateFuncPointerDef(FuncTranslator* functranslator,TBNode* tokens){

}

//typedef命名类型别名语句
TBNode* translateTypedef(FuncTranslator* functranslator,TBNode* tokens){

}

//翻译函数调用语句
TBNode* translateFuncUse(FuncTranslator* functranslator,TBNode* tokens){
  //TODO

  return tokens;
}

//翻译赋值语句
TBNode* translateAssign(FuncTranslator* functranslator,TBNode* tokens){
  //TODO

  return tokens;
}






//翻译类型方法调用语句
TBNode* translateTypeMethodUse(FuncTranslator* functranslator,TBNode* tokens){

  //则第一个位置就是量,对这个量进行翻译



}

