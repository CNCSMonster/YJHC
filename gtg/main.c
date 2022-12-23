#include "all.h"


int main(){

  init();
  // maintainOrd(ord=strcpy(malloc(20),"run res/yjhc.gtg"));
  // gtg_exit();
  while((ord=fgetOrd(stdin))!=NULL) 
    maintainOrd(ord);
  return 0;
}