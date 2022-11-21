#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mystring.h"
#include "mystring.c"

//关于局部结构体定义,局部结构体定义不支持函数
//定义宏定义结构体
struct poundline{
  char* name;
  char* value;
};

//从文件中读取一个宏定义表,通过指针返回读取到的宏定义表的大小
struct poundline* loadPLs(FILE* pound_fin,int* returnSize);


//进行宏替换,与一般字符串替换不同,宏替换有一定语义分析
char* pound_substitute_string(char* toChange,struct poundline val);

//去除宏嵌套
char* pound_remove_nest(struct poundline* PLs,int size);


//主要功能方法
int pound_substitute_file(FILE* code_fin,FILE* pound_fin,FILE* fout);





/*
过程:
1.从文件中加载宏,选择合适的结构保存
2.对宏进行化简，去除嵌套
3.对文件内容进行替换

1.加载宏
信息有宏名,宏值,均为字符串
使用动态数组读取,保存到固定长度数组中
因为实际上宏的长度应该是有限的,合理的程序中宏的使用值基本不可能超过2000长度
2.宏化简
使用一个局部的大字符数组来辅助化简
具体做法就是以分界符进行读取,如果匹配到宏的内容就替换,否则就不替换
每个宏都对其他所有宏进行宏替换操作
最后得到的宏之间两两都不存在替换关系
3.文件替换
使用化简后的宏对文件内容进行替换
*/


/*
问题分析:
对于
#define M a_b
下面
int M_c;//变量名中部分匹配的M不会被替换
int a_b=3;
printf("%d",M); //这个语句中M会被替换成a_b

总结就是只有独立元才能够进行宏替换
独立元包括,作为参数的整体,作为运算数的整体,或者是出现的一个宏元
也就是说独立元前后应该是逗号或者分号或者空格或者换行或者小括号或者运算符或者方括号
这样的独立元才能够进行宏替换

另外需要注意的是宏嵌套的处理


*/



int main(int argc ,char* argv[]){

  if(argc!=4){
    printf("wrong!miss argument in doc_del");
    exit(-1);
  }
  FILE* code_fin=fopen(argv[1],"r"); //打开要读取文件，获取句柄
  // FILE* fin=fopen("code.yjhc","r"); //打开要读取文件，获取句柄
  if(!code_fin){
    //如果打开文件失败
    printf("fail to open %s",argv[1]);
    exit(-1);
  }
  FILE* pound_fin=fopen(argv[2],"r");
  // FILE* fout=fopen("out.txt","w"); //打开要读取文件，获取句柄
  if(!code_fin){
    fclose(code_fin);
    printf("fail to open %s for write",argv[2]);
    exit(-1);
  }
  FILE* fout=fopen(argv[3],"w");
  // FILE* fout=fopen("out.txt","w"); //打开要读取文件，获取句柄
  if(!fout){
    fclose(code_fin);
    fclose(pound_fin);
    printf("fail to open %s for write",argv[3]);
    exit(-1);
  }
  if(
    pound_substitute_file(code_fin,pound_fin,fout)
  ){
    printf("syntax error for pound_substitute");
  }
  fclose(code_fin);
  fclose(pound_fin);
  fclose(fout);
  return 0;
}



//从文件中读取一个宏定义表,通过指针返回读取到的宏定义表的大小
struct poundline* loadPLs(FILE* pound_fin,int* returnSize){
  char tmp[2000];
  char tmp1[1000];
  int n=0;  //用来统计读取到的pound_fin的数量
  int capacity=100; //定义初始容量
  //使用一个动态数组来存储pound_fin
  struct poundline* PLs=(struct poundline*)malloc(sizeof(struct poundline)*capacity);

  char end=myfgets(tmp,"\n",pound_fin);
  while(end!=EOF&&end=='\n'){
    //开始读取
    mysgets(tmp1," ",tmp);
    //如果是预编译包含指令,则跳过改行
    if(strcmp(tmp1,"#include")==0){
      end=myfgets(tmp,"\n",pound_fin);
      continue;
    }
    //如果是宏替换语句,则进行提取
    else if(strcmp(tmp1,"#define")){
      end=mysgets(tmp1," ",tmp+strlen("#define")+1);  //读取下一个字符串
      //如果遇到异常,异常处理
      if(end!=' '){
        for(int k=0;k<n;k++){free(PLs[k].name);free(PLs[k].value);}free(PLs);
        *returnSize=0;
        return NULL;
      }
      //如果没有遇到异常,
      PLs[n].name=(char*)malloc(sizeof(char)*(strlen(tmp1)+1));
      strcpy(PLs[n].name,tmp1);
      //第二个字符应该用\0作为终结符
      char* next=tmp+strlen("#define")+1+strlen(tmp1)+1;
      end=mysgets(tmp1,"\0",next);  //读取下一个字符串
      //如果遇到异常,异常处理
      if(end!='\0'){
        //否则语法异常
        printf("syntax error!each pound sentence should take and only take single line!");
        for(int k=0;k<n;k++){free(PLs[k].name);free(PLs[k].value);}free(PLs);
        *returnSize=0;
        return NULL;
      }
      PLs[n].value=(char*)malloc(sizeof(char)*(strlen(tmp1)+1));
      strcpy(PLs[n].value,tmp1);
      n++;
      //判断n的大小是否已经达到了上限,如果达到了,增加一半+1
      //如果容量不充足了,进行扩充
      if(n==capacity){
        int newCapacity=capacity*3/2+1;
        void* newMem=malloc(sizeof(struct poundline)*newCapacity);
        capacity=newCapacity;
        memcpy(newMem,PLs,sizeof(struct poundline)*n);
        free(PLs);
      }
      //完成扩充后读取下一行,进入下一个语句
      end=myfgets(tmp,"\n",pound_fin);
    }
    else{
      //否则语法异常
      printf("syntax error!each pound sentence should take and only take single line!");
      //清除之前分配的所有空间,并返回NULL
      //逐个清楚
      for(int k=0;k<n;k++){free(PLs[k].name);free(PLs[k].value);}free(PLs);
      *returnSize=0;
      return NULL;
    }
  }
  //出现了异常错误
  if(end!=EOF){
    printf("syntax error!the last pound line should have a enter");
    //清除之前分配的所有空间,并返回NULL
    //逐个清楚
    for(int k=0;k<n;k++){free(PLs[k].name);free(PLs[k].value);}free(PLs);
    *returnSize=0;
    return NULL;
  }
  //返回部分
  void* out=malloc(sizeof(struct poundline)*n);
  memcpy(out,PLs,sizeof(struct poundline)*n);
  free(PLs);
  *returnSize=n;
  return (struct poundline*)out;
}



