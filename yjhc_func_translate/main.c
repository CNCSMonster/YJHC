#include "all.h"






//������������̨��������

int main(int argc,char* argv[]){


  //ʵ��ʹ�õ�����
  char* typesPath=argv[1];
  char* valPath=argv[2];
  char* funcPath=argv[3];
  char* tokenInPath=argv[4];
  char* tokenOutPath=argv[5];  //����������

  //debugʹ�õ�����
  // char* typesPath="..\\out\\type.txt";
  // char* valPath="..\\out\\global.txt";
  // char* funcPath="..\\out\\func_head.txt";
  // char* tokenInPath="..\\out\\func_tokens.txt";
  // char* tokenOutPath="..\\out\\func_tokens_after.txt";  //����������

  //���ȼ��غ���������
  FuncTranslator funcTranslator=getFuncTranslator(typesPath,funcPath,valPath);
  if(!func_translate(&funcTranslator,tokenInPath,tokenOutPath)){
    printf("fail to translate\n");
  }
  release_funcTranslator(&funcTranslator);
  return 0;
}