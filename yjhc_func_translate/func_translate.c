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
  *(out.gloabalTypeTbl)=getGlobalTypeTbl();
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

    if(nodes==NULL){
      isRight=0;
      break;
    }

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
  //如果是错误的内容
  if(!isRight){
    fprintf(funcTranslator->warningFout,"\nsyntax error!\n");
  }
  if(!isRight) return 0;
  return 1;
}


//补全成员方法中自身方法或者属性的调用
TBNode* member_use_complement(FuncTranslator* funcTranslator,TBNode* nodes){
  //如果不是成员方法则不用补全类型
  if(funcTranslator->curFunc==NULL) return nodes;
  char* ownerName=funcTranslator->curFunc->owner;
  if(ownerName==NULL) return nodes; //如果函数没有主人名,则不是自身调用函数
  //取出函数主人，找到对应类型
  int typeIndex=findType(funcTranslator->gloabalTypeTbl,ownerName,NULL);
  Type type;
  if(typeIndex!=0){
    vector_get(&funcTranslator->gloabalTypeTbl->types,typeIndex,&type);
  }
  //定义遍历暂存区域
  TBNode* track=nodes;
  TBNode* pre=NULL;
  TBNode* prePre=NULL;
  TBNode head;
  head.next=nodes;
  nodes->last=&head;
  //开始搜索可能要补全的位置
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
  if(nodes->token.kind>Tokens_NUM) return NOT_TRANSLATE_FTK;    //如果是一个完整的句子,不用翻译
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
  //判断是否是自身属性调用语句,self.var的形式
  if(nodes->token.kind==SELF_KEYWORD){
    if(nodes->next->token.kind==OP&&strcmp(nodes->next->token.val,".")==0){
      if(nodes->next->next->token.kind==VAR) return SELF_FIELD_VISIT_FTK;
      else if(nodes->next->next->token.kind==FUNC) return SELF_FUNC_VISIT_FTK;
      else return NOT_LEAGAL_FTK;
    }
    else return NOT_LEAGAL_FTK;
  }

  //判断是否是类型强转语句
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

  //如果是单个已知变量,则是不需要翻译的类型
  if(nodes->next==NULL&&nodes->token.kind==VAR){
    return 1;
  }


  //如果只有运算表达式和常量符号或者界符号,而且没有属性访问语句,则是不需要方法调用翻译的句子
  BitMapUtil bmu={
    .mapSize=sizeof(long long)
  };
  BitMap bm=getBitMap(&bmu);
  TBNode* track=nodes;
  while(track!=NULL){
    int ifRet=0;
    TokenKind kind=track->token.kind;
    // if(kind==LEFT_BRACE||kind==LEFT_PAR||kind==LEFT_BRACKET
    // ){
    //   ifRet=1;
    // }
    // else if(track->token.kind==OP&&strcmp(track->token.val,".")==0){
    //   ifRet=1;
    // }
    // if(ifRet){
    //   delBitMap(&bm);
    //   return 0;
    // }
    put_bitmap(&bmu,&bm,track->token.kind);
    track=track->next;
  }
  long long mode=CONTROL_KEYWORD_BITMAP|SEP_BITMAP|(1<<CONST);
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
      toNum++;
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
  if(nodes==NULL||nodes->token.kind!=LEFT_BRACKET) return 0;
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
//独立出一个参数检查功能，检查匹配返回非０值，检查不匹配返回０
//这个函数里面不允许对分配的argNodes进行回收工作
int checkArgs(FuncTranslator* transLator,Func* thisFunc,TBNode* argNodes){
  //argNodes内容是(arglist)
  //以左括号开头,右括号结尾
  //能够从函数种获取函数名字和参数列表
  int argIndex=0;
  TokenKind ends[]={COMMA,RIGHT_PAR};
  int endsSize=2;
  //对第一个参数往后面循环读取参数
  //开始的时候argNodes处于开头的左括号位置
  //当参数分析没有读取到参数列表的尽头的时候就循环读取
  while(argNodes!=NULL&&argIndex<thisFunc->args.size){
    //读取直到逗号或者右括号结尾
    TBNode* tail=NULL;
    TBNode* preHead=NULL;
    TBNode* head=argNodes->next;;
    TBNode* sufTail=NULL;
    //如果第argIndex个参数搜索失败
    if(!searchExpressUntil(head,&tail,ends,endsSize)||tail==NULL||tail->next==NULL||head->token.kind==COMMA||head->token.kind==RIGHT_PAR){
      fprintf(transLator->warningFout,"%d arg not found in use of func %s:\n\t",argIndex+1,thisFunc->func_name);
      return 0;
    }
    //否则搜索参数成功,取出参数表达式进行处理
    preHead=head->last;
    sufTail=tail->next;
    
    preHead->next=sufTail;
    head->last=NULL;
    tail->next=NULL;
    sufTail->last=preHead;

    //处理参数表达式
    head=process_singleLine(transLator,head);
    //如果处理失败
    if(head==NULL){
      fprintf(transLator->warningFout,"fail to recognized %d arg in use of func %s\n",argIndex+1,thisFunc->func_name);
      return 0;
    }

    //否则处理成功,首先连接回来,然后用处理成功的式子判断是否是匹配的
    sufTail->last=head;
    head->next=sufTail;
    head->last=preHead;
    preHead->next=head;

    //保存形参类型信息
    Arg arg;
    Type argType;
    int argTypeLayer;
    //定义保存实参类型信息
    Val val;
    Type type;
    int typeLayer;
    //获取形参类型
    vector_get(&thisFunc->args,argIndex,&arg);
    //获取形参类型名,通过typeId
    if(!findTypeById(transLator->gloabalTypeTbl,arg.typeId,&argType,&argTypeLayer)){
      // fprintf
      return 0;
    }
    //获取实参信息
    if(head->token.kind==CONST){
      val.isConst=1;
      if(isConstIntToken(head->token)){
        findType_valtbl(transLator->globalValTbl,"int",&type,&typeLayer);
      }else if(isConstStrToken(head->token)){
        findType_valtbl(transLator->globalValTbl,"char*",&type,&typeLayer);
      }
    }
    else if(!findVal(transLator->partialValTbl,head->token.val,&val,&type,&typeLayer)){
      return 0;
    }
    //进行类型匹配

    //如果类型不匹配
    if((arg.isConst^val.isConst)||strcmp(argType.defaultName,type.defaultName)!=0||argTypeLayer!=typeLayer){
      //首先获得两个类型全名
      char* typeName=getTypeName(type.defaultName,typeLayer);
      char* argTypeName=getTypeName(argType.defaultName,argTypeLayer);
      if(val.isConst) fprintf(transLator->warningFout,"Error! use const val %s of %s type ",head->token.val,typeName);
      else fprintf(transLator->warningFout,"Error! use var %s of %s type ",head->token.val,typeName);
      
      if(arg.isConst) fprintf(transLator->warningFout,"but not const val of %s type as the %dth arg of func %s\n",argTypeName,argIndex+1,thisFunc->func_name);
      else fprintf(transLator->warningFout,"but not var of %s type as the %dth arg of func %s\n",argTypeName,argIndex+1,thisFunc->func_name);
      free(typeName);
      free(argTypeName);
      return 0;
    }

    //否则类型匹配,更新argIndex;
    argIndex++;
    //然后进入下一个参数分析
    argNodes=head->next;
  }
  //如果形参数量与实参数量不匹配
  //如果实参数量小于形参数量,(ps:正常匹配的时候的结束的时候argIndex应该等于形参数量<thisFunc->args.size))
  if(argIndex<thisFunc->args.size){
    fprintf(transLator->warningFout,"miss arguments in use of func %s",thisFunc->func_name);
    if(thisFunc->owner!=NULL) fprintf(transLator->warningFout,"of %s type",thisFunc->owner);
    fprintf(transLator->warningFout,"\n");
    return 0;
  }
  //如果实际参数的数量大于形参的数量,也就是没有读到实际参数结束就已经匹配完所有形参的情况
  else if(argNodes->token.kind!=RIGHT_PAR){
    fprintf(transLator->warningFout,"too many arguments of func %s",thisFunc->func_name);
    if(thisFunc->owner!=NULL) fprintf(transLator->warningFout,"of %s type",thisFunc->owner);
    fprintf(transLator->warningFout,"\n");
    return 0;
  }
  //否则就是参数数量正常的情况,分析正常结束,参数列表匹配正确
  return 1;
}

//翻译成员属性访问语句
TBNode* translateMemberFieldVisit(FuncTranslator* funcTranslator,TBNode* tokens){
  //需要保证传给这个函数的tokens一定是对应类型的
  //首先第一个量一定是VAR,否则则是异常情况
  //首先获取变量的类型
  Type type;
  Val val;
  int retLayer;
  char* newTypeName=NULL;  //新变量的类型名
  //如果表没有找到这个量，可能是因为是c标准库里定义的结构体量,因为还没有加载c库的定义
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
  //首先判断是否是常量数组
  int isConst=0;
  {
    int isRight=1;
    char* arrName=tokens->token.val;
    Val val;
    int layer;
    if(!findVal(funcTranslator->partialValTbl,arrName,&val,NULL,&layer)){
      fprintf(funcTranslator->warningFout,"undefined arr name %s in:\n\t",arrName);
      isRight=0;
    }
    else if(layer<1){
      fprintf(funcTranslator->warningFout,"not arr name %s for arr visit in:\n\t",arrName);
      isRight=0;
    }
    if(!isRight){
      fshow_tokenLine(funcTranslator->warningFout,tokens);
      del_tokenLine(tokens);
      return NULL;
    }
    isConst=val.isConst;
  }
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
    fprintf(funcTranslator->warningFout,"warning!can not visit arr element with empty index!\n");
    del_tokenLine(tokens);
    return NULL;
  }
  //然后处理下标表达式,事实下标表达式应该是一个结果为整型变量的运算
  
  //首先分离出下标表达式
  head->last=NULL;
  preHead->next=sufTail;
  sufTail->last->next=NULL;
  sufTail->last=preHead;


  //然后处理下标表达式
  head=process_singleLine(funcTranslator,head);
  //如果下标表达式分析错误
  if(head==NULL){
    del_tokenLine(tokens);
    return NULL;
  }
  //连接回原来的接口
  preHead->next=head;
  head->last=preHead;
  head->next=sufTail;
  sufTail->last=head;
  //判断下标表达式的类型
  Type type;
  Val val;
  int layer;
  int isRight=1;
  //如果下标表达式是整数常量,则是正确的下标表达式
  if(head->token.kind==CONST){
    if(!isConstIntToken(head->token)){
      isRight=0;
      fprintf(funcTranslator->warningFout,"index to visit arr should be int,but not const string %s\n",head->token.val);
    }
  }
  //从表中搜索获取下标表达式的变量信息,如果查找不到,说明出现异常
  else if(!findVal(funcTranslator->partialValTbl,head->token.val,&val,&type,&layer)){
    //如果是莫名变量,提示,然后返回错误信息
    fprintf(funcTranslator->warningFout,"var %s used but not defined!\n",head->token);  //使用未定义变量
    isRight=0;
  }
  //判断表达式的类类型
  //如果下标不是int类型或者不是指针层次为0,则为错误的下标表达式,返回异常值
  else if(layer!=0||type.kind!=TYPE_INT){
    char* typeName=getTypeName(type.defaultName,layer);
    fprintf(funcTranslator->warningFout,"you must use int val to visit arr elements but not val %s of %s type\n",head->token.val,typeName);
    free(typeName);
    isRight=0;
  }
  if(!isRight){
    del_tokenLine(tokens);
    return NULL;
  }
  //否则是正确类型的表达式

  //TODO,然后获取数组名的指针层次
  if(!findVal(funcTranslator->partialValTbl,preHead->last->token.val,&val,&type,&layer)){
    fprintf(funcTranslator->warningFout,"unknown arr %s!\n",preHead->last->token.val);
    isRight=0;
  }
  else if(layer<1){
    fprintf(funcTranslator->warningFout,"visit un-arr val %s!\n",preHead->last->token.val);
    isRight=0;
  }
  if(!isRight){
    del_tokenLine(tokens);
    return NULL;
  }
  
  //加入后面的点
  TBNode* sufSufTail=sufTail->next;



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

  addVal_valtbl(funcTranslator->partialValTbl,tokens->token.val,NULL,isConst,type.defaultName,layer);

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
  findType_valtbl(funcTranslator->partialValTbl,typeName,&type,&layer);

  //然后处理后面表达式
  TBNode* preHead=tokens->next->next;
  TBNode* head=tokens->next->next->next;

  //首先切割出后面表达式
  preHead->next=NULL;
  head->last=NULL;

  head=process_singleLine(funcTranslator,head);  //后面应该是运算表达式
  if(head==NULL){
    del_tokenLine(tokens);
    return NULL;
  }
  //连接回后面表达式

  preHead->next=head;
  head->last=preHead;

  //然后把后面表达式合并成变量类型
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
  StrSet constArrName=getStrSet(myStrHash);
  //查找类型
  findType_valtbl(funcTranslator->partialValTbl,typeName,&type,&typeLayer);

    /*变量定义语句可能的形式与效果
    int* a,b,*c;结果a->int*,b->int,c->int*
    int arr[2][3]; arr->const int**
    int a=2,b;
    int arr[]={2,3,4};
    */
  vector vars=getVector(sizeof(char*));
  vector layers=getVector(sizeof(int));
  //首先,获取第一个定义,
  TBNode* track=tokens->next;
  int isFirst=1;  //用来判断是否是该语句定义的第一个变量
  int state=0;  //用来指导翻译动作
  int isRight=1;
  //然后获取后面的定义
  while (track!=NULL)
  {
    //状态0的时候遇到新量符号
    if(state==0&&track->token.kind==VAR){
      //判断是否是第一次个定义变量,如果不是统计前面的*数量
      int layer=0;
      if(isFirst){
        layer=typeLayer;
        isFirst=0;
      }else{
        //统计前面的*数量
        TBNode* countM=track->last;
        while(countM!=NULL&&countM->token.kind!=TYPE&&countM->token.kind!=COMMA){
          layer++;
          countM=countM->last;
        }
      }
      char* valName=track->token.val;
      vector_push_back(&vars,&valName);
      vector_push_back(&layers,&layer);
      state=1;
    }
    else if(state==0&&track->token.kind==OP&&strcmp(track->token.val,"*")==0){
      // state=0;  //状态保持0不变
    }
    //状态1的时候是遇到量符号后,期待方括号,逗号,等号
    else if(state==1){
      if(track->token.kind==COMMA) state=0;
      else if(track->token.kind==LEFT_BRACKET) state=3;
      else if(track->token.kind==OP&&strcmp(track->token.val,"=")==0){
        state=2;
      }
      else{
        isRight=0;
        break;
      }
    }
    //状态2是遇到等号之后,读取一个值表达式并处理
    else if(state==2){
      //读取到,结束或者NULL结束
      TBNode* tail;
      TBNode* sufTail;
      TBNode* head;
      TBNode* preHead;

      //搜索表达式直到遇到
      TokenKind kinds[]={COMMA };
      if(!searchExpressUntil(track,&tail,kinds,1)){
        isRight=0;
        break;
      }
      head=track;
      preHead=head->last;
      sufTail=tail->next;
      
      //分离值表达式,对值表达式进行处理
      preHead->next=NULL;
      head->last=NULL;
      tail->next=NULL;
      if(sufTail!=NULL) sufTail->last=NULL;
      //处理值表达式
      head=process_singleLine(funcTranslator,head);
      if(head==NULL){
        preHead->next=sufTail;
        if(sufTail!=NULL) sufTail->last=preHead;
        isRight=0;
        break;
      }
      //如果处理成功,接回链表
      preHead->next=head;
      head->last=preHead;
      head->next=sufTail;
      if(sufTail!=NULL)
        sufTail->last=head;
      //处理完值表达式进入句子
      if(sufTail==NULL){
        state=1;
        break;
      }else{
        track=sufTail;
        state=0;
      }
    }
    //state3是状态1遇到方括号之后,读取一个维度大小表达式并处理
    else if(state==3){
      //把该量名加入值表达式
      char* cName=NULL;
      vector_get(&vars,vars.size-1,&cName);
      addStr_StrSet(&constArrName,cName);

      //读取方括号进行处理,并进入状态1
      TBNode* head=track;
      TBNode* preHead=head->last;
      TBNode* tail=NULL;
      TBNode* sufTail=NULL;
      if(!searchBracketExpression(track->last,&head,&tail)){
        isRight=0;
        break;
      }
      //读取成功后head,tail的为止分别是左右花括号

      //根据方括号表达式获得方括号内维度大小表达式
      preHead=head;
      head=preHead->next;
      sufTail=tail;
      tail=sufTail->last;

      //分离方括号内表达式,也就是分离出维度大小表达式
      preHead->next=NULL;head->last=NULL;
      sufTail->last=NULL;tail->next=NULL;
      
      //对该表达式分析
      head=process_singleLine(funcTranslator,head);
      if(!head){
        sufTail->last=preHead;
        preHead->next=sufTail;
        isRight=0;
        break;
      }
      //分析成功连接上
      preHead->next=head;
      head->last=preHead;
      head->next=sufTail;
      sufTail->last=head;
      //分析维度表达式内的内容是否符合要求
      if(head->token.kind==VAR){
        //获取量的类型,判断是否是常量
        //TODO,查找这个量,必须是常整型的量
        Val val;
        Type type;
        int typeLayer;
        if(!findVal(funcTranslator->partialValTbl,head->token.val,&val,&type,&typeLayer)){
          fprintf(funcTranslator->warningFout,"var %s used but not defined\n",head->token.val);
          isRight=0;
        }
        else if(typeLayer!=0||type.kind!=TYPE_INT||!val.isConst
        ){
          fprintf(funcTranslator->warningFout,"used %s as arr dimension but not const int val\n",head->token.val);
          isRight=0;
        }
        if(!isRight) break;
      }else if(head->token.kind!=CONST){
        fprintf(funcTranslator->warningFout,"arr definition should use const int val,but not %s\n",head->token.val);
        isRight=0;
        break;
      }else{
        //TODO
      }
      //对指针层次进行更新
      int layer;
      vector_get(&layers,layers.size-1,&layer);
      layer++;
      vector_set(&layers,layers.size-1,&layer,NULL);

      // vector_get(&layers,layers.size-1,&layer);
      state=1;
      track=sufTail;
    }
    //异常情况
    else{
      isRight=0;
      break;
    }
    track=track->next;
  }
  //然后合并整个句子为类型定义句子
  //值表达式缺失
  if(state==2){
    fprintf(funcTranslator->warningFout,"miss val for inition in ");
    fshow_tokenLine(funcTranslator->warningFout,tokens);
    fprintf(funcTranslator->warningFout,"\n");
    isRight=0;
  }
  //数组括号定义异常
  else if(state==3){
    fprintf(funcTranslator->warningFout,"syntax error!un complete definition of arr in ");
    fshow_tokenLine(funcTranslator->warningFout,tokens);
    fprintf(funcTranslator->warningFout,"\n");
    isRight=0;
  }
  if(!isRight){
    del_tokenLine(tokens);
    vector_clear(&layers);
    vector_clear(&vars);
    initStrSet(&constArrName);
    return NULL;
  }
  
  //加入变量表
  for(int i=0;i<vars.size;i++){
    //取出名
    char* valName=NULL;
    int layer;
    vector_get(&vars,i,&valName);
    //取出层次
    vector_get(&layers,i,&layer);

    int isConst=0;
    //TODO,进行是否常量判断,如果这个id是变量数组名,则设置为对应的指针常量
    if(containsStr_StrSet(&constArrName,valName)) 
      isConst=1;

    //增加待增加变量检查
    if(!preValNameAddJudge(funcTranslator,valName)){
      fprintf(funcTranslator->warningFout,"fail to define var %s in:\n\t",valName);
      fshow_tokenLine(funcTranslator->warningFout,tokens);
      isRight=0;
      break;
    }
    //加入量表
    addVal_valtbl(funcTranslator->partialValTbl,valName,NULL,isConst,type.defaultName,layer);
  }
  //清除表
  vector_clear(&vars);
  vector_clear(&layers);
  //TODO,释放常id表空间
  initStrSet(&constArrName);
  if(!isRight){
    del_tokenLine(tokens); return NULL;
  }

  //合并tokens为变量定义句子
  TBNode* next=tokens->next;
  tokens->next=NULL;
  if(next!=NULL)
    next->last=NULL;
  next=connect_tokens(next,CONST,"");
  tokens->next=next;
  if(next!=NULL) next->last=tokens;
  tokens=connect_tokens(tokens,CONST," ");
  return tokens;
}