//进行宏替换,与一般字符串替换不同,宏替换有一定语法要求
char* pound_substitute_string(char* toChange,struct poundline val){
  //
  //对于字符串量,应该进行特殊的处理,因为字符串中就算匹配了宏名也不会被替换
  //宏值里面不支持半个字符串,也就是里面的字符串前后分界符"成对出现,
  //需要注意的是成对出现的分界符里面可能有转义的\"单独出现,也就是说，
  //在字符串里面\和"连续地组成一个整体的时候,可以任意地出现
  //总之,宏替换之后不会组合形成新的字符串,一个字符串要么原本就在宏定义里面,要么就在宏外面
  //所以宏与外部,宏与宏之间不会形成新的字符串
  //所以宏内部对val进行替换是全局替换的有效子结构,
  //也就是说理论上我们能够先对宏组合之间的宏两两互相做宏替换来化简去除嵌套
  //此外,如果我们要去去除嵌套的宏value是一个整体,也就是其内没有分界符,则不应该进行替换
  char* stops=";\n ,)(.[]{}+-*/^<>=&|\\\"";
  char tmp[2000];
  char tmp1[200];
  int i=0;
  char end;
  while((end=mysgets(tmp1,stops,toChange))!='\0'){
    toChange+=strlen(tmp1)+1;
    //如果遇到空串,不需要写回,也不需要替换
    if(strcmp(tmp1,"")==0){
      ;
    }
    //如果需要替换,进行替换,或者说,把替换后的内容写入文件中
    else if(strcmp(tmp1,val.name)==0){
      strcpy(tmp+i,val.value);
      i+=strlen(val.value);
    }
    //否则不需要替换,原样写入
    else{
      strcpy(tmp+i,tmp1);
      i+=strlen(tmp1);
    }
    tmp[i++]=end;
    //根据读取到的终结符是否是双引号判断是否要跳过后面字符串部分
    if(end=='\"'){
      //循环读取处理过程中的
      char* stops2="\"\\";
      int len=strlen(tmp1);
      while((end=mysgets(tmp1,stops2,toChange))!='\0'&&end!='\"'){
        toChange+=strlen(tmp1)+1;
        //如果这个字符和后面连在一起的部分构成转义字符,一起读出
        if(end=='\\'&&(
          toChange[0]=='"'||
          toChange[0]=='\\'||
          toChange[0]=='n'||
          toChange[0]=='t'||
          toChange[0]=='b'||
          toChange[0]=='r'
        )){
          tmp[i++]=toChange[0];
          toChange++;
        }
        //否则只是单纯的一个反斜杠,
        else{

        }

      }
      //如果读到字符串末尾了都没有出现字符串的双引号,则报异常
      if(end!='\"'){
        return NULL;  //返回NULL表示异常,也就是宏替换异常,因为宏替换只能够在字符串外进行
      }else{
        continue;
      }
    }
    tmp[i++]=end;
  }
  //TODO,遇到\0退出后还有最后一次分离出的tmp1要处理







  //完成上述替换后,生成合适大小的空间保存结果并返回
  //首先最后要加入字符串结束符
  tmp[i++]='\0';
  char* out=(char*)malloc(sizeof(char)*(i));
  strcpy(out,tmp);
  return out;
}

//去除宏嵌套,不需要返回值
void pound_remove_nest(struct poundline* PLs,int size){
  for(int i=0;i<size;i++){
    for(int j=0;j<size;j++){
      if(i==j) continue;  //不允许宏定义有自身嵌套
      char* newVal=pound_substitute_string(PLs[i].value,PLs[j]);
      free(PLs[i].value);
      PLs[i].value=newVal;
    }
  }
}


//主要功能方法
int pound_substitute_file(FILE* code_fin,FILE* pound_fin,FILE* fout){
  //首先,先加载宏定义表
  int size;
  struct poundline* PLs=loadPLs(pound_fin,&size);
  //然后,对宏定义表进行去除嵌套处理
  pound_remove_nest(PLs,size);
  //然后使用宏定义表对文件代码进行替换
  char tmp[1000];
  char* stops=",;"


}