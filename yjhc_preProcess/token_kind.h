#ifndef _TOKEN_KIND
#define _TOKEN_KIND

//在该头文件中定义token编号,分类不是很细致,因为实际上用不了分太多
typedef enum tokenkind{
  TYPE,
CONST,
VAR,
TYPEDEF_KEYWORD,
CONST_KEYWORD,
IF,
ELIF,
ELSE,
DO,
WHILE,
FOR,
RETURN,
BREAK,
CONTINUE,
OP,
LEFT_PAR,
RIGHT_PAR,
LEFT_BRACKET,
RIGHT_BRACKET,
LEFT_BRACE,
RIGHT_BRACE,
SEMICOLON,
COMMA,
FUNC,
UNKNOWN,
SELF_KEYWORD,
Tokens_NUM
}TokenKind;

//定义关键字self的字符字面值，关键字self用来访问结构体或者共用体自身属性或者方法
#define SELF_STRING_VALUE "self"


//定义token类型对应的字符串描述
const char* tokenStrings[]={
  [UNKNOWN] "unknown",
  [IF]    "keyword if",
  [CONST_KEYWORD] "keyword const",
  [TYPEDEF_KEYWORD] "keyword typedef",
  [ELIF]  "keyword else if",
  [ELSE]  "keyword else",
  [FOR] "keyword for",
  [DO]  "keyword do in do-while structure",
  [WHILE] "keyword while in do-while structure",
  [CONTINUE]  "keyword continue",
  [BREAK] "keyword break",
  [RETURN]  "keyword return",
  [TYPE]  "type",
  [VAR] "variable", //变量名
  [CONST] "const value",  //常量名
  [SEMICOLON] "semicolon",      //分号
  [COMMA] "comma",      //逗号
  [LEFT_PAR] "left parenthesis",     //左圆括号
  [RIGHT_PAR] "right parenthesis",  //右圆括号  
  [LEFT_BRACKET] "left bracket",   //左方括号
  [RIGHT_BRACKET] "right bracket",    //右方括号
  [LEFT_BRACE] "left brace",   //左花括号
  [RIGHT_BRACE] "right brace",  //右花括号
  [OP]  "operation symbol",
  [SELF_KEYWORD] "self keyword",
  [FUNC] "function"
};

//定义流程控制字符串
const char* keyForProcessControl[]={
  "if","else","else if","for",
  "do","while","return","break",
  "continue"
};

const TokenKind controlTokens[]={
  IF,ELSE,ELIF,FOR,
  DO,WHILE,RETURN,BREAK,
  CONTINUE
};


//定义基础数据类型
const char* basetypes[]={
    "int",
    "short",
    "double",
    "long",
    "char",
    "void"  //表示非返回类型,一种特殊的类型，也被认为基础类型
};

const char *typeKeyWords[] = {
    "struct",
    "union",
    "enum"
};


const char seps[]      =    {';',      ',',  '(',            ')',               '[',        ']',           '{',       '}'        };
const TokenKind sepTokens[]={SEMICOLON,COMMA,LEFT_PAR,RIGHT_PAR,LEFT_BRACKET,RIGHT_BRACKET,LEFT_BRACE,RIGHT_BRACE};

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