//翻译常量定义语句
TBNode* translateConstDef(FuncTranslator* funcTranslator,TBNode* tokens){
  /*
  类似于变量定义语句,不过是把变量加入为常量
  */
  //首先获取类型定义
  //首先获取类型
  char* typeName=tokens->next->token.val;
  Type type;
  int typeLayer;
  //查找类型
  if(!findType_valtbl(funcTranslator->partialValTbl,typeName,&type,&typeLayer)){
    //类型不存在的情况，TODO,报错

  }
  //TODO,常量表害需要处理初始化值


    /*常量定义语句可能的形式与效果
    const int* a,b,*c;结果a->int*,b->int,c->int*
    const int arr[2][3]; arr->const int**
    const int a=2,b;
    const int arr[]={2,3,4};
    */
  vector vars=getVector(sizeof(char*));
  vector layers=getVector(sizeof(int));
  //值表,每个常量定义的时候都要分配一个值,如果没有初始化就初始化为默认值
  vector initVals=getVector(sizeof(char*));
  //首先,获取第一个定义,
  TBNode* track=tokens->next->next;
  int isFirst=1;  //用来判断是否是该语句定义的第一个变量
  int state=0;  //用来指导翻译动作
  int isRight=1;
  //然后获取后面的定义
  while (track!=NULL)
  {
    //状态0的时候遇到新量符号
    if(state==0&&track->token.kind==VAR){
      //判断是否是第一次个定义变量,如果不是统计前面的*数量
      int layer=0;
      if(isFirst){
        layer=typeLayer;
        isFirst=0;
      }else{
        //统计前面的*数量
        TBNode* countM=track->last;
        while(countM!=NULL&&countM->token.kind!=TYPE&&countM->token.kind!=COMMA){
          layer++;
        }
      }
      //根据layer和typeName获取默认值
      char* defaultVal;
      if(!getDefaultValOfCertainType(&defaultVal,type.kind,layer)){
        isRight=0;break;
      }
      char* valName=track->token.val;
      vector_push_back(&vars,&valName);
      vector_push_back(&layers,&layer);
      //开始时,设置对应值为默认值
      vector_push_back(&initVals,&defaultVal);
      state=1;
    }
    else if(state==0&&track->token.kind==OP&&strcmp(track->token.val,"*")==0){
      // state=0;  //状态保持0不变
    }
    //状态1的时候是遇到量符号后,期待方括号,逗号,等号
    else if(state==1){
      if(track->token.kind==COMMA) state=0;
      else if(track->token.kind==LEFT_BRACKET) state=3;
      else if(track->token.kind==OP&&strcmp(track->token.val,"=")==0){
        state=2;
      }
      else{
        isRight=0;
        break;
      }
    }
    //状态2是遇到等号之后,读取一个值表达式并处理
    else if(state==2){
      //读取到,结束或者NULL结束
      TBNode* tail;
      TBNode* sufTail;
      TBNode* head;
      TBNode* preHead;

      //搜索表达式直到遇到
      TokenKind kinds[]={COMMA };
      if(!searchExpressUntil(track,&tail,kinds,1)){
        isRight=0;
        break;
      }
      head=track;
      preHead=head->last;
      sufTail=tail->next;
      
      //分离值表达式,对值表达式进行处理
      preHead->next=NULL;
      head->last=NULL;
      tail->next=NULL;
      if(sufTail!=NULL) sufTail->last=NULL;

      //连接剩余部分
      preHead->next=sufTail;
      if(sufTail!=NULL) sufTail->last=preHead;

      //处理值表达式
      head=process_singleLine(funcTranslator,head);
      if(head==NULL||head->next!=NULL||head->last!=NULL){
        del_tokenLine(head);
        isRight=0;
        break;
      }
      //如果处理成功,接回链表
      preHead->next=head;
      head->last=preHead;
      head->next=sufTail;
      if(sufTail!=NULL)
        sufTail->last=head;

      //把值表达式加入赋值表,但是这个值就是动态的值,要回收空间
      vector_set(&initVals,initVals.size-1,&(head->token.val),NULL);

      //处理完值表达式进入句子
      if(sufTail==NULL){
        state=1;
        break;
      }else{
        track=sufTail;
        state=0;
      }
    }
    //state3是状态1遇到方括号之后,读取一个维度大小表达式并处理
    else if(state==3){
      //读取方括号进行处理,并进入状态1
      TBNode* head=track;
      TBNode* preHead=head->last;
      TBNode* tail=NULL;
      TBNode* sufTail=NULL;
      if(!searchBracketExpression(track->last,&head,&tail)){
        isRight=0;
        break;
      }
      //读取成功后head,tail的为止分别是左右花括号

      //根据方括号表达式获得方括号内维度大小表达式
      preHead=head;
      head=preHead->next;
      sufTail=tail;
      tail=sufTail->last;

      //分离方括号内表达式,也就是分离出维度大小表达式
      preHead->next=NULL;head->last=NULL;
      sufTail->last=NULL;tail->next=NULL;
      
      //对该表达式分析
      head=process_singleLine(funcTranslator,head);
      if(!head){
        sufTail->last=preHead;
        preHead->next=sufTail;
        isRight=0;
        break;
      }
      //分析成功连接上
      preHead->next=head;
      head->last=preHead;
      head->next=sufTail;
      sufTail->last=head;
      //分析维度表达式内的内容是否符合要求
      int isSuitable=0;
      if(head->token.kind==CONST){
        isSuitable=1;
      }
      else if(head->token.kind==VAR){
        //获取量的类型,判断是否是常量
        //TODO,查找这个量,必须是常整型的量
        Val val;
        Type type;
        int typeLayer;
        if(findVal(funcTranslator->partialValTbl,head->token.val,&val,&type,&typeLayer)
        &&typeLayer==0&&type.kind==TYPE_INT&&val.isConst
        ){
          isSuitable=1;
        }
      }else{

      }
      //如果维度大小表达式不是合适的常量,则进行报错提示
      if(!isSuitable){
        fprintf(funcTranslator->warningFout,"arr definition should use const int val,but not %s\n",head->token.val);
      }
      //对指针层次进行更新
      int layer;
      vector_get(&layers,layers.size-1,&layer);
      layer++;
      vector_set(&layers,layers.size-1,&layer,NULL);
      state=1;
      track=sufTail;
    }
    //异常情况
    else{
      isRight=0;
      break;
    }
    track=track->next;
  }
  //然后合并整个句子为类型定义句子
  //值表达式缺失
  
  if(state==2){
    fprintf(funcTranslator->warningFout,"miss val for inition in ");
    fshow_tokenLine(funcTranslator->warningFout,tokens);
    fprintf(funcTranslator->warningFout,"\n");
    isRight=0;
  }
  //数组括号定义异常
  else if(state==3){
    fprintf(funcTranslator->warningFout,"syntax error!un complete definition of arr in ");
    fshow_tokenLine(funcTranslator->warningFout,tokens);
    fprintf(funcTranslator->warningFout,"\n");
    isRight=0;
  }
  if(!isRight){
    del_tokenLine(tokens);
    vector_clear(&vars);
    vector_clear(&initVals);
    vector_clear(&layers);
    return NULL;
  }
  
  //加入常量表
  for(int i=0;i<vars.size;i++){
    //取出名
    char* valName=NULL;
    int layer;
    char* defaultVal=NULL;
    int ifFree;
    vector_get(&vars,i,&valName);
    //取出层次
    vector_get(&layers,i,&layer);
    //取出常量初始值
    vector_get(&initVals,i,&defaultVal);
    //TODO,增加待增加常量检查
    if(!preValNameAddJudge(funcTranslator,valName)){
      fprintf(funcTranslator->warningFout,"fail to define const val %s in:\n\t",valName);
      fshow_tokenLine(funcTranslator->warningFout,tokens);
      isRight=0;
      break;
    }
    //加入量表
    addVal_valtbl(funcTranslator->partialValTbl,valName,defaultVal,1,type.defaultName,layer);
  }
  //清空三个表
  vector_clear(&vars);
  vector_clear(&layers);
  vector_clear(&initVals);
  if(!isRight){
    del_tokenLine(tokens); return NULL;
  }
  //合并tokens为变量定义句子
  TBNode* next=tokens->next;
  tokens->next=NULL;
  if(next!=NULL)
    next->last=NULL;
  next=connect_tokens(next,CONST,"");
  tokens->next=next;
  if(next!=NULL) next->last=tokens;
  tokens=connect_tokens(tokens,CONST," ");
  return tokens;
}

