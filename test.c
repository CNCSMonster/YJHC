#include <stdlib.h>
#include <stdio.h>


int arr[3][3]={
  [0] {[0] 1,[1] 2,[2] 3},
  [1] {4,5,6},
  [2] {7,8,9}
};





int main(){
  //���Դ�һ�����ļ�����ɨ��һ���ַ���
  FILE* f=fopen("0.txt","r");
  int i=0;
  int jud=fscanf(f,"%d",&i);
  printf("%d,%d\n",i,jud);
  jud=fscanf(f,"%d",&i);
  printf("%d,%d\n",i,jud);
  fclose(f);
  return 0;
}