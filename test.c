#include <stdlib.h>
#include <stdio.h>
#include "yjhc/token_kind.h"
union ttt
{
  int ti;
  double tf;
  char* ts;
};


int main(){
  union  ttt* a;
  a->tf=2.22;
  
  return 0;
}