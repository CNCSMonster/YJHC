//�������Ը���ģ��Ĺ���
#include "all.h"

//�����ж��Ƿ���Ҫ���д�ӡ






// //����val_tbl�Ĳ��ҹ���
// int main(){
//   //���ȴ���val_tbl
//   TypeTbl typeTbl=getGlobalTypeTbl();
//   FILE* fin=fopen("..\\out\\func_type.txt","r");
//   loadFile_typeTbl(&typeTbl,fin);
//   fclose(fin);
//   ValTbl valTbl=getValTbl(typeTbl);
//   fin=fopen("..\\out\\global.txt","r");
//   loadFile_valtbl(&valTbl,fin);
//   fclose(fin);
//   //���в�����ʾ
//   do{
//     char name[500];
//     scanf("%s",name);
//     printf("\n**********************\n");
//     if(strcmp(name,"*")==0){
//       break;
//     }
//     Val val;
//     Type type;
//     int retLayer;
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
//   release_token_reader();
//   return 1;
// }




// //����typetbl�Ĳ��ҹ��ܺ��������͹���
// int main(){
//   //���ȼ�������
//   //Ȼ�������������������,�Լ�������Ϣ
//   TypeTbl typeTbl=getGlobalTypeTbl();
//   FILE* fin=fopen("../out/type.txt","r");
//   loadFile_typeTbl(&typeTbl,fin);
//   fclose(fin);
//   while(1){
//     char tmp[1000];
//     printf("******************************************\n");
//     printf("input:");
//     myfgets(tmp,"\n",stdin);
//     if(strcmp(tmp,"exit()")==0){
//       printf("mm");
//       break;
//     }
//     //�����һ���ַ���<,��˵��������
//     if(tmp[0]=='<'){
//       if(!loadLine_typetbl(&typeTbl,tmp+1)){
//         printf("fail to execute %s",tmp+1);
//       }
//     }
//     //������������,����������
//     else{
//       int typeIndex;
//       int layer;
//       typeIndex=findType(&typeTbl,tmp,&layer);
//       if(typeIndex<0){
//         printf("???\n");
//         continue;
//       }
//       Type type;
//       vector_get(&typeTbl.types,typeIndex,&type);
//       printf("typeIndex:%d,layer:%d\n",typeIndex,layer);
//       showType(&type);
//     }
//   }
//   delTypeTbl(&typeTbl);
//   return 0;
// }

//����˳���ȡtype������

// //����func_tbl�Ĳ��ҹ���
// int main(){
//   //���ȼ���func_tbl
//   TypeTbl typeTbl=getGlobalTypeTbl(); //���Ȼ�ȡȫ�����ͱ�
//   //Ȼ����ļ��м������ͱ���Ϣ
//   FILE* fin=fopen("../out/type.txt","r");
//   loadFile_typeTbl(&typeTbl,fin);
//   fclose(fin);
//   FuncTbl funcTbl=getFuncTbl(&typeTbl);
//   //���ļ��м��غ�����Ϣ
//   fin=fopen("../out/func_head.txt","r");
//   loadFile_functbl(&funcTbl,fin);
//   fclose(fin);
//   //����˳��չʾfunc
//   for(int i=0;i<funcTbl.funcKeys.size;i++){
//     printf("\n\n****************************************\n\n");
//     char* key;
//     vector_get(&funcTbl.funcKeys,i,&key);
//     char funcName[300];
//     char ownerId[300];
//     mysgets(funcName,"#",key);
//     mysgets(ownerId,"",key+strlen(funcName)+1);
//     long long typeId=atoll(ownerId);
//     //��ȡ���˵�����
//     printf("funcName:%s\n",funcName); 
//     printf("onwer:\n");
    
