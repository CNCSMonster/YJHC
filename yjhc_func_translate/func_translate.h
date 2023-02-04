#ifndef _FUNC_TRANSLATE_H
#define _FUNC_TRANSLATE_H

#include "lib_use.h"
#include "syntax_symbol.h"
#include "func.h"
#include "type.h"
#include "val_tbl.h"
#include "token_reader.h"
#include "config.h"



//目前只是处理单文件的问题,多文件的yjhc代码会先通过静态链接的处理再进行翻译

//一些翻译用的参数,
//该参数用于重命名结构体方法
#define PREFIX_OF_NEW_FUNC "__yjhc_"
//该参数用于重命名结构体方法中调用的自身属性
#define PREFIX_OF_NEW_VAL "this."


//函数翻译者,保存函数翻译者需要的一些参数
typedef struct func_translator{
  //表结构
  TypeTbl* gloabalTypeTbl; //全局类型表
  ValTbl* globalValTbl;  //全局量表
  ValTbl* partialValTbl;  //局部量表
  FuncTbl* funcTbl; //全局函数表
  //设置异常信息,如果异常信息出现问题,用来传递提示
  int judSentenceKindErr;
  //注意,token读取器使用的单例模式
  Func* curFunc;  //当前翻译到的函数
  FILE* warningFout;  //报错信息输出路径,一般默认是stdout
}FuncTranslator;

//创建函数翻译器,翻译结果是把未翻译的yjhc的函数代码token转为c的函数代码token序列
FuncTranslator getFuncTranslator(char* typePath,char* funcHeadPath,char* valPath);

//进行翻译前检查
int pre_translate_check(FuncTranslator* translator);

//进行是否有主函数检查,如果有主函数,返回非0值,如果没有，返回0
int check_main_function(FuncTranslator* translator);


//使用函数翻译器开始翻译
int func_translate(FuncTranslator* funcTranslator,char* tokenInPath,char* tokenOutPath);


//补全成员方法中自身方法的调用
TBNode* member_use_complement(FuncTranslator* funcTranslator,TBNode* nodes);


//翻译单个句子,成功返回nodes,失败返回NULL
TBNode* process_singleLine(FuncTranslator* funcTranslator,TBNode* nodes);

//翻译结束释放函数翻译器
int release_funcTranslator(FuncTranslator* funcTranslator);


//判断翻译类型
typedef enum func_translate_kind{
  NOT_LEAGAL_FTK,   //判断是有语法错误的句子就会返回NOT_LEAGAL_FTK
  NOT_TRANSLATE_FTK,    //不用翻译的语句
  VAR_DEFINE_FTK,      //变量定义语句
  CONST_DEFINE_FTK,     //常量定义语句
  FUNC_USE_FTK,     //函数调用语句
  COUNT_FTK,    //运算语句,纯粹是运算语句
  TYPEDEF_FTK,    //typedef起头的类型别名定义语句
  FUNCPOINTER_DEF_FTK,  //函数指针量定义语句
  ASSIGN_FTK, //赋值语句
  MEMBER_FUNCTION_USE_FTK,  //类型方法调用语句
  TYPE_CHANGE_FTK,  //类型强转语句
  MEMBER_FIELD_USE_FTK, //类型属性调用语句
  ARR_VISIT_FTK,  //数组元素访问调用语句
  SELF_FIELD_VISIT_FTK,                //自身成员属性访问语句
  SELF_FUNC_VISIT_FTK,  //自身方法访问语句
  SET_EXP_FTK,  //处理像是{a1,a2}这样的枚举表达式
  FTK_NUM   //token的数量
}FTK;

//判断句子的翻译类型,以选择不同的翻译语句
FTK getTokenLineKind(FuncTranslator* funcTranslator,TBNode* tokens);


//判断是否是不需要方法调用翻译的句子
int ifNotNeedFuncTranslate(FuncTranslator* translator,TBNode* nodes);



//句子类型判断子函数
//判断是否是函数指针定义语句
int isFuncPointerDefSentence(FuncTranslator* funcTranslator,TBNode* nodes);


//判断是否是赋值语句
int isAssignSentence(FuncTranslator* funcTranslator,TBNode* nodes);

//判断是否是数组成员访问
int isArrVisitSentence(FuncTranslator* funcTranslator,TBNode* nodes);

//判断是否是类型强转语句
int isTypeChangeSentence(FuncTranslator* funcTranslator,TBNode* nodes);


//翻译功能子代码,翻译成功返回非NULL,翻译失败返回NULL

//翻译成员属性访问语句
TBNode* translateMemberFieldVisit(FuncTranslator* functranslator,TBNode* tokens);

