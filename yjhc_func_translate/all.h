#ifndef _ALL_H
#define _ALL_H


#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

//声明translate要用到的各个文件的预编译指令
#include "token_reader.c"
#include "token_reader.h"

#include "../yjhc_preProcess/token.h"
#include "../yjhc_preProcess/token.c"

#include "../yjhc_preProcess/mystring.h"
#include "../yjhc_preProcess/mystring.c"

#include "stluse.h"
#include "..\\mystl\\string_index.c"
#include "..\\mystl\\str_set.c"
#include "..\\mystl\\vector.c"
#include "..\\mystl\\hashtable.c"
#include "..\\mystl\\bitmap.c"

#include "val_tbl.h"
#include "val_tbl.c"

#include "func.h"
#include "func.c"

#include "func_tbl.h"
#include "func_tbl.c"

#include "type.h"
#include "type.c"

#include "func_translate.h"
#include "func_translate.c"

#include "action.h"

#include "func_rename.h"
#include "func_rename.c"


#endif