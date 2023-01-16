#include "func_translate.h"



//创建函数翻译器,翻译结果是把未翻译的yjhc的函数代码token转为c的函数代码token序列
FuncTranslator getFuncTranslator(char* typePath,char* funcHeadPath,char* valPath){
  FuncTranslator out={
    .funcTbl=NULL,
    .gloabalTypeTbl=NULL,
    .partialValTbl=NULL,
    .globalValTbl=NULL,
    .curFunc=NULL
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

  out.globalValTbl=malloc(sizeof(FuncTbl));
  *(out.globalValTbl)=getValTbl(*(out.gloabalTypeTbl));
  loadFile_valtbl(out.globalValTbl,valFin);

  out.partialValTbl=out.globalValTbl;

  //完成空间分配后释放空间
  fclose(typeFin);
  fclose(valFin);
  fclose(funcFin);
  return out;
}





//使用函数翻译器开始翻译
int func_translate(FuncTranslator* funcTranslator,char* tokenInPath,char* tokenOutPath){
  if(tokenInPath==NULL || tokenOutPath==NULL ) return 0;
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
    nodes=process_singleLine(funcTranslator,nodes);
    //然后把处理结果写入文件
    if(nodes!=NULL) fput_tokenLine(fout,nodes);
    //进行块的进出更新
    if(preBlocks==actionSet.blocks-1){
      //判断是否要加载函数
      if(preBlocks==0){ //加载函数
        //查找对应函数
        char* name;
        vector_get(&funcTranslator->funcTbl->funcNames,funcIndex,&name);
        //根据名字查找函数
        hashtbl_get(&funcTranslator->funcTbl->funcs,&name,&funcTranslator->curFunc);
        //TODO加入安全检查
        if(funcTranslator->curFunc==NULL){
          isRight=0;
          break;
        }
      }
      //增加新的局部量表
      funcTranslator->partialValTbl->next=malloc(sizeof(ValTbl));
      *(funcTranslator->partialValTbl->next)=getValTbl(getTypeTbl());
      funcTranslator->partialValTbl->next->next=NULL;
      funcTranslator->partialValTbl->next->pre=funcTranslator->partialValTbl->next;
      funcTranslator->partialValTbl=funcTranslator->partialValTbl->next;
      //TODO,加入函数的参数表到局部变量中
      
    }
    else if(preBlocks==actionSet.blocks+1){
      //TODO,判断函数是否退出
      if(preBlocks==1){
        funcTranslator->curFunc=NULL;
      }
      //释放局部量块
      ValTbl* tmpValTbl=funcTranslator->partialValTbl;
      //进行安全性检查,TODO
      if(tmpValTbl==funcTranslator->globalValTbl){
        isRight=0;
        break;
      }
      funcTranslator->partialValTbl=tmpValTbl->pre;
      del_valTbl(tmpValTbl);
      free(tmpValTbl);
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
  return 1;
}



//判断句子的翻译类型,以选择不同的翻译语句
FTK getTokenLineKind(FuncTranslator* funcTransltor,TBNode* nodes){
  //如果一个句子里面全部都是界符,则这个句子是不用翻译的
  //首先进行一次遍历,取出所有符号的类型
  //然后使用位运算来判断该符号是否正确
  //比如说0000_0000八个位置表示八种符号
  //而我们的句子的符号种类分布是 1100_0000，1表示有
  //要判断是否全部符号都在0001_1111内
  //则取反后者1110_0000,然后拿后者与前者做与运算
  //如果结果不为0,则说明目标句子含有其他符号,否则没有
  if(nodes==NULL) return NOT_TRANSLATE_FTK;
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
  //如果消除sepOrCon以外的标记,
  //如果结果为0,说明没有其他标记,
  //说明没有sepOrCon以外的字符
  if(((~sepOrCon)&curMap)==0){
    return NOT_TRANSLATE_FTK;
  }
  //否则进行量查询
  //如果存在typedef,则说明是类型起别名语句
  if(in_bitmap(&bmu,&bm,TYPEDEF_KEYWORD)) return TYPEDEF_FTK;
  //如果开头是类型,或者const关键字什么的,则是量定义语句
  if(nodes->token.kind==TYPE){
    return VAR_DEFINE_FTK;
  }
  else if(nodes->token.kind==CONST){
    return CONST_DEFINE_FTK;
  }
  //判断是否是需要翻译的函数调用
  //也就是判断是否是a.funcA()这种类型,或者如果是在成员方法内的话,
  //判断是否是省略了self的funcA()
  //又或者是否是self.funcA()


  //判断是否是赋值语句
  //也就是判断是否是a=1或者a,b,c=1,2,3这种类型

  //判断是否

  
  

  


  return NOT_TRANSLATE_FTK;
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

}

//翻译类型方法调用语句
TBNode* translateTypeMethodUse(FuncTranslator* functranslator,TBNode* tokens){

}