//翻译枚举表达式
TBNode* translateSetExp(FuncTranslator* funcTranslator,TBNode* tokens){
  //TODO
  tokens=connect_tokens(tokens,CONST," ");  //枚举表达式的值不能够用来调用方法或者作为常数
  return tokens;
}

//翻译运算语句
TBNode* translateCountDef(FuncTranslator* functranslator,TBNode* tokens){
  
  //首先小括号决定运算优先级别
  //先去除除了函数调用外的小括号,小括号的运算结果只能是量,而且一般只能够是运算常量,除非小括号里面的内容是单一变量
  tokens=removeParExp(functranslator,tokens);
  
  //然后花括号是序列表达式,对序列表达式进行分析处理,去除所有花括号
  tokens=removeSetExp(functranslator,tokens);

  //然后去除所有self.引用表达式
  tokens=removeSelfExp(functranslator,tokens);

  //然后去掉所有成员访问表达式和数组访问表达式
  tokens=removeMemVisitAndArrVisit(functranslator,tokens);

  //去除运算表达式
  tokens=removeOP(functranslator,tokens);

  return tokens;
}


//翻译运算语句的子函数

//去除括号表达式
TBNode* removeParExp(FuncTranslator* translator,TBNode* tokens){
  //去除括号表达式很简单
  

  return tokens;
}

