#ifndef _FUNC_H
#define _FUNC_H

//形式参数信息
typedef struct struct_func_arg{
  char* name; //形参名
  long long typeId; //形参类型信息编码
}Arg;


//记录函数的信息,比如形参
//一个函数的信息确定后是不会更改的
//而且函数的参数里面使用的信息只能允许全局信息
//所以表也是确定的
typedef struct struct_func{
  long long retType;  //返回类型信息的编码
  
}Func;













#endif