#ifndef _FUNC_TRANSLATE_H
#define _FUNC_TRANSLATE_H


#include "str_set.h"
#include "string_index.h"
#include "syntax_symbol.h"
#include "func.h"
#include "type.h"
#include "val_tbl.h"
#include "token_reader.h"

FILE* ft_fout;  //����ʹ�õ����

//һЩ���������õ�������
//Ȼ������ʽ��ʼ�������

int func_translate(FILE* funcFout,ValTbl* gloableValTbl,FuncTbl* funcTbl);


//���䷭��,����Ĳ���Ϊtokens,funcFout,���оֲ���partialValTbl,�Լ�ȫ�ֺ�����funcTbl
int func_translateSingleSentence(TBNode* tokens,FILE* funcFout,ValTbl* gloableValTbl,ValTbl* partialValTbl,FuncTbl* funcTbl);


/*
��ͬ���;��ӵķ��뷽��
*/


//�����������








#endif