#include "all.h"

//命令里面要是有err,则设置if_show_err为1,否则初始为0
int main(int argc,char* argv[]){

  for(int i=1;i<argc;i++){
    if(strcmp("err",argv[i])==0) if_show_err=1;
  }
  
  init();
  // maintainOrd(ord=strcpy(malloc(20),"run res/yjhc.gtg"));
  // gtg_exit();
  while((ord=fgetOrd(stdin))!=NULL) 
    maintainOrd(ord);
  return 0;
}