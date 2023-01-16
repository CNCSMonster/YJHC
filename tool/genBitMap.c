#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//ʹ�ú꺯��
#define put(X) put_bitmap(&bmu,&bm,X)

#include "../mystl/bitmap.h"
#include "../mystl/bitmap.c"
#include "../yjhc_preProcess/token_kind.h"


//ע��,������������func_translateҪ�õ���sentence_kind.h
int main(){
  printf("#ifndef _CONFIG_H\n");
  printf("#define _CONFIG_H\n");
  //���32
  BitMapUtil bmu={
    .mapSize=sizeof(long long)
  };
  BitMap bm=getBitMap(&bmu);
  //���Ȼ�ȡ����ļ���,������������
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
  //Ȼ���ȡ���̿��ƹؼ��ֵļ���
  clear_bitmap(&bmu,&bm);
  put(DO);put(WHILE);put(FOR);
  put(IF);put(ELIF);put(ELSE);
  put(CONTINUE);put(BREAK);put(RETURN);
  memcpy(&out,bm.map,sizeof(out));
  printf("const long long CONTROL_KEYWORD_BITMAP = %lld;\n",out);
  //��������û��Ҫ
  printf("#endif\n");
  delBitMap(&bm);

  return 0;
}
