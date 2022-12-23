#include "myprint.h"

//打印分界符号,指定长度
void printSplit(int len,char* c){
  while(len-->0) printf("%s",c);
  printf("\n");
}

//打印短分界符
void printShortSplit(){
  printSplit(30,"#");
}

//打印中分界符
void printMidSplit(){
  printSplit(55,"#");
}

//打印长分界符
void printLongSplit(){
  printSplit(100,"#");
}















