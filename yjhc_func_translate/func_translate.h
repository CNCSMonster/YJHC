#ifndef _FUNC_TRANSLATE_H
#define _FUNC_TRANSLATE_H


#include "str_set.h"
#include "string_index.h"
#include "syntax_symbol.h"
#include "func.h"
#include "type.h"
#include "val_tbl.h"
#include "token_reader.h"

FILE* ft_fout;  //函数使用的输出

//一些函数翻译用到的内容
//然后是正式开始翻译过程

int func_translate(FILE* funcFout,ValTbl* gloableValTbl,FuncTbl* funcTbl);


//单句翻译,传入的参数为tokens,funcFout,还有局部块partialValTbl,以及全局函数表funcTbl
int func_translateSingleSentence(TBNode* tokens,FILE* funcFout,ValTbl* gloableValTbl,ValTbl* partialValTbl,FuncTbl* funcTbl);


/*
不同类型句子的翻译方法
*/


//翻译结束后动作








#endif