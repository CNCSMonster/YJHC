#include <stdlib.h>
#include <stdio.h>
#include "yjhc/token_kind.h"
union ttt
{
  int ti;
  double tf;
  char* ts;
};
struct st{
  int a;
  char* name;
  double score;
};

struct st a ={1,"stname",2.33};


int main(){
  printf("%d,%s,%f",a.a,a.name,a.score);
  
  return 0;
}