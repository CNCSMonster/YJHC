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
  //����c�����ܷ���ָ����ʽ���Զ�ȡ
  char s[20];
  char s2[20];
  scanf("%s",s);
  scanf("%s",s2);
  printf("s1:%s\ns2:%s\n",s,s2);
  //����,scanfֻ�ܹ��Կ�ͷ����ƥ��,���ܹ��Խ��Ԥ��

  int d=sprintf(s,"%s,nihao%d",s2,11);
  printf("%d",d);

  // int a_b=3;
  // char* name E"33";
  // printf("ggb%s\n",name);
  // printf("%d",M); //��������M�ᱻ�滻��a_b
  // struct stu a;
  // PL PR
  // T
  // printf("%d",a.age);
  return 0;
}