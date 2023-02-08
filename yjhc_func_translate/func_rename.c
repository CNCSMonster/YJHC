#include "func_rename.h"




// //获取动态分配空间的主人名,主人名中可能存在的空格用_填充
char* yjhc_rename_member_method(char* funcName,char* ownerDefaultName){
  if(funcName==NULL||ownerDefaultName==NULL) return NULL;
  int length=strlen(YJHC_PREFIX_FUNC_RENAME)+strlen(funcName)+strlen(ownerDefaultName)+2;
  char* out=malloc(length);
  sprintf(out,"%s%s_%s",YJHC_PREFIX_FUNC_RENAME,ownerDefaultName,funcName);
  mystrReplace(out,' ','_');
  return out;
}