//去除序列表达式
TBNode* removeSetExp(FuncTranslator* translator,TBNode* tokens){
  //TODO,序列表达式翻译成一个序列值,能够用来对结构体类型或者数组类型进行初始化
  //TODO,序列表达式不能够调用方法,也不能够进行操作,所以可以赋一个常量类型标志
  //TODO,然后序列表达式类型可以作为一个特殊的类型处理
  if(tokens==NULL) return NULL;

  //首先处理括号表达式里面的每个子表达式,对每个子表达式进行处理
  //当前先不考虑具名初始化的内容,假设每个子表达式都是没有具名的,也没有指定下标的
  TBNode head;
  head.token.kind=UNKNOWN;
  head.last=NULL;
  head.next=tokens;
  TBNode* track=tokens->next;
  TBNode* tail;
  TokenKind kinds[]={RIGHT_BRACE,COMMA};
  int kindsSize=2;
  int isRight=1;
  while(searchExpressUntil(track,&tail,kinds,kindsSize)){
    TBNode* preHead=track->last;
    TBNode* sufTail=tail->next;

    //分离出子表达式
    preHead->next=sufTail;sufTail->last=preHead;
    track->last=NULL;tail->next=NULL;

    //对子表达式分析
    track=translateCountDef(translator,track);
    if(track==NULL){
      isRight=0;
      break;
    }
    if(sufTail->token.kind==RIGHT_BRACE) break;
    track=sufTail->next;
  }
  tokens=head.next;
  tokens->last=NULL;
  if(!isRight){
    del_tokenLine(tokens);
    return NULL;
    
  }
  tokens=connect_tokens(tokens,CONST," ");
  return tokens;
}

