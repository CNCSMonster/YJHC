#ifndef _SYNTAX_SYMBOL_H
#define _SYNTAX_SYMBOL_H

//语句类型,如果是读到第一条语句前,则上一条语句类型是init
enum syntax_symbol
{
  /*
  一些定义.
  无条件块进入:if块,for块,while块,do块是基本任何时候都可以进入的,不需要置块条件
  有条件块:比如else if块和else块，只有前面紧紧跟着if或者else if结构的时候才能够进入
  */
  INIT,     //还没有读取进入函数的状态,唯一接受的token就是left brace,遇到left_brace压入,然后换行,如果遇到其他则报错
  BLOCK_IN, //遇到块或者condition进入标志则压入栈中,遇到分号换行,其他同行同号输入,遇到右花括号则弹出
  // if-elif-if结构
  IF_CONDITION_IN,   //任何时候遇到iftoken压入该符号,不换行,遇到左圆括号压入栈中,不接受遇到分号,遇到花括号则弹出,压入if_block_in
  LEFT_PARENTHESIS,  //左边小括号在栈顶,说明在某个condition中,如果遇到左圆括号压入栈顶,遇到右圆括号把左圆括号弹出不接受遇到分号
  IF_BLOCK_IN,       //遇到分号换行,可以对块或者condition进入标志反应,遇到右花括号弹出,压入if_block_out,换行
  IF_BLOCK_OUT,      //遇到else弹出,压入else_condition_in,遇到else if弹出,,压入else if condition in,遇到其他符号则弹出if_block_out再与之前栈顶元素做判断
  ELIF_CONDITION_IN, //遇到左圆括号压入,遇到左花括号弹出并换行,压入elif_block_in,进入块,不接受分号,其他当行打印
  ELIF_BLOCK_IN,     //可以无条件块进入,遇到分号换行,遇到右括号弹出,进入elif_block_out
  ELIF_BLOCK_OUT,    //遇到else/else if token，进入块,遇到其他结构,弹出elif_block_out,继续与新栈顶做判断
  ELSE_BLOCK_IN,     //接受无条件块进入,遇到分号换行,遇到右花括号弹出,
  ELSE_BLOCK_OUT,    //遇到else if/else结构进入标记可进入换行，遇到其他则弹出并使用新栈顶符号继续判断
  // do-while结构,
  DO_PRE,          //遇到花括号弹出,压入do_block_in,不能接受其他任何token
  DO_BLOCK_IN,     //遇到分号换行,能够接受无条件块,遇到花括号结束,弹出DO-block-in，不换行，出块进入DO_BLOCK_OUT
  DO_BLOCK_OUT,    //遇到while不换行,进入DO_CONDITION_IN
  DO_CONDITION_IN, //遇到左括号压入,遇到分号换行，弹出
  // while结构
  WHILE_CONDITION_IN, //遇到while关键字压入,遇到左括号压入,左边花括号弹出,压入while_block_in
  WHILE_BLOCK_IN,     //接受无条件块,遇到分号换行,遇到花括号结束,弹出WHILE_BLOCK_IN,出块,换行,无压入
  // for结构
  FOR_PRE,          //遇到for后压入FOR_PRE,for_pre栈顶遇到左括号,弹出,压入for_init_in
  FOR_INIT_IN,      //遇到分号不换行,弹出,压入for_condition_in,
  FOR_CONDITION_IN, //遇到左括号压入,遇到右括号弹出,遇到分号不换行,弹出,压入FOR_REFRESH_IN
  FOR_REFRESH_IN,   //遇到左括号压入,遇到右括号弹出,压入FOR_REFRESH_OUT,
  FOR_REFRESH_OUT,  //遇到左花括号弹出,压入FOR_BODY_IN,换行
  FOR_BODY_IN,      //接受无条件块,遇到分号换行,遇到右花括号弹出,出块,换行
  sentence_kind_num //用最后一个enum来记录这些enum的数量
};




#endif