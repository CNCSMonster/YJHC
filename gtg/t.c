#include <stdio.h>

int arr[2][2]={
  [0] {[0] 1 },
  [1] {[0] 3 }
};

int main(){
  // for(int i=0;i<4)
  for(int i=0;i<2;i++){
    for(int j=0;j<2;j++)
    printf("%d\n",arr[i][j]);
  }
  return 0;
}