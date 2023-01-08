#include <stdio.h>
#include <stdlib.h>


int main()
{
    int c=o123;
    
    volatile const int a=5;
    printf("%p\n",&a);
    int* m=(int*)&a;
    // int* t=&a;
    // int* m=(int*)*(&t);
    // int* m=(int*)6422036;
    // int* m=(int*)6422044;   //000000000061FE1C;
    printf("%p\n",m);
    //
    printf("%d\n",a);
    *m=3;
    printf("%d\n",*m);
    printf("%d\n",m==&a);
    // int* m=&a;
    // *m=4;
    // printf("%d",*m);
    exit(0);
    // return 4;
}