#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef long long unsigned int(*G)(const char*) ;


void sort(int i,int (*)(void*,void*));

void sort(int i,int (*cmp)(void*,void*)){

}


typedef struct zarr{
    int m;
    int arr[];
}ZArr;

int main(char* argv[])
{    

    ZArr a={
        .m=1
    };
    //0¿òÊý×é
    printf("%d",a.arr[10000]);

    return 1;
}