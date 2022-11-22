#include <stdlib.h>
#include <stdio.h>
#define M a_b
#define N 33
#define G printf("nihaoaggb%d",N);
#define PL printf("niaho%d"
#define PR ,N);
#define T a.age=M;


#define E =

struct stu{
  int age;
};


int main(){
  //测试c语言能否按照指定格式忽略读取
  char s[20];
  char s2[20];
  scanf("%s",s);
  scanf("%s",s2);
  printf("s1:%s\ns2:%s\n",s,s2);
  //不能,scanf只能够对开头进行匹配,不能够对结果预测

  int d=sprintf(s,"%s,nihao%d",s2,11);
  printf("%d",d);

  // int a_b=3;
  // char* name E"33";
  // printf("ggb%s\n",name);
  // printf("%d",M); //这个语句中M会被替换成a_b
  // struct stu a;
  // PL PR
  // T
  // printf("%d",a.age);
  return 0;
}