#include <stdlib.h>
#include <stdio.h>
// #include "yjhc/token_kind.h"
// #include "yjhc/token.h"
// #include "yjhc/token.c"
// #include "yjhc/mystring.c"
// #include "yjhc/token_reader.h"
// #include "yjhc/token_reader.c"



//��һ�����,����Ƕ�����һ�����,����һ�����������init



int main(){
  //���Զ�ȡ
 

  int i=1;
  scanf("%d",&i);
  switch(i){
    case 1:
    ;
      int a=1;
      int b=2;
    case 2:
    {
      int a=22;
      int b=33;
      printf("%d,%d",a,b);
    }
    case 3:printf("3");break;
    default: printf("default");
  }
  return 0;
}