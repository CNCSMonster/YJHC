#include "all.h"

//关闭文件退出系统
int closeFileAndExit(FILE* tokenFin,FILE* typeFin,FILE* globalFin,FILE* funcHeadFin,FILE* funcFout){
  if(tokenFin!=NULL) fclose(tokenFin);
    else printf("fail to open %s",tokensPath);
    if(typeFin!=NULL) fclose(typeFin);
    else printf("fail to open %s",typesPath);
    if(globalFin!=NULL) fclose(globalFin);
    else printf("fail to open %s",globalPath);
    if(funcHeadFin!=NULL) fclose(funcHeadFin);
    else printf("fail to open %s",funcHeadsPath);
    if(funcFout!=NULL) fclose(funcFout);
    else printf("fial to open %s",funcOutPath);
    exit(-1);
}






int main(){
  char* tokensPath="..\\out\\func_tokens.txt";
  char* typesPath="..\\out\\type.txt";
  char* globalPath="..\\out\\global.txt";
  char* funcHeadsPath="..\\out\\func_head.txt";
  char* funcOutPath="..\\out\\func_body_ts.txt";  //函数输出结果
  FILE* tokenFin=fopen(tokensPath,"r");
  FILE* typeFin=fopen(typesPath,"r");
  FILE* globalFin=fopen(globalPath,"r");
  FILE* funcHeadFin=fopen(funcHeadsPath,"r");
  FILE* funcFout=fopen(funcOutPath,"w");  //写入文件的地址
  //检查文件是否正常打开
  if(
    tokenFin==NULL
    ||typeFin==NULL
    ||globalFin==NULL
    ||funcHeadFin==NULL
    ||funcFout==NULL
  ){
    if(tokenFin!=NULL) fclose(tokenFin);
    else printf("fail to open %s",tokensPath);
    if(typeFin!=NULL) fclose(typeFin);
    else printf("fail to open %s",typesPath);
    if(globalFin!=NULL) fclose(globalFin);
    else printf("fail to open %s",globalPath);
    if(funcHeadFin!=NULL) fclose(funcHeadFin);
    else printf("fail to open %s",funcHeadsPath);
    if(funcFout!=NULL) fclose(funcFout);
    else printf("fial to open %s",funcOutPath);
    exit(-1);
  }
  init_token_reader(tokenFin); //初始化token读取器
  TypeTbl typeTbl=getGlobalTypeTbl();

  if(!loadFile_typeTbl(typeFin)){
    //关闭文件,然后退出
    delTypeTbl(&typeTbl);
    closeFileAndExit(tokenFin,typeFin,globalFin,funcHeadFin,funcFout);
  }
  //读取类型,全局量表,同样打印
  ValTbl valTbl=getValTbl(typeTbl);   //加载量表
  if(!loadFromFile_valtbl(&valTbl,globalFin)){
    del_valTbl(&valTbl);
    closeFileAndExit(tokenFin,typeFin,globalFin,funcHeadFin,funcFout);
  }
  //加载函数信息表
  FuncTbl funcTbl=getFuncTbl(&typeTbl);
  if(!loadFile_functbl(&funcTbl,funcHeadFin)){
    del_valTbl(&valTbl);
    del_functbl(&funcTbl);
    closeFileAndExit(tokenFin,typeFin,globalFin,funcHeadFin,funcFout);
  }
  //关闭不用到的三个文件句柄
  fclose(typeFin);
  fclose(globalFin);
  fclose(funcHeadFin);
  //开始正式的运行过程,函数翻译需要传递表进去三个表和两个句柄
  if(TODO){
    printf("err happen in func_translate\n");
  }
  del_functbl(&funcTbl);  //删除函数表
  //删除全部量表,类型表内置在量表中,会一同被释放资源
  ValTbl* tmpValTbl=valTbl.next;
  while(tmpValTbl!=NULL){
    valTbl.next=tmpValTbl->next;
    del_valTbl(tmpValTbl);
    free(tmpValTbl);
    tmpValTbl=valTbl.next;
  }
  del_valTbl(&valTbl);
  del_rest_token_reader();  //回收token读取器资源
  //关闭剩下所有文件
  fclose(tokenFin);
  fclose(funcFout);
  return 0;
}