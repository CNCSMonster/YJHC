#include <stdio.h>
#include <stdlib.h>

enum M{
    GG,
    MM
};

int main()
{
   enum M a;
   a=(enum M)100;   //enum没有对大小有限制
   printf("%d\n",a);
    printf("%d\n", __STDC__);
    printf("%ld\n",__STDC_VERSION__);
    return 0;
}