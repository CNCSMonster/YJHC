#ifndef _TOKEN_KIND
#define _TOKEN_KIND

//�ڸ�ͷ�ļ��ж���token���,���಻�Ǻ�ϸ��,��Ϊʵ�����ò��˷�̫��
typedef enum tokenkind{
  UNKNOWN,       //δ֪����,û��ȷ�����͵�id,������typedef��������ͱ���
  CONTROL,        //���̿��ƹؼ���,����if,else,do,while,for,return(��ʱ������GOTO)
  TYPE,     //������
  VAR,      //������
  CONST,  //������,�����ַ���,��������,�Լ�ʹ��Const����ĳ���
  SEMICOLON,      //�ֺ�
  COMMA,      //����
  LEFT_PARENTHESIS,     //��Բ����
  RIGHT_PARENTHESIS,  //��Բ����  
  LEFT_BRACKET,   //������
  RIGHT_BRACKET,    //�ҷ�����
  LEFT_BRACE,   //������
  RIGHT_BRACE,  //�һ�����
  //�����,������Ա���ʷ�,�����÷�(*),ȡ��ַ�������������(���г˺�*)�Լ��߼������
  //,�Լ�ָ���������η�(*),��Ҫע����Ÿ��õĴ���
  OP,
  POINTER, //ָ������,��Ҫע�����ָ�����Ϳ���ָ������ĵ�ַ(Ϊ����,��֧��ָ����ýṹ�巽��)
  FUNC
}TokenKind;


//����token���Ͷ�Ӧ���ַ�������
const char* tokenStrings[]={
  [UNKNOWN] "unknown",
  [CONTROL] "control keywords",
  [TYPE]  "type",
  [VAR] "variable", //������
  [CONST] "const value",  //������
  [SEMICOLON] "�ֺ�",      //�ֺ�
  [COMMA] "����",      //����
  [LEFT_PARENTHESIS] "��Բ����",     //��Բ����
  [RIGHT_PARENTHESIS] "��Բ����",  //��Բ����  
  [LEFT_BRACKET] "������",   //������
  [RIGHT_BRACKET] "�ҷ�����",    //�ҷ�����
  [LEFT_BRACE] "������",   //������
  [RIGHT_BRACE] "�һ�����",  //�һ�����
  [OP]  "operation symbol",
  [POINTER] "pointer var id",  //ָ�������
  [FUNC] "function"
};

//�������̿����ַ���
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



//���������������
const char* basetypes[]={
    "int",
    "short",
    "double",
    "long",
    "char",
    "void"  //��ʾ������
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