//去除self表达式
TBNode* removeSelfExp(FuncTranslator* translator,TBNode* tokens){
  if(tokens==NULL) return NULL;
  TBNode* track=tokens;
  TBNode head={
    .last=NULL,
    .next=NULL,
    .token={.kind=UNKNOWN,.val=NULL}
  };
  head.next=tokens;
  tokens->last=&head;
  int isRight=1;
  while(track!=NULL){
    if(track->token.kind!=SELF_KEYWORD){
      track=track->next;
      continue;
    }
    if(track->next==NULL||track->next->next==NULL||track->next->token.kind!=OP||strcmp(track->next->token.val,".")!=0){
      isRight=0;
      break;
    }
    //处理self属性调用
    if(track->next->next->token.kind==VAR){
      TBNode* preHead=track->last;
      TBNode* tail=track->next->next;
      TBNode* sufTail=tail->next;
      preHead->next=NULL;track->last=NULL;
      tail->next=NULL;if(sufTail!=NULL) sufTail->last=NULL;
      preHead->next=sufTail;if(sufTail!=NULL) sufTail->last=NULL;
      track=translateSelfFieldVisit(translator,track);
      if(track==NULL){
        isRight=0;
        break;
      }
      preHead->next=track;track->last=preHead;
      track->next=sufTail;if(sufTail!=NULL) sufTail->last=track;
      track=head.next;  //让track来到第一个元素位置,重新搜索
    }
    //处理self方法调用
    else if(track->next->next->token.kind==FUNC&&track->next->next->next!=NULL
      &&track->next->next->next->token.kind==LEFT_PAR
    ){
      TBNode* preHead=track->last;
      TBNode* tail=track->next->next->next->next;
      TBNode* sufTail=NULL;
      if(!searchArgExpression(tail,&tail)){
        isRight=0;
        break;
      }
      sufTail=tail->next;

      preHead->next=sufTail;if(sufTail!=NULL) sufTail->last=preHead;
      track->last=NULL;tail->next=NULL;

      track=translateSelfFuncVisit(translator,track);
      if(track==NULL){
        isRight=0;
        break;
      }
      preHead->next=track;track->last=preHead;
      track->next=sufTail;if(sufTail!=NULL) sufTail->last=track;
      track=head.next;
    }
    else{
      isRight=0;
      break;
    }
  }
  tokens=head.next;
  tokens->last=NULL;
  if(!isRight){
    del_tokenLine(tokens);
    return NULL;
  }
  return tokens;
}

//去除所有成员访问表达式和数组访问表达式,(事实上,因为成员访问的效果和数组访问的效果是本质一样的)
TBNode* removeMemVisitAndArrVisit(FuncTranslator* translator,TBNode* tokens){
  //TODO
  return tokens;
}

//去除运算符,要根据优先级,先去除优先级最高的运算符
TBNode* removeOP(FuncTranslator* translator,TBNode* tokens){
  //TODO
  return tokens;
}



//函数指针定义语句
TBNode* translateFuncPointerDef(FuncTranslator* functranslator,TBNode* tokens){
  //对于函数指针,获取函数类型,然后加入全局函数表
  //函数指针里面不允许有未知类型










  return tokens;
}

//typedef命名类型别名语句
TBNode* translateTypedef(FuncTranslator* functranslator,TBNode* tokens){
  //首先读取到第一个字符串的地方,如果遇到左括号,说明是函数指针定义语句
  TokenKind kinds[]={LEFT_PAR};
  TBNode* tail=NULL;
  //判断是否不是函数指针重命名类型
  if(!searchExpressUntil(tokens,&tail,kinds,1)||tail->next==NULL||tail->next->token.kind!=LEFT_PAR){
    //如果不是函数指针重名名类型,则是普通类型重名名类型
    if(tail==NULL||tail->token.kind!=LEFT_PAR){
      if(tokens->next==NULL||tokens->next->next==NULL){
        del_tokenLine(tokens);return NULL;
      }
      char* oldName=tokens->next->token.val;
      char* newName=tokens->next->next->token.val;
      //TODO,新类型名合法性检查
      //如果不是合法的新类型
      if(!isLegalNewTypeName(functranslator,newName)){
        fprintf(functranslator->warningFout,"\nfail to use %s as new type name!\n",newName);
        del_tokenLine(tokens);
        return NULL;
      }
      //检查旧类型名是否已经存在
      Type type;
      int layer;
      //如果旧的类型名不存在,或者是未知类型
      if(!findType_valtbl(functranslator->partialValTbl,oldName,&type,&layer)||type.kind==TYPE_UNKNOW){
        fprintf(functranslator->warningFout,"undefined old type %s in:\n",oldName);
        fshow_tokenLine(functranslator->warningFout,tokens);  
        del_tokenLine(tokens);
        return NULL;
      }
      //TODO,加入新类型
      if(!assignTypeNewName(functranslator->partialValTbl,oldName,layer,newName)){
        fprintf(functranslator->warningFout,"fail to assign new type name %s to %s in:\n",newName,oldName);
        fshow_tokenLine(functranslator->warningFout,tokens);  
        del_tokenLine(tokens);
        return NULL;
      }
      //然后合并tokens为CONST
      tokens=connect_tokens(tokens,CONST," ");
      return tokens;
    }
    del_tokenLine(tokens);
    return NULL;
  }
  
  //否则是函数指针定义语句
  char newFuncPointerTypeName[1000];
  char funcPointerTypeName[1000];
  sprint_tbnodes(funcPointerTypeName,sizeof(funcPointerTypeName),tokens->next);
  if(!extractFuncPointerFieldName(funcPointerTypeName,newFuncPointerTypeName,NULL)){
    fprintf(functranslator->warningFout,"fail to extract funcPointer Type in:\n");
    fshow_tokenLine(functranslator->warningFout,tokens);
    del_tokenLine(tokens);
    return NULL;
  }
  if(formatFuncPointerTypeName(funcPointerTypeName)==NULL){
    fprintf(functranslator->warningFout,"unlegal funcPointerType definition in:\n");
    fshow_tokenLine(functranslator->warningFout,tokens);
    del_tokenLine(tokens);
    return NULL;
  }
  //对新类型名进行判断
  if(!isLegalNewTypeName(functranslator,newFuncPointerTypeName)){
    fprintf(functranslator->warningFout,"\nfail to use %s as new type name!\n",newFuncPointerTypeName);
    del_tokenLine(tokens);
    return NULL;
  }
  if (!assignTypeNewName(functranslator->partialValTbl, funcPointerTypeName, 0, newFuncPointerTypeName))
  {
    fprintf(functranslator->warningFout, "fail to assign new type name %s to %s in:\n", newFuncPointerTypeName, funcPointerTypeName);
    fshow_tokenLine(functranslator->warningFout, tokens);
    del_tokenLine(tokens);
    return NULL;
  }
  tail=tokens->next;
  tail->last=NULL;
  tokens->next=NULL;
  tail=connect_tokens(tail,CONST,"");
  tail->last=tokens;
  tokens->next=tail;
  tokens=connect_tokens(tokens,CONST," ");
  return tokens;
}

