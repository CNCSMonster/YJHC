//�������Ը���ģ��Ĺ���
#include "all.h"

//�����ж��Ƿ���Ҫ���д�ӡ






// //����val_tbl�Ĳ��ҹ���
// int main(){
  // //���ȴ���val_tbl
  // TypeTbl typeTbl=getGlobalTypeTbl();
  // FILE* fin=fopen("..\\out\\func_type.txt","r");
  // loadFile_typeTbl(&typeTbl,fin);
  // fclose(fin);
  // ValTbl valTbl=getValTbl(typeTbl);
  // fin=fopen("..\\out\\global.txt","r");
  // loadFromFile_valtbl(&valTbl,fin);
  // fclose(fin);
  // //���в�����ʾ
  // do{
  //   char name[500];
  //   scanf("%s",name);
  //   printf("\n**********************\n");
  //   if(strcmp(name,"*")==0){
  //     break;
  //   }
  //   Val val;
  //   Type type;
  //   int retLayer;
//     int jud=findVal(&valTbl,name,&val,&type,&retLayer);
//     if(jud==0){
//       printf("not such val!\n");
//       continue;
//     }
//     printf("valName:%s\n",val.name);
//     printf("isConst:%d",val.isConst);
//     printf("initVal:%s",val.val);
//     //������Ϣ
//     showType(&type);
//   }while(1);
//   del_valTbl(&valTbl);
//   return 0;
// }

// //����func_tbl�Ĳ��ҹ���
// int main(){
//   //���ȼ������ͱ�
//   TypeTbl typeTbl=getGlobalTypeTbl();
//   FILE* fin=fopen("..\\out\\type.txt","r");
//   loadFile_typeTbl(&typeTbl,fin);
//   fclose(fin);
//   fin=fopen("..\\out\\func_head.txt","r");
//   FuncTbl funcTbl=getFuncTbl(&typeTbl);
//   loadFile_functbl(&funcTbl,fin);
//   fclose(fin);
//   //���в�ѯ,�������ֺ����˲�ѯ
//   char funcName[1000];
//   char ownerName[1000];
//   //�鿴��������
//   for(int i=0;i<typeTbl.types.size;i++){
//     Type type;
//     vector_get(&typeTbl.types,i,&type);
//     printf("type %d.\n",i+1);
//     showType(&type);
//   }
//   do{
//     printf("****************\n");
//     printf("input funcName:");
//     myfgets(funcName,"\n\r",stdin);
//     printf("input ownerName:");
//     myfgets(ownerName,"\n\r",stdin);
//     Func* find=NULL;
//     if(strlen(ownerName)==0){
//       find=findFunc(&funcTbl,funcName,NULL);
//     }
//     else{
//       find=findFunc(&funcTbl,funcName,ownerName);
//     }
//     if(find!=NULL)
//       showFunc(find);
//     else
//       printf("not found\n");
//   }while(strcmp(ownerName,"*")!=0);
//   delTypeTbl(&typeTbl);
//   del_functbl(&funcTbl);
//   return 0;
// }


// // ����token_reader�ķֶζ�����
// int main(){
//   FILE* fin=fopen("..\\out\\func_tokens.txt","r");
//   init_token_reader(fin);
//   TBNode* nodes;
//   ActionSet actionSet;
//   while((nodes=readTokenSentence(&actionSet))!=NULL){
//     show_tokenLine(nodes);
//     printf("\t$printAction:%d\n",actionSet.printAction);
//     // if(actionSet.printAction==PRINTENTER) printf("\n");
//     // else printf(" ");
//     del_tokenLine(nodes);
//   }
//   fclose(fin);
//   del_rest_token_reader();
//   return 1;
// }

//���Ը�ʽ�����token
//����token_reader�ķֶζ�����
int main(){
  FILE* fin=fopen("..\\out\\func_tokens.txt","r");
  init_token_reader(fin);
  TBNode* nodes;
  ActionSet actionSet;
  int preblocks=0;
  while((nodes=readTokenSentence(&actionSet))!=NULL){
    //����ǽ��뺯��,�Ȼ���
    show_tokenLine(nodes);
    if(actionSet.printAction==PRINTENTER){
      printf("\n");
      //�����ӡenter��,��ӡ�Ϳ����൱����������
      int n=actionSet.blocks;
      // if(strcmp(nodes->token.val,"}")==0) n--;
      for(int i=0;i<n;i++) printf("\t");
    }
    else printf(" ");
    preblocks=actionSet.blocks;
    del_tokenLine(nodes);
  }
  fclose(fin);
  del_rest_token_reader();
  return 1;
}




// //����typetbl�Ĳ��ҹ��ܺ��������͹���
// int main(){
//   return 0;
// }


