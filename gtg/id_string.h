#ifndef _ID_STRING_H
#define _ID_STRING_H
#include <stdio.h>
#include <stdlib.h>
#include <mem.h>

#define ID_STRING_NUM 200

//放缩界限系数
#define ID_ALLOCATOR_SHINK_COE  (2/10.0)
#define ID_ALLOCATOR_EXPAND_COE (8/10.0)



//内部使用动态数组实现的分配器
typedef struct IdAllocator{
  char** val;
  int* useTimes;
  int alcSt;  //分配起点
  int size; //数组元素的数量
  int* toReuse;
  int toReuseNum; //待回收的元素的数量;
  int toReuseSize;
}IdAl,*IdAlp;

//关于如何区分一个id是引用次数为0还是没有分配?根据对应的char* val，
//如果是没有分配的对应的val为NULL


//中间号码分配器
//中间号码需要的信息有,引用次数,可分配编号
//可分配编号，按照顺序从分配起点开始分配,除了
//分配起点顺序往后的内容,已回收内容也可以分配
//准备一个顺序表,保存回收号码
//准备一个哈希表,保存中间号码与对应字符串

//获取一个id分配器
struct IdAllocator getIdAllocator();


//给字符串str分配一个id,如果分配成功,返回分配的id(一个非负数)
int allocateId(IdAlp idAllocator,char* str);

//增加id的引用次数,必须是绑定了字符串的id才能增加引用次数,如果是字符串已经解绑的id,不能够增加引用次数
//正常增加引用次数返回非0值,否则返回0
int increaseIdUseTimes(IdAlp idAllocator,int id,int increaseTimes);

//减少一个可用id的引用次数,正常减少返回非0值,异常情况返回0
int dropIdUseTimes(IdAlp idAllocator,int id,int dropTimes);


//重定位一个已分配未回收的id到新的字符串
int resetIdString(IdAlp idAllocator,int id,char* newStr);

//获取id对应的字符串,获取成功返回分配空间的str,获取失败返回NULL
char* getIdString(IdAlp idAllocator,int id);

//强制回收一个id
int releaseId(IdAlp idAllocator,int id);

//解除id与某个字符串的绑定,但是不立刻回收,而是会等到引用计数为0时才回收。正常解除返回非0值,解除异常返回0
int delString(IdAlp IdAllocator,int id);

//空间扩展或者缩小检查
int spaceAllocateForIdAllocator(IdAlp idAllocator);


//回收id分配器
int freeIdAllocator(IdAlp idAllocator);


#endif