//判断是否是合法的新类型名,如果是返回非0值,如果不是返回0
int isLegalNewTypeName(FuncTranslator* funcTranslator,const char* newName){
  // 新类型名合法性检查
  // 如果新类型名已经注册为类型
  //复制一个newName进行处理
  char tmpName[1000];
  strcpy(tmpName,newName);
  //判断新名字是否是被注册的别名
  if(hashtbl_get(&funcTranslator->partialValTbl->newOld,&newName,NULL)){
    fprintf(funcTranslator->warningFout,"%s has been used as another type name\n",newName);
    return 0;
  }
  else if (findType_valtbl(funcTranslator->partialValTbl, tmpName, NULL, NULL))
  {
    fprintf(funcTranslator->warningFout,"type %s has defined!\n",tmpName);
    return 0;
  }
  // 如果新类型名已经注册为变量
  else if (findVal(funcTranslator->partialValTbl, tmpName, NULL, NULL, NULL))
  {
    fprintf(funcTranslator->warningFout,"val %s has defined!\n",tmpName);
    return 0;
  }
  // 如果新类型名已经注册为函数指针
  else if (findFuncPointer_valtbl(funcTranslator->partialValTbl, tmpName,NULL,NULL,NULL ))
  {
    fprintf(funcTranslator->warningFout,"func pointer %s has defined!\n",tmpName);
    return 0;
  }
  //判断是否存在函数名中
  else if(findFunc(funcTranslator->funcTbl,tmpName,NULL)){
    fprintf(funcTranslator->warningFout,"func %s has defined!\n",tmpName);
    return 0;
  }
  //判断是否是合法的类型名
  else if(!isLegalId(tmpName)){
    fprintf(funcTranslator->warningFout,"unlegal type name %s!\n",tmpName);
    return 0;
  }
  return 1;
}



//翻译函数调用语句
TBNode* translateFuncUse(FuncTranslator* functranslator,TBNode* tokens){
  char* funcName=tokens->token.val;
  Func* func=findFunc(functranslator->funcTbl,funcName,NULL);
  //如果返回的函数结构体指针为NULL的话,说明是未知的函数,对未知函数不用对每个参数进行类型匹配
  //对每个参数只需要进行表达处理即可
  if(func==NULL){
    //这部分应该是不会到达的位置,不过目前头文件不完善,所以分析
    TBNode* track=tokens->next->next; //首先让足迹来到第一个参数的头部
    int argIndex=0; //记录当前分析的参数下标
    int isRight=1;  //标记分析过程是否出现异常
    //然后每次读取一个参数直到读取到函数的最后一个参数为止
    while(track!=NULL){
      TBNode* argHead=track;  //参数头部
      TBNode* argTail=NULL; //参数尾
      //搜索参数的尾部
      //如果搜索参数尾部失败,说明参数表达式存在异常
      if(!searchArgExpression(argHead,&argTail)){
        //
        isRight=0;
        break;
      }
      //如果搜索成功,首先把参数表达式摘出,处理,然后再接回,然后分析下一个
      TBNode* preHead=argHead->last;
      TBNode* sufTail=argTail->next;

      //摘出参数表达式
      preHead->next=NULL;argHead->last=NULL;
      sufTail->last=NULL;argTail->next=NULL;
      preHead->next=sufTail;sufTail->last=preHead;

      argHead=process_singleLine(functranslator,argHead);
      if(argHead==NULL||argHead->last==NULL||argHead->next!=NULL){
        del_tokenLine(argHead);
        isRight=0;
        break;
      }
      
      //如果参数处理成功,连接回来
      preHead->next=argHead;argHead->last=preHead;
      sufTail->last=argHead;argHead->next=sufTail;
      //判断是否要结束
      if(sufTail->token.kind==RIGHT_PAR){
        break;
      }
      track=sufTail->next;
      if(track==NULL){
        isRight=0;
        break;
      }
    }

    //TODO,异常处理
    if(!isRight){
      del_tokenLine(tokens);
      return NULL;
    }
    //连接整个函数调用,整体化作一个unknown类型的变量
    tokens=connect_tokens(tokens,VAR,"");
    //作为unknown类型变量加入量表
    addVal_valtbl(functranslator->partialValTbl,tokens->token.val,NULL,0,NULL,0);
    return tokens;
  }
  
  TBNode* head=tokens->next;
  TBNode* preHead;
  TBNode* tail;
  TBNode* sufTail;
  TokenKind ends[]={RIGHT_PAR};
  int endsSize=sizeof(ends)/sizeof(ends[0]);
  //首先搜索参数列表
  //如果参数为空
  if(head->next->token.kind==RIGHT_PAR){
    //空参数调用有参函数
    if(functranslator->curFunc->args.size!=0){
      fprintf(functranslator->warningFout,"miss arguement for use of func %s in:\n\t",func->func_name);
      fshow_tokenLine(functranslator->warningFout,tokens);
      del_tokenLine(tokens);
      return NULL;
    }
  }
  //否则如果找不到参数列表
  else if(!searchExpressUntil(head->next,&tail,ends,endsSize)||tail==NULL||(tail->next!=NULL&&tail->next->token.kind!=RIGHT_PAR)){
    fprintf(functranslator->warningFout,"unlegal func use in:\n\t");
    fshow_tokenLine(functranslator->warningFout,tokens);
    del_tokenLine(tokens);
    return NULL;
  }
  tail=tail->next;  //让tail来到右括号位置
  preHead=head->last;
  sufTail=tail->next;

  //分离参数括号表达式
  tail->next=NULL;head->last=NULL;
  if(sufTail!=NULL){
    sufTail->last=preHead;
  }
  preHead->next=NULL;

  //然后进行参数匹配
  int isRightArgs=checkArgs(functranslator,func,head);
  //接回参数列表
  preHead->next=head;head->last=preHead;
  tail->next=sufTail;
  if(sufTail!=NULL) sufTail->last=tail;
  //如果参数匹配失败
  if(!isRightArgs){
    fprintf(functranslator->warningFout,"un correct use of func %s in:\n\t",func->func_name);
    fshow_tokenLine(functranslator->warningFout,tokens);
    del_tokenLine(tokens);
    return NULL;
  }
  //否则参数匹配成功,
  //对整个函数部分进行处理

  //首先分离出 func(...)部分,把这部分合并
  tail->next=NULL;
  if(sufTail!=NULL) sufTail->last=NULL;
  tokens=connect_tokens(tokens,VAR,"");
  //合并func(..)部分后再与后面部分连接
  tokens->next=sufTail;
  if(sufTail!=NULL) sufTail->last=tokens;

  //加入量表
  Type type;  //首先获取返回值类型
  int typeLayer;
  if(!findTypeById(functranslator->gloabalTypeTbl,func->retTypeId,&type,&typeLayer)){
    fprintf(functranslator->warningFout,"undefined retType of func %s in:\n\t",func->func_name);
    fshow_tokenLine(functranslator->warningFout,tokens);
    del_tokenLine(tokens);
    return NULL;
  }
  //返回值类型查找成功,加入量表
  addVal_valtbl(functranslator->partialValTbl,tokens->token.val,NULL,1,type.defaultName,typeLayer);

  return process_singleLine(functranslator,tokens);
}

