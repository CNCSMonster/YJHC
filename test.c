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
    //���԰�λȡ��
    FILE* fin=fopen("gtg.txt","w");
    printf("%p\n",fin);
    FILE* fin2=fopen("gtg.txt","w");
    //���fin�Ƿ���ָ����ͬ���ļ�
    printf("%p\n",fin2);
    fprintf(fin,"11");
    fprintf(fin2,"22");
    fclose(fin);
    fclose(fin2);
    return 1;
}

