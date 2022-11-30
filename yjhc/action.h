#ifndef _ACTION_H
#define _ACTION_H

//判断是否需要递归
enum RecurseAction{

};

//判断是否需要抛出异常,也就是是否是未定义部分
enum ErrAction{

};

//进出块的四种可能,
enum BlockAction{
  MOVEINTO, //进入新的块
  MOVEOUT,  //离开块
  STAY  //保持在块中
};

//是否换行的两种可能
enum PrintAction{
  NOT_DEFINE_PRINTACTION,   //没有定义的打印动作,用来表示语法分析的异常情况
  NEWLINE,  //换行
  KEEPLINE  //不换行
};

//函数翻译的两种情况
enum FuncTranslateAction
{
  FUNC_TS_NEED,   //需要翻译函数
  FUNC_TS_NOTNEED //不需要翻译函数
};


#endif