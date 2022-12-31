#include <stdlib.h>
#include <stdio.h>


int arr[3][3]={
  [0] {[0] 1,[1] 2},
  [1] {4,5,6},
  [2] {7,8,9}
};


struct as{
  char* str;
};

struct as a={"ÄãºÃ"};


typedef int M;



int main(){
  // printf("%d",sizeof(float));
  // for(int i=0;i<sizeof(arr)/sizeof(arr[0]);i++){
  //   for(int j=0;j<sizeof(arr[0])/sizeof(arr[0][0]);j++){
  //     printf("%d,",arr[i][j]);
  //   }
  // }
  typedef M N;
  N bb;
  printf("%d",sizeof(bb));
  
  return 0;
}