#include "func_translate.h"

//�����ж��Ƿ���Ҫ���д�ӡ

int main(){
  //��ȡ��Ȼ���ӡÿ��������
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
    //��ӡһ��
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

  //��ȡ����,Ȼ���ӡ
  TypeTbl typeTbl=loadFile_typeTbl(typeFin);
  // //��ӡÿ������
  // for(int i=0;i<typeTbl.types.size;i++){
  //   printf("%d\n\n",i);
  //   Type type;
  //   vector_get(&typeTbl.types,i,&type);
  //   showType(&type);
  // }
  // delTypeTbl(&typeTbl); //���ɾ�������˾Ͳ���ɾ�����

  //��ȡ����,ȫ�ֱ�����,ͬ����ӡ
  ValTbl valTbl=getValTbl(typeTbl);
  loadFromFile_valtbl(&valTbl,globalFin);
  //��ӡÿ������
  for(int i=0;i<valTbl.vals.size;i++){
    //��ӡ�ַ�����Ϣ
    printf("\n\n%d.\n",i+1);
    Val val;
    vector_get(&valTbl.vals,i,&val);
    printf("valName:%s\nisConst:%d\nval:%s\n",val.name,val.isConst,val.val);
    //��ȡ��������Ϣ
    char* typeName;
    hashtbl_get(&valTbl.valToType,&val.name,&typeName);
    printf("typeName:%s\n",typeName);
    //��ȡ������Ϣ
    int layer;
    int index=findType(&valTbl.typeTbl,typeName,&layer);
    //��ӡ������Ϣ,
    Type type;
    vector_get(&valTbl.typeTbl.types,index,&type);
    showType(&type);
  }
  del_valTbl(&valTbl);
  fclose(typeFin);
  fclose(globalFin);

  return 0;
}












