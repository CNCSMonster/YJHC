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

int main(char* argv[])
{    
    int a,b,c;
    //���԰�λȡ��
    return 1;
}

