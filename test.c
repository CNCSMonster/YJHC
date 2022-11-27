#include <stdlib.h>
#include <stdio.h>
// #include "yjhc/token_kind.h"
// #include "yjhc/token.h"
// #include "yjhc/token.c"
// #include "yjhc/mystring.c"
// #include "yjhc/token_reader.h"
// #include "yjhc/token_reader.c"



//上一条语句,如果是读到第一条语句,则上一条语句类型是init



int main(){
  //测试读取
 

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