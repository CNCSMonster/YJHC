#ifndef _FUNC_RENAME_H
#define _FUNC_RENAME_H
//׼����������Ҫ�Ĳ���,����������ǰ׺
#define YJHC_PREFIX_FUNC_RENAME "_yjhc_"

#include <stdio.h>
#include "../yjhc_preProcess/mystring.h"

//����һ��������������



//ʹ��ǰ׺����������O�ķ���F������Ϊ_RUNC_RENAME_PRE+O+"_"+F

// //��ȡ��̬����ռ��������,�������п��ܴ��ڵĿո���_���
char* yjhc_rename_member_method(char* funcName,char* ownerDefaultName);


#endif