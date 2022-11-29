#ifndef _TOKEN_KIND
#define _TOKEN_KIND

//在该头文件中定义token编号,分类不是很细致,因为实际上用不了分太多
typedef enum tokenkind{
  UNKNOWN,       //未知名称,没有确定类型的id,可能是typedef定义的类型别名
  CONTROL,        //流程控制关键字,包括if,else,do,while,for,return(暂时不包含GOTO)
  TYPE,     //类型名
  VAR,      //变量名
  CONST,  //常量名,包括字符串,各种数字,以及使用Const定义的常量
  SEMICOLON,      //分号
  COMMA,      //逗号
  LEFT_PARENTHESIS,     //左圆括号
  RIGHT_PARENTHESIS,  //右圆括号  
  LEFT_BRACKET,   //左方括号
  RIGHT_BRACKET,    //右方括号
  LEFT_BRACE,   //左花括号
  RIGHT_BRACE,  //右花括号
  //运算符,包括成员访问符,解引用符(*),取地址符，算术运算符(其中乘号*)以及逻辑运算符
  //,以及指针类型修饰符(*),需要注意符号复用的处理
  OP,
  POINTER, //指针类型,需要注意的是指针类型可能指向任意的地址(为方便,不支持指针调用结构体方法)
  FUNC
}TokenKind;


//定义token类型对应的字符串描述
const char* tokenStrings[]={
  [UNKNOWN] "unknown",
  [CONTROL] "control keywords",
  [TYPE]  "type",
  [VAR] "variable", //变量名
  [CONST] "const value",  //常量名
  [SEMICOLON] "分号",      //分号
  [COMMA] "逗号",      //逗号
  [LEFT_PARENTHESIS] "左圆括号",     //左圆括号
  [RIGHT_PARENTHESIS] "右圆括号",  //右圆括号  
  [LEFT_BRACKET] "左方括号",   //左方括号
  [RIGHT_BRACKET] "右方括号",    //右方括号
  [LEFT_BRACE] "左花括号",   //左花括号
  [RIGHT_BRACE] "右花括号",  //右花括号
  [OP]  "operation symbol",
  [POINTER] "pointer var id",  //指针变量名
  [FUNC] "function"
};

//定义流程控制字符串
const char* keyForProcessControl[]={
  "if",
  "else",
  "for",
  "do",
  "while",
  "return",
  "break",
  "continue"
};



//定义基础数据类型
const char* basetypes[]={
    "int",
    "short",
    "double",
    "long",
    "char",
    "void"  //表示无类型
};

const char *typeKeyWords[] = {
    "struct",
    "union",
    "enum"
};


const char seps[]      =    {';',      ',',  '(',            ')',               '[',        ']',           '{',       '}'        };
const TokenKind sepTokens[]={SEMICOLON,COMMA,LEFT_PARENTHESIS,RIGHT_PARENTHESIS,LEFT_BRACKET,RIGHT_BRACKET,LEFT_BRACE,RIGHT_BRACE};

const char operationElems[]={
  '+',
  '-',
  '*',
  '/',
  '^',
  '=',
  '>',
  '<',
  '.',
  '|',
  '&'
};

//有效的运算字符串集合
const char* operations[]={
  "+",
  "-",
  "*",
  "/",
  "^",  //异或
  "+=",
  "-=",
  "*=",
  "/=",
  "^=",  //
  "->", //
  ".",  //类型变量访问成员符
  "<=",
  ">=",
  "<",
  ">",
  ">>",
  ">>=",
  "<<",
  "<<=",
  "|",
  "|=",
  "&",
  "&=",
  "||",
  "||=",
  "&&",
  "&&=",
  "!", //取否定,！作为一元运算符
  "!="  //判不等符
};

#endif