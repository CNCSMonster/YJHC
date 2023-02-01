//用来测试各个模块的功能
#include "all.h"

//首先判断是否需要进行打印






// //测试val_tbl的查找功能
// int main(){
//   //首先创建val_tbl
//   TypeTbl typeTbl=getGlobalTypeTbl();
//   FILE* fin=fopen("..\\out\\func_type.txt","r");
//   loadFile_typeTbl(&typeTbl,fin);
//   fclose(fin);
//   ValTbl valTbl=getValTbl(typeTbl);
//   fin=fopen("..\\out\\global.txt","r");
//   loadFile_valtbl(&valTbl,fin);
//   fclose(fin);
//   //进行查找显示
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
//     //类型信息
//     showType(&type);
//   }while(1);
//   del_valTbl(&valTbl);
//   return 0;
// }

// //测试func_tbl的查找功能
// int main(){
//   //首先加载类型表
//   TypeTbl typeTbl=getGlobalTypeTbl();
//   FILE* fin=fopen("..\\out\\type.txt","r");
//   loadFile_typeTbl(&typeTbl,fin);
//   fclose(fin);
//   fin=fopen("..\\out\\func_head.txt","r");
//   FuncTbl funcTbl=getFuncTbl(&typeTbl);
//   loadFile_functbl(&funcTbl,fin);
//   fclose(fin);
//   //进行查询,输入名字和主人查询
//   char funcName[1000];
//   char ownerName[1000];
//   //查看所有类型
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


// // 测试token_reader的分段读功能
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




// //测试typetbl的查找功能和增加类型功能
// int main(){
//   //首先加载类型
//   //然后根据类型名查找类型,以及类型信息
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
//     //如果第一个字符是<,则说明是命令
//     if(tmp[0]=='<'){
//       if(!loadLine_typetbl(&typeTbl,tmp+1)){
//         printf("fail to execute %s",tmp+1);
//       }
//     }
//     //否则是类型名,查找类型名
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

//测试顺序读取type的名字

// //测试func_tbl的查找功能
// int main(){
//   //首先加载func_tbl
//   TypeTbl typeTbl=getGlobalTypeTbl(); //首先获取全局类型表
//   //然后从文件中加载类型表信息
//   FILE* fin=fopen("../out/type.txt","r");
//   loadFile_typeTbl(&typeTbl,fin);
//   fclose(fin);
//   FuncTbl funcTbl=getFuncTbl(&typeTbl);
//   //从文件中加载函数信息
//   fin=fopen("../out/func_head.txt","r");
//   loadFile_functbl(&funcTbl,fin);
//   fclose(fin);
//   //进行顺序展示func
//   for(int i=0;i<funcTbl.funcKeys.size;i++){
//     printf("\n\n****************************************\n\n");
//     char* key;
//     vector_get(&funcTbl.funcKeys,i,&key);
//     char funcName[300];
//     char ownerId[300];
//     mysgets(funcName,"#",key);
//     mysgets(ownerId,"",key+strlen(funcName)+1);
//     long long typeId=atoll(ownerId);
//     //获取主人的名字
//     printf("funcName:%s\n",funcName); 
//     printf("onwer:\n");
    
//     //展示onwer类型
//     if(typeId==0) printf("null\n");
//     else{
//       int typeIndex;
//       extractTypeIndexAndPointerLayer(typeId,&typeIndex,NULL);
//       //获取类型
//       Type type;
//       vector_get(&(funcTbl.globalTypeTbl->types),typeIndex,&type);
//       showType(&type);
//     }
//     //打印函数信息
//     Func* func;
//     hashtbl_get(&funcTbl.funcs,&key,&func);
//     //打印结果
//     showFunc(func);
//   }
//   //删除函数表和类型表
//   del_functbl(&funcTbl);
//   delTypeTbl(&typeTbl);
//   return 1;
// }




// //测试val_tbl的查找和增加功能
// //以及测试val_tbl加载函数参数列表功能
// int main(){
//   TypeTbl typeTbl=getGlobalTypeTbl();
//   FILE* fin=fopen("../out/type.txt","r");
//   loadFile_typeTbl(&typeTbl,fin);
//   fclose(fin);
//   ValTbl valTbl=getValTbl(typeTbl); //把类型表交给valTbl了
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
    
//     //输入exit()退出
//     if(strcmp(ord,"exit()")==0) break;
//     //输入select 查询
//     mysgets(tmp," ",ord);
//     if(strcmp(tmp,"check")==0){
//       //查找量
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
//     //输入load加载函数列表
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
//       //扩展局部表
//       if(func!=NULL){
//         loadArgs_valtbl(partialValTbl,&funcTbl,func);
//         printf("load sucess!\n");
//       }
//     }
//     //输入extend扩展新的局部表
//     else if(strcmp(tmp,"extend")==0){
//       partialValTbl=extendValTbl(partialValTbl);
//     }
//     //输入recycle释放新的局部表
//     else if(strcmp(tmp,"recycle")==0){
//       if(partialValTbl==&valTbl){
//         printf("fail!");
//       }else{
//         partialValTbl=recycleValTbl(partialValTbl);
//       }
//     }
//   }while(1);
//   //释放空间
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
  //对于str,进行提取和格式化
  if(!formatTypeName(tmpStr)){
    printf("err\n");
  }
  printf("%s$\n",tmpStr);
  //提取名字
  char name[200];
  extractFuncPointerFieldName(str,name);
  printf("%s$\n",name);

  return 0;
}