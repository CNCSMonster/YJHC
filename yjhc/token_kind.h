#ifndef _TOKEN_KIND
#define _TOKEN_KIND

//�ڸ�ͷ�ļ��ж���token���,���಻�Ǻ�ϸ��,��Ϊʵ�����ò��˷�̫��
typedef enum tokenkind{
  UNKNOWN,        //δ֪����,û��ȷ�����͵�id,������typedef��������ͱ���
  //���ƹؼ���Ҳ�ָ��С����
  IF, 
  ELIF, 
  ELSE, 
  FOR, 
  DO, 
  WHILE, 
  CONTINUE,  
  BREAK, 
  RETURN, 
  TYPE,     //������
  VAR,      //������
  CONST,  //������,�����ַ���,��������,�Լ�ʹ��Const����ĳ���
  SEMICOLON,      //�ֺ�
  COMMA,      //����
  LEFT_PAR,     //��Բ����
  RIGHT_PAR,  //��Բ����  
  LEFT_BRACKET,   //������
  RIGHT_BRACKET,    //�ҷ�����
  LEFT_BRACE,   //������
  RIGHT_BRACE,  //�һ�����
  //�����,������Ա���ʷ�,�����÷�(*),ȡ��ַ�������������(���г˺�*)�Լ��߼������
  //,�Լ�ָ���������η�(*),��Ҫע����Ÿ��õĴ���
  OP,
  FUNC,   //������
  Tokens_NUM  //����enum������
}TokenKind;





//����token���Ͷ�Ӧ���ַ�������
const char* tokenStrings[]={
  [UNKNOWN] "unknown",
  [IF]    "keyword if",
  [ELIF]  "keyword else if",
  [ELSE]  "keyword else",
  [FOR] "keyword for",
  [DO]  "keyword do in do-while structure",
  [WHILE] "keyword while in do-while structure",
  [CONTINUE]  "keyword continue",
  [BREAK] "keyword break",
  [RETURN]  "keyword return",
  [TYPE]  "type",
  [VAR] "variable", //������
  [CONST] "const value",  //������
  [SEMICOLON] "semicolon",      //�ֺ�
  [COMMA] "comma",      //����
  [LEFT_PAR] "left parenthesis",     //��Բ����
  [RIGHT_PAR] "right parenthesis",  //��Բ����  
  [LEFT_BRACKET] "left bracket",   //������
  [RIGHT_BRACKET] "right bracket",    //�ҷ�����
  [LEFT_BRACE] "left brace",   //������
  [RIGHT_BRACE] "right brace",  //�һ�����
  [OP]  "operation symbol",
  [FUNC] "function"
};

//�������̿����ַ���
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



//���������������
const char* basetypes[]={
    "int",
    "short",
    "double",
    "long",
    "char",
    "void"  //��ʾ�Ƿ�������,һ�����������
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

//��Ч�������ַ�������
const char* operations[]={
  "+",
  "-",
  "*",
  "/",
  "^",  //���
  "+=",
  "-=",
  "*=",
  "/=",
  "^=",  //
  "->", //
  ".",  //���ͱ������ʳ�Ա��
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
  "!", //ȡ��,����ΪһԪ�����
  "!="  //�в��ȷ�
};

#endif