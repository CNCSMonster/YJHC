#include "func_translate.h"

//首先判断是否需要进行打印

int main(){
  //读取，然后打印每个块的语句
  char* tokensPath="..\\out\\func_tokens.txt";
  char* typesPath="..\\out\\type.txt";
  char* globalPath="..\\out\\global.txt";
  FILE* fin=fopen(tokensPath,"r");
  FILE* typeFin=fopen(typesPath,"r");
  FILE* globalFin=fopen(globalPath,"r");
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

  //读取类型,然后打印
  TypeTbl typeTbl=loadFile_typeTbl(typeFin);
  // //打印每个类型
  // for(int i=0;i<typeTbl.types.size;i++){
  //   printf("%d\n\n",i);
  //   Type type;
  //   vector_get(&typeTbl.types,i,&type);
  //   showType(&type);
  // }
  // delTypeTbl(&typeTbl); //如果删除链表了就不用删除这个

  //读取类型,全局变量表,同样打印
  ValTbl valTbl=getValTbl(typeTbl);
  loadFromFile_valtbl(&valTbl,globalFin);
  //打印每个变量
  for(int i=0;i<valTbl.vals.size;i++){
    //打印字符串信息
    printf("\n\n%d.\n",i+1);
    Val val;
    vector_get(&valTbl.vals,i,&val);
    printf("valName:%s\nisConst:%d\nval:%s\n",val.name,val.isConst,val.val);
    //获取类型名信息
    char* typeName;
    hashtbl_get(&valTbl.valToType,&val.name,&typeName);
    printf("typeName:%s\n",typeName);
    //获取类型信息
    int layer;
    int index=findType(&valTbl.typeTbl,typeName,&layer);
    //打印类型信息,
    Type type;
    vector_get(&valTbl.typeTbl.types,index,&type);
    showType(&type);
  }
  del_valTbl(&valTbl);
  fclose(typeFin);
  fclose(globalFin);

  return 0;
}












