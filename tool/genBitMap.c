#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//使用宏函数
#define put(X) put_bitmap(&bmu,&bm,X)

#include "../mystl/bitmap.h"
#include "../mystl/bitmap.c"
#include "../yjhc_preProcess/token_kind.h"


//注意,程序用来产生func_translate要用到的sentence_kind.h
int main(){
  printf("#ifndef _CONFIG_H\n");
  printf("#define _CONFIG_H\n");
  //最多32
  BitMapUtil bmu={
    .mapSize=sizeof(long long)
  };
  BitMap bm=getBitMap(&bmu);
  //首先获取界符的集合,包括各种括号
  put(LEFT_BRACE);
  put(RIGHT_BRACE);
  put(LEFT_PAR);
  put(RIGHT_PAR);
  put(LEFT_BRACKET);
  put(RIGHT_BRACKET);
  put(COMMA);
  put(SEMICOLON);
  long long out;
  memcpy(&out,bm.map,sizeof(out));
  printf("const long long SEP_BITMAP = %lld;\n",out);
  //然后获取流程控制关键字的集合
  clear_bitmap(&bmu,&bm);
  put(DO);put(WHILE);put(FOR);
  put(IF);put(ELIF);put(ELSE);
  put(CONTINUE);put(BREAK);put(RETURN);
  memcpy(&out,bm.map,sizeof(out));
  printf("const long long CONTROL_KEYWORD_BITMAP = %lld;\n",out);
  //其他集合没必要
  printf("#endif\n");
  delBitMap(&bm);

  return 0;
}
