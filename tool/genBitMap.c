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
  printf("//界符位图\n");
  printf("const long long SEP_BITMAP = %lld;\n",out);
  //然后获取流程控制关键字的集合
  clear_bitmap(&bmu,&bm);
  put(DO);put(WHILE);put(FOR);
  put(IF);put(ELIF);put(ELSE);
  put(CONTINUE);put(BREAK);put(RETURN);
  memcpy(&out,bm.map,sizeof(out));
  printf("//流程控制关键字位图\n");
  printf("const long long CONTROL_KEYWORD_BITMAP = %lld;\n",out);
  //加入运算表达式的内容
  clear_bitmap(&bmu,&bm);
  put(OP);put(VAR);put(CONST);
  //加入运算层次括号
  put(LEFT_PAR);put(RIGHT_PAR);
  put(LEFT_BRACKET);put(RIGHT_BRACKET);
  put(LEFT_BRACE);put(RIGHT_BRACE);
  memcpy(&out,bm.map,sizeof(out));
  printf("//加入运算表达式位图\n");
  printf("const long long COUNT_BITMAP = %lld;\n",out);
  //非运算符位图

  printf("#endif\n");
  delBitMap(&bm);
  return 0;
}
