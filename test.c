#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef long long unsigned int(*G)(const char*);

int countA(int i){
    return i;
}
int countB(int j){
    return j*2;
}

int (*getCount(int i))(int){
    if(i==1) return countA;
    else return countB;
}

const int arr[]={1,2};

enum M{
    Ma=33,
    Mb
};

int main(char* argv[])
{    
    const int arr[3]={1,2,3};
    return 0;
}

