#include "all.h"

//�ر��ļ��˳�ϵͳ
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
  char* funcOutPath="..\\out\\func_body_ts.txt";  //����������
  FILE* tokenFin=fopen(tokensPath,"r");
  FILE* typeFin=fopen(typesPath,"r");
  FILE* globalFin=fopen(globalPath,"r");
  FILE* funcHeadFin=fopen(funcHeadsPath,"r");
  FILE* funcFout=fopen(funcOutPath,"w");  //д���ļ��ĵ�ַ
  //����ļ��Ƿ�������
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
  init_token_reader(tokenFin); //��ʼ��token��ȡ��
  TypeTbl typeTbl=getGlobalTypeTbl();

  if(!loadFile_typeTbl(typeFin)){
    //�ر��ļ�,Ȼ���˳�
    delTypeTbl(&typeTbl);
    closeFileAndExit(tokenFin,typeFin,globalFin,funcHeadFin,funcFout);
  }
  //��ȡ����,ȫ������,ͬ����ӡ
  ValTbl valTbl=getValTbl(typeTbl);   //��������
  if(!loadFromFile_valtbl(&valTbl,globalFin)){
    del_valTbl(&valTbl);
    closeFileAndExit(tokenFin,typeFin,globalFin,funcHeadFin,funcFout);
  }
  //���غ�����Ϣ��
  FuncTbl funcTbl=getFuncTbl(&typeTbl);
  if(!loadFile_functbl(&funcTbl,funcHeadFin)){
    del_valTbl(&valTbl);
    del_functbl(&funcTbl);
    closeFileAndExit(tokenFin,typeFin,globalFin,funcHeadFin,funcFout);
  }
  //�رղ��õ��������ļ����
  fclose(typeFin);
  fclose(globalFin);
  fclose(funcHeadFin);
  //��ʼ��ʽ�����й���,����������Ҫ���ݱ��ȥ��������������
  if(TODO){
    printf("err happen in func_translate\n");
  }
  del_functbl(&funcTbl);  //ɾ��������
  //ɾ��ȫ������,���ͱ�������������,��һͬ���ͷ���Դ
  ValTbl* tmpValTbl=valTbl.next;
  while(tmpValTbl!=NULL){
    valTbl.next=tmpValTbl->next;
    del_valTbl(tmpValTbl);
    free(tmpValTbl);
    tmpValTbl=valTbl.next;
  }
  del_valTbl(&valTbl);
  del_rest_token_reader();  //����token��ȡ����Դ
  //�ر�ʣ�������ļ�
  fclose(tokenFin);
  fclose(funcFout);
  return 0;
}