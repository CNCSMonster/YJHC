#include "func_rename.h"




// //��ȡ��̬����ռ��������,�������п��ܴ��ڵĿո���_���
char* rename_member_method(char* funcName,char* ownerDefaultName){
  if(funcName==NULL||ownerDefaultName==NULL) return NULL;
  int length=strlen(_FUNC_RENAME_PRE)+strlen(funcName)+strlen(ownerDefaultName)+2;
  char* out=malloc(length);
  sprintf(out,"%s%s_%s",_FUNC_RENAME_PRE,ownerDefaultName,funcName);
  mystrReplace(out,' ','_');
  return out;
}




