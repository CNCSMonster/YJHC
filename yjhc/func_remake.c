#include <stdlib.h>
#include <stdio.h>



#define FUNC_TABLE_ARR_SIZE 200

//函数结构体
struct func{
  //如果这个函数是某个结构体类型的方法的话,则owner为这个结构体类型的名称
  //如果这个函数是全局函数的话,则owner为NULL
  char* owner; 
  char* returnType; //返回值类型,如果是基础数据类型的话,则这个函数不能够再调用方法
};

//为了实现简单,采用数组链表的形式实现哈希表

//加载函数表,用来快速查找函数,为了快速查填
//这是一个哈希表，里面保存的成员是动态分配空间的函数结构体的指针
//可以使用哈希表的方式加载这个函数表,也可以使用非哈希表的方式加载
//也就是这个表可以当作哈希表或者动态数组用
struct func_table
{
  //用来快速判断某个函数是否存在以及判断这个函数的位置
  struct func* funcs[FUNC_TABLE_ARR_SIZE];
  struct func_table* next;  //如果next为null的时候就说明下一空间没有
  /* data */
};

//用来保存函数的动态数组
struct func_arr
{
  struct func_table;
  int num;  //保存动态数组中元素的数量
  /* data */
};



//初始化函数
void initFunc(struct func* func);
//初始化函数表
void initFuncTable(struct func_table* funcTable);
//函数动态数组初始化
void initFuncArr(struct func_arr* func);
//从函数哈希表中取成员,根据名字取成员
struct func* getFromFuncTable(struct func_table* funcTable,char* name,int* ifSuccess);
//往哈希表中加入成员,不允许重复加入成员
struct func* putToFuncTable(struct func_table* funcTable,struct func* toAddFunc,int* ifSuccess);
//从函数动态数组中取成员,根据下标取
struct func* getFromFuncArr(struct func_arr* funcArr,int index,int* ifSuccess);
//往函数中末尾加入一个成员
struct func* appendFuncArr(struct func_arr*,struct func* toAppendFunc);
//计算散列用的哈希值
int hashCode(char* funcName,char* funcOwner);
//往文件中输出一个类型的信息
int fputFunc(struct func* toPutFunc,FILE* fout);

//主要过程函数
//从前面步骤提取的函数文件中加载函数信息到哈希表中,包括函数名和是否结构体方法以及返回值类型
int loadFunc(struct func_table* funcTable,struct func_arr* funcArr);


int main(){
  
  
  return 0;
}