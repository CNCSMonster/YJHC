#include "all.h"






//传入的五个控制台参数依次

int main(int argc,char* argv[]){


  //实际使用的内容
  char* typesPath=argv[1];
  char* valPath=argv[2];
  char* funcPath=argv[3];
  char* tokenInPath=argv[4];
  char* tokenOutPath=argv[5];  //函数输出结果

  //debug使用的内容
  // char* typesPath="..\\out\\type.txt";
  // char* valPath="..\\out\\global.txt";
  // char* funcPath="..\\out\\func_head.txt";
  // char* tokenInPath="..\\out\\func_tokens.txt";
  // char* tokenOutPath="..\\out\\func_tokens_after.txt";  //函数输出结果

  //首先加载函数翻译器
  FuncTranslator funcTranslator=getFuncTranslator(typesPath,funcPath,valPath);
  if(!func_translate(&funcTranslator,tokenInPath,tokenOutPath)){
    printf("fail to translate\n");
  }
  release_funcTranslator(&funcTranslator);
  return 0;
}