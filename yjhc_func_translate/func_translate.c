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
    if(nodes!=NULL) fput_tokenLine(fout,nodes);

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
  if(kind=NOT_LEAGAL_FTK){
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
  //如果一个句子里面全部都是界符,则这个句子是不用翻译的
  //首先进行一次遍历,取出所有符号的类型
  //然后使用位运算来判断该符号是否正确
  //比如说0000_0000八个位置表示八种符号
  //而我们的句子的符号种类分布是 1100_0000，1表示有
  //要判断是否全部符号都在0001_1111内
  //则取反后者1110_0000,然后拿后者与前者做与运算
  //如果结果不为0,则说明目标句子含有其他符号,否则没有
  if(nodes==NULL) return NOT_TRANSLATE_FTK;
  if(funcTranslator->curFunc==NULL) return NOT_TRANSLATE_FTK;
  BitMapUtil bmu={
    .mapSize=sizeof(long long)
  };
  BitMap bm=getBitMap(&bmu);
  TBNode* track=nodes;
  while(track!=NULL){
    put_bitmap(&bmu,&bm,track->token.kind);
    track=track->next;
  }
  long long curMap;
  memcpy(&curMap,bm.map,sizeof(curMap));
  long long sepOrCon=SEP_BITMAP|CONTROL_KEYWORD_BITMAP;
  //判断是否仅仅含有界符和关键字的表达式,则不用进行任何翻译动作处理，
  if(((~sepOrCon)&curMap)==0){
    return NOT_TRANSLATE_FTK;
  }
  
  //判断是否是typedef语句
  if(nodes->token.kind==TYPEDEF_KEYWORD){
    return TYPEDEF_FTK;
  }
  //判断是否是变量定义语句
  if(nodes->token.kind==TYPE&&nodes->next!=NULL&&nodes->next->token.kind==VAR){
    return VAR_DEFINE_FTK;
  }
  //判断是否是常量定义语句
  if(nodes->token.kind==CONST_KEYWORD){
    if(nodes->next!=NULL&&nodes->next->token.kind==TYPE) {
      return CONST_DEFINE_FTK;
    }
  }
  
  //判断是否是函数调用语句
  if(nodes->token.kind==FUNC){
    return FUNC_USE_FTK;
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

  //判断是否是不需要进行成员方法调用翻译的语句
  if(isNotNeedTranslate(funcTranslator,nodes)){
    return NOT_TRANSLATE_FTK;
  }
  if(funcTranslator->judSentenceKindErr){
    //处理异常,并返回对判断:句子不合语法
    funcTranslator->judSentenceKindErr=0;
    return NOT_LEAGAL_FTK;
  }
  
  //否则需要进行成员方法调用翻译
  return MEMBER_FUNCTION_USE_FTK;
}

//判断是否是不需要翻译的语句,也就是没有成员函数调用的语句
int isNotNeedTranslate(FuncTranslator* funcTranslator,TBNode* nodes){
  //如果没有成员方法调用,则不需要翻译
  //判断是否有成员方法调用的分析
  TBNode* track=nodes;
  //如果存在self就不是不需要翻译的情况
  while(track!=NULL){
    if(track->token.kind==SELF_KEYWORD){
      return 0; //需要翻译
    }
    track=track->next;
  }
  //如果没有在上面退出,说明里面不含有self关键字
  track==nodes;
  while (track!=NULL)
  {
    if(track->token.kind!=FUNC||track->last==NULL||strcmp(track->last->token.val,".")!=0){
      track=track->next;
      continue;
    }
    //往前搜索直到上一个表达式结束为止

    //或者搜索到NULL为止,搜索到NULL说明情况异常
    TBNode* ownerHead=searchOwnerHead(track->last->last);
    TBNode* ownerTail=track->last->last;
    if(ownerHead!=NULL){
      //获取表达式类型
      Type type;
      int retLayer;
      //如果获取表达式类型成功,根据类型判断是否式成员方法调用
      if(getTypeOfExpression(funcTranslator,nodes,&type,&retLayer)){
        
      }
      //如果获取失败,报错
      else{

      }

    }

    track=track->next;
  }
  return 1; //如果在循环中没有发现不用输出,
}

//往前搜索到一个连锁表达式的起点
TBNode* searchOwnerHead(TBNode* tail){
  TBNode* track=tail;
  //对表达式进行搜索
  TBNode* head=tail;
  while(track!=NULL){
    if(track->token.kind==VAR){
      head=track;
    }
    else if(track->token.kind==RIGHT_PAR){
      //往前搜索直到括号表达式结束
      int count=1;
      TBNode* track2=track->last;
      TBNode* sufTrack2=NULL;
      while(track2!=NULL&&count!=0){
        if(track2->token.kind==RIGHT_PAR) count++;
        else if(track2->token.kind==LEFT_PAR) count--;
        sufTrack2=track2;
        track2=track2->last;
      }
      //异常情况,括号不对齐,返回NULL
      if(count!=0){
        return NULL;
      }
      //否则是对齐的
      if(track2==NULL||track2->token.kind!=FUNC){
        head=sufTrack2;
      }
      else{
        head=track2;
      }
    }
    else if(track->token.kind==RIGHT_BRACKET){
      //往前搜索直到方块表达式结束
      int count=1;
      TBNode* track2=track2->last;
      TBNode* sufTrack2=NULL;
      while(track2!=NULL){
        sufTrack2=track2;
        if(track2->token.kind==RIGHT_BRACKET&&) count++;
        else if(track2->token.kind==LEFT_BRACKET) count--;
        if(count!=0||track2->last==NULL||track2->last->token==RIGHT_BRACKET) track2=track2->last;
        sufTrack2=track2->next;
        break;
      }
      //表达式前面的内容应该是field,如果表达
      if(count!=0||track2==NULL){
        return NULL;
      }
      if(track2->last->token.kind==VAR){
        head=track2->last;
      }
      else{
        return NULL;
      }
    }
    else return NULL;
    if(head->last->token.kind==OP&&strcmp(head->last->token.val,".")==0){
      track=head->last->last
    }
    else break;
  }
  return head;
}


//获取表达式的类型
int getTypeOfExpression(FuncTranslator* translator,TBNode* nodes,Type* retType,int* retTypeLayer){

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


//判断是否是赋值语句
int isAssignSentence(FuncTranslator* funcTranslator,TBNode* nodes){
  //准备内容
  // 格式为:var=val,var2=val,...var3=val
  //准备个自动机来判断是否是赋值语句
  int state=0;
  while(nodes!=NULL){
    //初始状态仅仅可能遇到VAR
    if(state==0&&nodes->token.kind==VAR){
      state=1;
    } 
    //读到var之后进入状态1,可能遇到,或者=
    //遇到','进入状态0,期待遇到下一个变量
    else if(state==1&&nodes->token.kind==COMMA){
      state=0;
    }
    //遇到=号说明是赋值语句
    else if(state==1&&strcmp(nodes->token.val,"=")==0){
      return 1;
    }else{
      funcTranslator->judSentenceKindErr=1;
      return 0;
    }
    nodes=nodes->next;
  }
  return 0;
}



//翻译功能子代码,翻译成功返回非NULL,翻译失败返回NULL

//翻译变量定义语句
TBNode* translateVarDef(FuncTranslator* functranslator,TBNode* tokens){

}

//翻译常量定义语句
TBNode* translateConstDef(FuncTranslator* functranslator,TBNode* tokens){

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

}

//翻译赋值语句
TBNode* translateAssign(FuncTranslator* functranslator,TBNode* tokens){
  //首先获取类型的typeId

  //然后把后面的每个变量都加入该类型,以括号为间隔
  char* typeName=tokens->token.val; //首先取出类型名

  TBNode* track=tokens->next;
  TBNode* tHead=NULL;
  int state=0;
  int leftPar=1; //统计左括号数量,只有左括号数量为0的时候遇到右括号才进行处理
  //TODO
  while(track!=NULL){
    if(state==0){
      //取出变量名
      if(track->token.kind!=VAR) return NULL;
      addVal_valtbl(functranslator->partialValTbl,track->token.val,NULL,0,typeName);
      tHead=track;
      state=1;
    }
    else if(state!=1) return NULL;
    //如果到一个句子的结尾
    else if(leftPar==0&&track->token.kind==COMMA){
      //截取出这个句子进行处理
      TBNode* tail=track->last;
      TBNode* preHead=tHead->last;
      tHead->last=NULL;
      tail->next=NULL;
      tHead=process_singleLine(functranslator,tHead); //返回新双向链表头部
      //TODO,
      tail=findTBNodesTail(tHead); //找到双向链表尾部


    }
    else if(track->token.kind==RIGHT_PAR){
      leftPar--;
    }
    else if(track->token.kind==LEFT_PAR){
      leftPar++;
    }

  }
  //对最后一句进行处理
  if(tHead->next!=NULL){

  }

}






//翻译类型方法调用语句
TBNode* translateTypeMethodUse(FuncTranslator* functranslator,TBNode* tokens){

}

