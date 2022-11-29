#ifndef _YJHC_GRAMMER_H
#define _YJHC_GRAMMER_H


//语法表内容放置此处
//定义表示没有定义的符号not define
#define NOT_DEFINE (-1)


//进出块的三种可能
enum BlockAction{
  MOVEINTO, //进入新的块
  MOVEOUT,  //离开块
  STAY  //保持在块中
};

//是否换行的两种可能
enum PrintAction{
  NEWLINE,  //换行
  KEEPLINE  //不换行
};

//函数翻译的两种情况
enum FuncTranslateAction{
  FUNC_TS_NEED,   //需要翻译函数
  FUNC_TS_NOTNEED   //不需要翻译函数
}

/*
改模块主要负责代码格式化和局部快划分以及翻译指导
返回的是一个代码行以及相应的块动作和打印动作
是否进出块，是否要换行打印,是否要语义分析等(比如for(不用分析,直接打印即可),或者说
相应的是否换行是否进入新的局部块的分析token_reader中完成了


总结进入块的情况:
0. 进入函数
1. if条件语句()后面接花括号或者不接花括号(如果不接花括号的化默认该分支只包含一句内容)
2. else if条件语句()后面接花括号或者不接花括号
3. else关键字后面花括号代码段或者是没有花括号一个语句
4. for关键字面保罗init-condition-refresh结构以及循环体块
5. do关键字后面花括号代码块，或者没有花括号的一行分号结尾代码
6. while()条件语句后面单行代码或者花括号代码块

总结离开块的情况:
0.函数离开,花括号为标记
1.if,else if,else块和for块和while块,都是以花括号为标记,或者是以一行内容为标记(实际上分析的时候会格式化,没有分号地会插入分号)
2.do块离开，实际上do-while结构中,到了while里面就已经离开块了,while里面的内容不能够使用块中定义的量


//以下是按照笔者的编码风格
总结要换行的情况:
1.流程控制关键字要换行
2.选择结构if关键字要换行
3.循环结构关键字do或者是while或者是for要换行
4.块进入符号{要换行
5.块结束要换行
5.选择结构

总结不用换行的情况:
1.块内当行语句结束不用换行
2.for中init-condition-refresh语句块不用换行
3.选择结构,第一个逻辑表达式子式不用换行,之后每个外围逻辑联结词或者每个大括号换一行
4.单行计算语句多个式字组合不用换行
5.单行用括号隔开的赋值语句不用换行

结尾标记换行情况:
1.用;结尾的情况,除了for的init-condition-refresh(以下简称ICR块)不用换行以外,其他都要换行

for的ICR块中的init部分接受变量定义,
其他条件块中量范围属于外部量范围，使用外部量表，只有条件快后才进入局部区域

所以可以细化划分语法结构

1.普通顺序结构
遇到分号之前单行输入,遇到分号后换行
所以普通顺序结构里面有两种成分,分号之前成分,以及分号之后成分

2.if结构
if( if条件开始块(c语言里面if后面的逻辑表达式要用小括号括起来),要换行
逻辑条件  逻辑条件同行输入不用换行
) 逻辑条件结束符号,要换行
{ if块开始
if块内容
} if块结束,要换行输出

3.else if结构,只能够跟在if结构后面


4.else结构只能够跟在else if结构后面

5.{}结构,不是流程控制启动的块,而是单纯的一个块
{块开始符号,换行
块中语句,换行
}块结束语句,换行

6.do-while结构
do{ 块开始结构,换行
块中语句,换行
}while(   出块,进入循环逻辑判断语句,换行
)     do-while循环判断语句结束,不用换行


7.while结构
while(    while条件开始标志向
...       while条件逻辑表达式,不用换行
)       while条件结束标志
{       while块开始标志,不用换行
...     while块语句,换行
}     while块结束标志,换行,出块

8.for结构
for(    for结构开始标志,换行,进入块
init块    以分号结尾,不用换行
condition块 以分号结尾不用换行
refresh结构 以小括号结尾,不用换行
)         for的ICR结束标志,不用换行
{       for循环体开始标志,不用换行
...       for循环体语句,换行
}       for循环体结束标志,换行

9.switch结构(暂时不区分这个内容)
switch(     switch结构开始标志,换行
...       switch中的量表达式,不用换行
){       switch判断量表达式结尾以及块开始符
case    case关键字
...     case条件
:       case条件结束语


使用栈来保存上一条语句的内容,如果遇到块关键语句,则保存块关键语句类型。
如果遇到重复同类语句,则保存一个
比如遇到do{块开始标志,则把do{类型符号压入栈中
之后用栈顶的语句类型符号检查后面的语句类型是否符合语法要求
如果遇到了一条块内语句,则压入块内语句
如果遇到了第二条块内语句,则不用重复压入
如果遇到了进块符号,则压入.
如果遇到了出块符号,则取出之前压入的所有内容直到遇到第一个进块符号为止

比如
do{
  s1,g1,g2;
  s2;
  {
    s3;
  }
}while(q1&&q2);
则取法和栈内容为,因为块可以为空快,所以可以简化
do{     doBLockIn
s1    doBlockIn,partSentence,换行,
g1;   doBlockIn,partSentence   仍然没有完成句子,于是仍然不取出里面的parSentece
g2;   doBlockIn,
s2;     doBlockIn
{       doBlockIn,normalBlockIn
s3      doBlockIn,normalBlcockIn
}       doBlockIn,normalBlockOut     //出块,取出直到之前的进栈符号,压入出栈符号
}while(   doBlockOut        //出do-while语句,取出栈中直到之前的入do-while块为止的内容,压入出doBlock符号
q1      doBlockOut halfDoCondition  //一半的doconditon的逻辑语句
&&      connective          //connective类型
q2    doBlockOut DoCondtition   //完成了逻辑语句,把之前一半的逻辑语句取出,填入完成
);    //doCondition结束标志,遇到后,则把之前的出块以及到doCondition的符号都出栈,或者说把所有do-while结构的内容出栈,这个结构的所有内容已经处理完了





*/



//语句类型,如果是读到第一条语句前,则上一条语句类型是init
enum sentence_kind{
  INIT,     //还没有读取进入函数的状态,唯一接受的token就是left brace
  BLOCK_IN,   //进入了一个普通块或者函数块的栈符号,在这个状态下能够接受任何token,遇到一般的token当行打印,不用换行
  IF_CONDITION_IN,    //在该栈顶符号下,遇到左边括号加入栈中,
  LEFT_PARENTHESIS,   //左边小括号在栈顶,说明在某个condition中,如果遇到左圆括号压入栈顶,遇到右圆括号把左圆括号弹出
  IF_BLOCK_IN,    //当在IF_CONDITION_IN的栈顶符号下遇到左花括号,则进入IF_BLOCK_IN,换行,if_block_in中遇到任何的句子都会
  IF_BLOCK_OUT,   //当在ifblcokin栈顶遇到右花括号的时候,移出if_block_in,压入if_block_out
  ELIF_CONDITION_IN,    //当处于if_block_in的时候,如果遇到了else if关键字,就弹出if_block_out，压入elif_condition_in
  
  
};

//语义动作指导表:包括打印动作，块进出动作，以及辅助栈的栈动作

//第一个是打印动作指导表
const PrintAction printActionTbl[][]={
[init_token_reader] {}
[]

}


//第二个是块动作指导表


//第三个是函数翻译动作指导表







#endif