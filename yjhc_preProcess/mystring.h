#ifndef _MYSTRING_H
#define _MYSTRING_H

#include <stdio.h>
#include <string.h>
#include "token.h"
#include "token_kind.h"





//�ж��Ƿ����Զ������Ͷ���ؼ���
int isTypeDefKeyWords(char* s);
//�ж��Ƿ�һ���ַ����ǻ����������͹ؼ���,�Ƿ��ط�0ֵ�����Ƿ���0
int isBaseType(char* s);

int myfgets(char* buf,char* stops,FILE* fin);

int myStrHash(const char* str);




//���ַ�������ָ���ս����ȡһ���ַ���
int mysgets(char* buf,const char* stops,const char* src);

int mystrReplace(char* s,char old,char new);

//�����ַ�����������
int ungets(char* s,FILE* fin);

//���Ѿ���ȡ���ַ�����˫���ŵ������,���ļ��ж�ȡʣ�µ��ַ���,�������������c�﷨���ַ���,�򷵻�0,��ȡ�ɹ��򷵻ط�0ֵ
int freadRestString(FILE* fin,char* returnString);

//�ж��ַ��ܷ�ת��
//�ж��Ƿ��Ǻ����ת���ַ�
int isEscape(char c);

//�ж��Ƿ������̿��ƹؼ���,����������̿��ƹؼ���,���ظ���,�����,���ض�Ӧtokenkind
int isKeyForProcessControl(char* s);

//�ж��Ƿ��ǽ��,����Ƿ��ؽ����Ӧ��TokenKind,�������,���ط���ֵ
int isSep(char c);

//�ж��Ƿ����������ɻ��ߴ����������ɲ���
int isOp(char c);


#endif