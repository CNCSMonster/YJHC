#ifndef _FUNC_RENAME_H
#define _FUNC_RENAME_H

#include <stdio.h>
#include "../yjhc_preProcess/mystring.h"

//����һ��������������

//׼����������Ҫ�Ĳ���,����������ǰ׺
#define _FUNC_RENAME_PRE "_yjhc_"

//ʹ��ǰ׺����������O�ķ���F������Ϊ_RUNC_RENAME_PRE+O+"_"+F

// //��ȡ��̬����ռ��������,�������п��ܴ��ڵĿո���_���
char* rename_member_method(char* funcName,char* ownerDefaultName);


#endif