//翻译赋值语句
TBNode* translateAssign(FuncTranslator* functranslator,TBNode* tokens){
  //TODO,首先先简单地实现一个单赋值语句
  
  //单赋值语句结构为tg = ...; ...为一个值表达式
  int isRight=1;
  if(tokens==NULL||tokens->next==NULL||tokens->next->next==NULL) isRight=0;
  else if(tokens->next->token.kind!=OP||strcmp(tokens->next->token.val,"=")!=0) isRight=0;
  else if(tokens->token.kind!=VAR){
    fprintf(functranslator->warningFout,"target of assign sentence should be var but not %s in:\n\t",tokens->token.val);
    fshow_tokenLine(functranslator->warningFout, tokens);
    isRight=0;
  }
  if(!isRight){
    del_tokenLine(tokens);
    return NULL;
  }
  //然后对后面的值表达式进行处理,首先提取值表达式
  TBNode* valHead=tokens->next->next;
  TBNode* preValHead=valHead->last;
  preValHead->next=NULL;valHead->last=NULL;
  valHead=process_singleLine(functranslator,valHead);
  if(valHead==NULL){
    del_tokenLine(tokens);
    return NULL;
  }
  else{
    preValHead->next=valHead;
    valHead->last=preValHead;
  }
  if(tokens->next->next->token.kind!=VAR&&tokens->next->next->token.kind!=CONST)
    isRight=0;
  if(!isRight){
    del_tokenLine(tokens);
    return NULL;
  }

  //否则先获取目的变量以及目的变量类型,判断是否类型匹配
  Type type;
  Val val;
  int typeLayer;
  char* valName=tokens->token.val;
  if(!findVal(functranslator->partialValTbl,valName,&val,&type,&typeLayer)){
    fprintf(functranslator->warningFout,"undefined var %s in:\n\t",valName);
    isRight=0;
  }
  else if(val.isConst){
    fprintf(functranslator->warningFout,"Error!use const var %s as target var in assign sentence in:\n\t",valName);
    isRight=0;
  }
  if(!isRight){
    fshow_tokenLine(functranslator->warningFout,tokens);
    del_tokenLine(tokens);
    return NULL;
  }
  //TODO,对值与目的量地类型进行判断是否匹配
  

  //最后合并所有tokens
  tokens=connect_tokens(tokens,CONST,"");

  return tokens;
}

//翻译自身属性调用语句
TBNode* translateSelfFieldVisit(FuncTranslator* funcTranslator,TBNode* tokens){
  //对于自身属性调用语句,.处理成->
  free(tokens->next->token.val);
  tokens->next->token.val=strcpy(malloc(strlen("->")),"->");
  //查询变量,然后把这个合并成一个整体
  if(funcTranslator->curFunc==NULL){
    del_tokenLine(tokens);
    return NULL;
  }
  //查找当前方法的主人
  char* ownerName=funcTranslator->curFunc->owner;
  //查找主人类型
  Type type;
  if(!findType_valtbl(funcTranslator->globalValTbl,ownerName,&type,NULL)){
    del_tokenLine(tokens);
    return NULL;
  }
  //如果该主人不含有该属性,进行报错提示
  char* fieldName=tokens->next->next->token.val;
  if(!hashtbl_get(&type.fields,&fieldName,NULL)){
    //如果不含该属性
    fprintf(funcTranslator->warningFout,"visit unexistable field %s in type %s->%s function\n",fieldName,type.defaultName,funcTranslator->curFunc->func_name);
    del_tokenLine(tokens);
    return NULL;
  }
  //否则合并为该类型,查找函数的返回类型
  int typeIndex;
  int typeLayer;
  extractTypeIndexAndPointerLayer(funcTranslator->curFunc->retTypeId,&typeIndex,&typeLayer);
  //取出类型
  vector_get(&funcTranslator->gloabalTypeTbl->types,typeIndex,&type);
  //截断后面部分
  TBNode* tail=tokens->next->next;
  TBNode* sufTail=tail->next;
  tail->next=NULL;
  if(sufTail!=NULL) sufTail->last=NULL;
  //合并tokens
  tokens=connect_tokens(tokens,VAR,"");
  if(tokens==NULL){
    del_tokenLine(sufTail);
    return NULL;
  }
  //加入量表
  addVal_valtbl(funcTranslator->partialValTbl,tokens->token.val,NULL,0,type.defaultName,typeLayer);
  tokens->next=sufTail;
  sufTail->last=tokens;
  return process_singleLine(funcTranslator,tokens);
}

//翻译自身方法调用语句
TBNode* translateSelfFuncVisit(FuncTranslator* funcTranslator,TBNode* tokens){
  //进行参数检查
  if(funcTranslator->curFunc==NULL){
    del_tokenLine(tokens);
    return NULL;
  }
  //获取当前方法的主人类型
  char* ownerName=funcTranslator->curFunc->owner;
  Type type;
  int layer=0;
  findType_valtbl(funcTranslator->globalValTbl,ownerName,&type,&layer);
  //默认之前的约束条件经过严格的判断，因此方法主人的类型要么是union要么是strcut，而且指针层次为0

  //通过self调用函数的情况有两种,一种是函数指针调用,另一种是类型自身方法调用

  //如果是使用函数指针;
  if(containsStr_StrSet(&type.funcPointerFields,tokens->next->next->token.val)){
    // 则合成该调用和该函数为一个未知函数,然后对函数进行分析
    //这是一种将就的选择
    //TODO,设计获取函数指针对应的参数列表和返回值类型的接口,对函数指针的函数调用也可以判断
    TBNode* tail=tokens->next->next;
    TBNode* sufTail=tail->next;
    free(tokens->next->token.val);
    tokens->next->token.val=strcpy(malloc(strlen("->")),"->");
    tail->next=NULL;if(sufTail!=NULL) sufTail->last=NULL;
    //合成
    tokens=connect_tokens(tokens,FUNC,"");
    tokens->next=sufTail;
    if(sufTail!=NULL) sufTail->last=tokens;
    //合成之后翻译普通函数
    tokens=process_singleLine(funcTranslator,tokens);
    //把翻译结果转化为合适的变量
    //TODO
    return tokens;
  }
  //否则如果是调用自身方法
  else if(containsStr_StrSet(&type.funcs,tokens->next->next->token.val)){
    //查询方法属性
    char* ownerName=type.defaultName;
    char* funcName=tokens->next->next->token.val;
    Func* func=findFunc(funcTranslator->funcTbl,funcName,ownerName);
    //如果函数没有找到
    if(func==NULL){
      fprintf(funcTranslator->warningFout,"member method %s of type %s undefined but used in:\n\t",funcName,ownerName);
      fshow_tokenLine(funcTranslator->warningFout,tokens);
      del_tokenLine(tokens);
      return NULL;
    }
    //进行参数匹配,分析
    TBNode* track=tokens->next->next->next->next; //让track来到第一个参数开头位置
    //记录匹配到的参数的数量
    int i=0;
    int isRight=1;  //标记是否正常运行
    //TODO,开始匹配
    while(track->token.kind!=RIGHT_PAR&&i<funcTranslator->curFunc->args.size){
      //读取到尽头,进行process处理
      TBNode* head=track;
      TBNode* preHead=head->last;
      TBNode* tail;
      TokenKind kinds[]={COMMA,RIGHT_PAR};
      //查找逗号
      if(!searchExpressUntil(head,&tail,kinds,2)||tail==NULL){
        isRight=0;
        break;
      }
      int isFinished=0;
      if(tail->next==NULL){
        isRight=0;
        break;
      }
      TBNode* sufTail=tail->next;
      if(sufTail->token.kind!=COMMA&&sufTail->token.kind!=RIGHT_PAR){
        isRight=0;
        break;
      }
      if(tail->next->token.kind==RIGHT_PAR) isFinished=1;
      //进行process处理

      //首先分离
      tail->next=NULL;
      sufTail->last=preHead;
      preHead->next=sufTail;
      head->last=NULL;

      head=process_singleLine(funcTranslator,head);
      if(head==NULL){
        isRight=0;
        break;
      }
      //TODO,否则head处理成功,接回来
      preHead->next=head;
      head->last=preHead;
      head->next=sufTail;
      sufTail->last=head;
      
      //然后进行参数类型匹配
      //查找实参类型
      Type type;
      Val val;
      int typeLayer;
      //如果实参变量不存在
      if(!findVal(funcTranslator->partialValTbl,head->token.val,&val,&type,&typeLayer)){
        fprintf(funcTranslator->warningFout,"var %s used but not defined in\n\t:",head->token.val);
        fshow_tokenLine(funcTranslator->warningFout,tokens);
        isRight=0;
        break;
      }
      //然后取出形参
      Arg arg;
      vector_get(&funcTranslator->curFunc->args,i,&arg);
      //获取参数类型
      Type argType;
      int argTypeLayer;
      int typeIndex;
      extractTypeIndexAndPointerLayer(arg.typeId,&typeIndex,&argTypeLayer);
      vector_get(&funcTranslator->gloabalTypeTbl->types,typeIndex,&argType);


      //进行匹配,TODO
      if(!(argType.defaultName==type.defaultName)&&argTypeLayer==typeLayer){
        //提示是不同的类型名
        char* argTypeName=getTypeName(argType.defaultName,argTypeLayer);
        char* typeName=getTypeName(type.defaultName,typeLayer);
        fprintf(funcTranslator->warningFout,"%d arg need %s type val but not %s,which is of %s type in:\n\t",i+1,argTypeName,head->token.val,typeName);
        fshow_tokenLine(funcTranslator->warningFout,tokens);
        free(argTypeName);
        free(typeName);
        isRight=0;
        break;
      }
      //判断常变是否匹配
      else if(arg.isConst&&!val.isConst){
        //TODO
      } 
      else if(!arg.isConst&&val.isConst){

      }
      

      //进行退出判断和处理,
      //如果结束让track来到函数结尾的右括号位置
      if(isFinished){
        //TODO,
        track=sufTail;
        break;
      }
      //如果还没处理完参数,进入到下一个位置
      else{
        track=sufTail->next;
      }
    }
    //TODO,检查形参数量与实参数量是否匹配


    if(!isRight){
      fprintf(funcTranslator->warningFout,"%d arg wrong!in use of func %s of type %s in:\n\t",i+1,funcName,ownerName);
      fshow_tokenLine(funcTranslator->warningFout,tokens);
      fprintf(funcTranslator->warningFout,"in definition of func %s\n",funcTranslator->curFunc->func_name);
      del_tokenLine(tokens);
      return NULL;
    } 

    //进行方法改名

    //修改

    //TODO

  }
  //否则是异常情况,调用不属于自身的方法，进行语法报错
  else{
    fprintf(funcTranslator->warningFout,"visit undeclare function %s of type %s!\n",tokens->next->next->token.val,funcTranslator->curFunc->owner);
    del_tokenLine(tokens);
    return NULL;
  }
  return tokens;
}

