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
//   loadFromFile_valtbl(&valTbl,fin);
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

//测试func_tbl的查找功能
int main(){
  //首先加载类型表
  


  return 0;
}


//测试token_reader的分段读功能

