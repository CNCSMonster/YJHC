#include "all.h"


int main(){

  init();
  while((ord=fgetOrd(stdin))!=NULL) maintainOrd(ord);
  return 0;
}