//     //չʾonwer����
//     if(typeId==0) printf("null\n");
//     else{
//       int typeIndex;
//       extractTypeIndexAndPointerLayer(typeId,&typeIndex,NULL);
//       //��ȡ����
//       Type type;
//       vector_get(&(funcTbl.globalTypeTbl->types),typeIndex,&type);
//       showType(&type);
//     }
//     //��ӡ������Ϣ
//     Func* func;
//     hashtbl_get(&funcTbl.funcs,&key,&func);
//     //��ӡ���
//     showFunc(func);
//   }
//   //ɾ������������ͱ�
//   del_functbl(&funcTbl);
//   delTypeTbl(&typeTbl);
//   return 1;
// }




// //����val_tbl�Ĳ��Һ����ӹ���
// //�Լ�����val_tbl���غ��������б���
// int main(){
//   TypeTbl typeTbl=getGlobalTypeTbl();
//   FILE* fin=fopen("../out/type.txt","r");
//   loadFile_typeTbl(&typeTbl,fin);
//   fclose(fin);
//   ValTbl valTbl=getValTbl(typeTbl); //�����ͱ���valTbl��
//   fin=fopen("../out/global.txt","r");
//   loadFile_valtbl(&valTbl,fin);
//   fclose(fin);
//   ValTbl* partialValTbl=&valTbl;
//   FuncTbl funcTbl=getFuncTbl(&typeTbl);
//   fin=fopen("../out/func_head.txt","r");
//   loadFile_functbl(&funcTbl,fin);
//   fclose(fin);
//   do{
//     int ord[2000];
//     int tmp[100];
//     printf("\n************************************************\n\n");
//     myfgets(ord,"\n",stdin);
    
//     //����exit()�˳�
//     if(strcmp(ord,"exit()")==0) break;
//     //����select ��ѯ
//     mysgets(tmp," ",ord);
//     if(strcmp(tmp,"check")==0){
//       //������
//       Val val;
//       Type type;
//       char* name=(char*)ord+strlen((const char*)tmp)+1;
//       int retLayer;
//       if(findVal(partialValTbl,name,&val,&type,&retLayer)){
//         show_val(&val);
//         printf("typeLayer:%d\n",retLayer);
//         showType(&type);
//       }
//       else printf("not found!\n");
//     }
//     //����load���غ����б�
//     else if(strcmp(tmp,"load")==0){
//       char* tmp2=(char*)ord+strlen((const char*)tmp)+1;
//       char funcName[200];
//       char end=mysgets(funcName," ",tmp2);
//       char* owner="";
//       if(end!='\0')
//         owner=(char*)tmp2+strlen(funcName)+1;
//       Func* func;
//       if(strlen(owner)==0)
//         func=findFunc(&funcTbl,funcName,NULL);
//       else 
//         func=findFunc(&funcTbl,funcName,owner);
//       if(func==NULL) printf("not found\n");
//       else showFunc(func);
//       //��չ�ֲ���
//       if(func!=NULL){
//         loadArgs_valtbl(partialValTbl,&funcTbl,func);
//         printf("load sucess!\n");
//       }
//     }
//     //����extend��չ�µľֲ���
//     else if(strcmp(tmp,"extend")==0){
//       partialValTbl=extendValTbl(partialValTbl);
//     }
//     //����recycle�ͷ��µľֲ���
//     else if(strcmp(tmp,"recycle")==0){
//       if(partialValTbl==&valTbl){
//         printf("fail!");
//       }else{
//         partialValTbl=recycleValTbl(partialValTbl);
//       }
//     }
//   }while(1);
//   //�ͷſռ�
//   del_functbl(&funcTbl);
//   ValTbl* track=valTbl.next;
//   while(track!=NULL){
//     valTbl.next=track->next;
//     del_valTbl(track);
//     free(track);
//     track=valTbl.next;
//   }
//   del_valTbl(&valTbl);
//   return 0;
// }

int main(){
  char* str="int ( * ( *   a ) (int) )  (int(*)(int,int),int   )";
  char tmpStr[1000];
  strcpy(tmpStr,str);
  //����str,������ȡ�͸�ʽ��
  if(!formatTypeName(tmpStr)){
    printf("err\n");
  }
  printf("%s$\n",tmpStr);
  //��ȡ����
  char name[200];
  extractFuncPointerFieldName(str,name);
  printf("%s$\n",name);

  return 0;
}