//翻译类型方法调用语句
TBNode* translateTypeMethodUse(FuncTranslator* functranslator,TBNode* tokens){
  //首先根据变量名查找量和类型
  Val val;
  Type type;
  int layer;
  if(tokens->token.kind!=VAR){
    del_tokenLine(tokens);
    return NULL;
  }
  if(!findVal(functranslator->partialValTbl,tokens->token.val,&val,&type,&layer)){
    del_tokenLine(tokens);
    return NULL;
  }
  //如果类型是含有这个函数指针
  if(containsStr_StrSet(&type.funcPointerFields,tokens->next->next->token.val)){
    //翻译函数指针内部参数内容
    //TODO

  }
  //如果类型不含有这个类型方法
  if(!containsStr_StrSet(&type.funcs,tokens->next->next->token.val)){
    fprintf(functranslator->warningFout,"%s type val %s doesn't have member function %s\n",type.defaultName,tokens->token.val,tokens->next->next->token.val);
    del_tokenLine(tokens);
    return NULL;
  }
  //否则该类型含有该成员方法,进行成员方法改造
  //TODO
  //首先找到该方法
  Func* func=findFunc(functranslator->funcTbl,tokens->next->next->token.val,type.defaultName);
  //然后,对后面传入的参数进行分析
  
  //然后进行成员改造和重命名

  //然后连接函数和后面表达式



  return tokens;
}


int preValNameAddJudge(FuncTranslator* funcTranslator,char* s){
  if(strToId(funcTranslator->partialValTbl->valIds,s)>=0){
    fprintf(funcTranslator->warningFout,"redefinition of val %s\n",s);
    return 0;
  }
  //判断是否是类型名
  Type type;
  if(findType_valtbl(funcTranslator->partialValTbl,s,&type,NULL)){
    fprintf(funcTranslator->warningFout,"the same typeName %s has exist !\n",s);
    return 0;
  }
  //再判断是否是合法的名字
  if(!isLegalId(s)){
    //事实上这一步并不必要,但是解释执行的时候可以用到,
    //因为编译执行的时候,前面已经做了语法检查，如果发生这样的语法错误前面的过程无法通过
    //事实上translate这一阶段应该负责语义检查
    fprintf(funcTranslator->warningFout,"unlegal syntax in val name %s\n",s);
    return 0;
  }
  return 1;
}




int searchBracketExpression(TBNode* nodes,TBNode** head,TBNode** tail){
  //首先查找开头左方括号
  while(nodes!=NULL){
    if(nodes->token.kind==LEFT_BRACKET){
      *head=nodes;
      break;
    }
    nodes=nodes->next;
  }
  if(nodes==NULL ) return 0;
  int leftP=1;
  do{
    nodes=nodes->next;
    TokenKind kind=nodes->token.kind;
    if(kind==LEFT_PAR||kind==LEFT_BRACE||kind==LEFT_BRACKET) leftP++;
    else if(kind==RIGHT_PAR||kind==RIGHT_BRACE||kind==RIGHT_BRACKET) leftP--;
    if(leftP==0&&kind==RIGHT_BRACKET) break;
  }while(nodes!=NULL);
  if(nodes==NULL) return 0;
  *tail=nodes;
  return 1;
}

//在参数的开头往后面搜索参数的结尾

int searchArgExpression(TBNode* head,TBNode** tail){
  int leftP=0;
  while(head!=NULL){
    TokenKind kind=head->token.kind;
    if(kind==LEFT_PAR||kind==LEFT_BRACE||kind==LEFT_BRACKET) leftP++;
    else if(kind==RIGHT_PAR||kind==RIGHT_BRACE||kind==RIGHT_BRACKET) leftP--;
    if(leftP==0&&(head->next->token.kind==COMMA||head->next->token.kind==RIGHT_PAR)) break;
    head=head->next;
  }
  if(head==NULL) return 0;
  *tail=head;
  return 1;
}


int searchExpressUntil(TBNode* head,TBNode** retTail,TokenKind* kinds,int kindSize){
  int leftP=0;
  if(head==NULL) return 0;
  
  while(head!=NULL){
    TokenKind kind=head->token.kind;
    if(kind==LEFT_PAR||kind==LEFT_BRACE||kind==LEFT_BRACKET) leftP++;
    else if(kind==RIGHT_PAR||kind==RIGHT_BRACE||kind==RIGHT_BRACKET) leftP--;
    int ifBreak=0;
    if(head->next==NULL) break;
    kind=head->next->token.kind;
    for(int i=0;i<kindSize&&leftP==0;i++){
      if(kinds[i]==kind){
        ifBreak=1;
        break;
      }
    }
    if(ifBreak) break;
    head=head->next;
  }
  if(leftP!=0) return 0;
  *retTail=head;
  return 1;
}