//数组元素访问语句
TBNode* translateArrVisit(FuncTranslator* funcTranslator,TBNode* tokens);

//翻译类型强转语句
TBNode* translateTypeChange(FuncTranslator* funcTranslator,TBNode* tokens);

//翻译变量定义语句
TBNode* translateVarDef(FuncTranslator* functranslator,TBNode* tokens);

//翻译常量定义语句
TBNode* translateConstDef(FuncTranslator* functranslator,TBNode* tokens);

//翻译枚举表达式
TBNode* translateSetExp(FuncTranslator* funcTranslator,TBNode* tokens);

//翻译运算语句
TBNode* translateCountDef(FuncTranslator* functranslator,TBNode* tokens);

//翻译运算语句的子函数


//翻译运算语句的子函数

//去除括号表达式
TBNode* removeParExp(FuncTranslator* translator,TBNode* tokens);

//去除序列表达式
TBNode* removeSetExp(FuncTranslator* translator,TBNode* tokens);

//去除self表达式
TBNode* removeSelfExp(FuncTranslator* translator,TBNode* tokens);

//去除所有成员访问表达式和数组访问表达式,(事实上,因为成员访问的效果和数组访问的效果是本质一样的)
TBNode* removeMemVisitAndArrVisit(FuncTranslator* translator,TBNode* tokens);

//去除运算符,要根据优先级,先去除优先级最高的运算符
TBNode* removeOP(FuncTranslator* translator,TBNode* tokens);


//函数指针定义语句
TBNode* translateFuncPointerDef(FuncTranslator* functranslator,TBNode* tokens);

//typedef命名类型别名语句
TBNode* translateTypedef(FuncTranslator* functranslator,TBNode* tokens);

//翻译函数调用语句
TBNode* translateFuncUse(FuncTranslator* functranslator,TBNode* tokens);

//翻译赋值语句
TBNode* translateAssign(FuncTranslator* functranslator,TBNode* tokens);

//翻译类型方法调用语句
TBNode* translateTypeMethodUse(FuncTranslator* functranslator,TBNode* tokens);

//翻译自身属性调用语句
TBNode* translateSelfFieldVisit(FuncTranslator* funcTranslator,TBNode* tokens);

//翻译自身方法调用语句
TBNode* translateSelfFuncVisit(FuncTranslator* funcTranslator,TBNode* tokens);


//单句翻译辅助,判断字符串是否是能够增加的量名或者类型名
int preValNameAddJudge(FuncTranslator* funcTranslator,char* s);


//常量开头语句

//翻译函数的选择表

TBNode* (*tranFuncs[]) (FuncTranslator*,TBNode*) = {
  [NOT_LEAGAL_FTK] NULL,
  [NOT_TRANSLATE_FTK] NULL,
  [VAR_DEFINE_FTK] translateVarDef,
  [CONST_DEFINE_FTK] translateConstDef,     //常量定义语句
  [FUNC_USE_FTK] translateFuncUse,     //函数调用语句
  [COUNT_FTK] translateCountDef,    //运算语句,纯粹是运算语句
  [TYPEDEF_FTK] translateTypedef,    //typedef起头的类型别名定义语句
  [FUNCPOINTER_DEF_FTK] translateFuncPointerDef,  //函数指针量定义语句
  [ASSIGN_FTK] translateAssign, //赋值语句
  [ARR_VISIT_FTK] translateArrVisit,  //数组元素访问语句
  [TYPE_CHANGE_FTK] translateTypeChange, //类型强转语句
  [SELF_FIELD_VISIT_FTK] translateSelfFieldVisit,
  [SELF_FUNC_VISIT_FTK] translateSelfFuncVisit,
  [SET_EXP_FTK] translateSetExp,  //翻译枚举表达式,或者说翻译集合表达式
  [MEMBER_FIELD_USE_FTK] translateMemberFieldVisit, //成员属性访问语句
  [MEMBER_FUNCTION_USE_FTK] translateTypeMethodUse   //类型方法调用语句
};

//从TBNode*某个位置起,读取一个方括号表达式,比如[dsds] 读取的结果从head到tail则为[dsds]
int searchBracketExpression(TBNode* nodes,TBNode** head,TBNode** tail);

//在参数的开头搜索参数表达式的结尾,通过函数指针返回结尾,搜索成功返回非0值，搜索异常返回0
int searchArgExpression(TBNode* head,TBNode** tail);

//从head搜索表达式知道遇到某个类型的token,或者搜索到表达式尽头为止(注意,中间括号包起来的部分被视为整体)
//如果正常搜索返回非0值,如果搜索结果不存在返回0,如果搜索异常也返回0
int searchExpressUntil(TBNode* head,TBNode** retTail,TokenKind* kinds,int kindSize);

#endif