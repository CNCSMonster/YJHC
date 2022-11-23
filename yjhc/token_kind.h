#ifndef _TOKEN_KIND
#define _TOKEN_KIND

//�ڸ�ͷ�ļ��ж���token���
typedef enum tokenkind{
  UNKOWN,       //δ֪����,û��ȷ�����͵�id,������typedef��������ͱ���
  CONTROL,        //���̿��ƹؼ���,����if,else,do,while,for,return(��ʱ������GOTO)
  TYPE,     //������
  VAR,      //������
  CONST,  //������,�����ַ���,��������,�Լ�ʹ��Const����ĳ���
  SEP,  //�ָ��,�����������źͷֺ��Լ�����
  //�����,������Ա���ʷ�,�����÷�(*),ȡ��ַ�������������(���г˺�*)�Լ��߼������
  //,�Լ�ָ���������η�(*),��Ҫע����Ÿ��õĴ���
  OP,
  POINTER, //ָ������,��Ҫע�����ָ�����Ϳ���ָ������ĵ�ַ(Ϊ����,��֧��ָ����ýṹ�巽��)
  FUNC
}TokenKind;


//����token���Ͷ�Ӧ���ַ�������
const char* tokenStrings[]={
  [UNKOWN] "unknown",
  [CONTROL] "control keywords",
  [TYPE]  "type",
  [VAR] "variable", //������
  [CONST] "const value",  //������
  [SEP] "seperator symbol",
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
  "return"
};

//������
const char sepratorSymbols[]={
  ';',
  '{',
  '}',
  '(',
  ')',
  '[',
  ']'
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

//������ַ���
const char* operations[]={
  "+",
  "-",
  "*",
  "/",
  "/",  //���
  "->", //
  ".",  //���ͱ������ʳ�Ա��
  "<=",
  ">=",
  "<",
  ">"
  ">>",
  "<<",
  "|",
  "&",
  "||",
  "&&",
  "!" //ȡ��
};

#endif