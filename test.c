#include <stdio.h>
#include <stdlib.h>

enum M{
    GG,
    MM
};

int main()
{
   enum M a;
   a=(enum M)100;   //enumû�жԴ�С������
   printf("%d\n",a);
    printf("%d\n", __STDC__);
    printf("%ld\n",__STDC_VERSION__);
    return 0;
}