#include "myprint.h"

//��ӡ�ֽ����,ָ������
void printSplit(int len,char* c){
  while(len-->0) printf("%s",c);
  printf("\n");
}

//��ӡ�̷ֽ��
void printShortSplit(){
  printSplit(30,"#");
}

//��ӡ�зֽ��
void printMidSplit(){
  printSplit(55,"#");
}

//��ӡ���ֽ��
void printLongSplit(){
  printSplit(100,"#");
}















