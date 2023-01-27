#ifndef _SENTENCE_KIND_H
#define _SENTENCE_KIND_H
#include "../yjhc_preProcess/token_kind.h"

//æ‰◊”¿‡–Õ
typedef enum sentence_kind{
  VAR_DEF_SENTENCE=Tokens_NUM+1,  
  CONST_DEF_SENTENCE, 
  TYPE_CHANGE_SENTENCE
}SentenceKind;









#endif
