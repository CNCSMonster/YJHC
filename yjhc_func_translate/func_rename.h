#ifndef _FUNC_RENAME_H
#define _FUNC_RENAME_H
//准备重命名需要的参数,函数重命名前缀
#define YJHC_PREFIX_FUNC_RENAME "_yjhc_"

#include <stdio.h>
#include "../yjhc_preProcess/mystring.h"

//进行一个函数的重命名



//使用前缀参数把主人O的方法F重命名为_RUNC_RENAME_PRE+O+"_"+F

// //获取动态分配空间的主人名,主人名中可能存在的空格用_填充
char* yjhc_rename_member_method(char* funcName,char* ownerDefaultName);


#endif