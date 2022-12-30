#include <stdio.h>
#include <stdlib.h>

#include "token_reader.c"
#include "token_reader.h"

#include "token.h"
#include "token.c"

#include "mystring.h"
#include "mystring.c"


//首先判断是否需要进行函数翻译


//然后进行翻译


//函数翻译要采用各种表的信息,要全局量表,包括常量和变量,
//要自自定义类型信息表(这个表不需要修改),要能够快速查找类型定义的成员量和成员函数
//还要有局部变量表,这个表表需要能够扩展以及进行块的区分
//使用方法ft <函数代码token文件> <函数头文件> <全局量文件> <类型定义文件> <函数声明文件> <函数输出文件>
int main(){
  //读取，然后打印每个块的语句
  char* tokensPath="..\\out\\func_tokens.txt";
  FILE* fin=fopen(tokensPath,"r");
  
  init_token_reader(fin);
  TBNode* tmp;
  BlockAction ba;
  PrintAction pa;
  while((tmp=readTokenSentence(&ba,&pa))!=NULL){
    //打印一下
    TBNode* tr=tmp;
    do{
      printf("%s ",tr->token.val);
      tr=tr->next;
    }while(tr!=NULL);
    // if(pa==PRINTENTER) printf("\tenter");
    // else printf("\tspace");
    if(pa==PRINTENTER) printf("\n");
    del_tokenLine(tmp);
  }
  del_rest();
  fclose(fin);

  return 0;
}