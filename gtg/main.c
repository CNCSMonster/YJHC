#include "all.h"

//��������Ҫ����err,������if_show_errΪ1,�����ʼΪ0
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