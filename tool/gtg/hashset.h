#ifndef _HASHSET_H
#define _HASHSET_H

#include <mem.h>
#include <stdio.h>
#include <stdlib.h>

//该哈希表能够用来存值


//采用链表数组的方式实现简单哈希
#define HASHSET_DEFAULT_ARRLEN 200
//每个节点的最大大小
#define HASHSET_NODE_DEFAULT_MAXSIZE sizeof(int)

typedef struct hashset_node{
    char val[HASHSET_NODE_DEFAULT_MAXSIZE];
    struct hashset_node* next;
}HashNode,*HashNodep;

typedef struct hashset{
    HashNodep nodes[HASHSET_DEFAULT_ARRLEN];    //动态分配基础空间
    int valSize;
    int num;    //记录添加的元素的数量
}HSet,*HSetp;

//创建一个基础大小的哈希数组
HSet hashset_cre(int valSize);


//往哈希表中加入元素
int hashset_add(HSetp hashset,void* toAdd);


//在哈希表中查找一个元素,如果存在返回非0值，如果不存在，返回0
int hashset_contains(HSetp hashset,void* toFind);

//从哈希表中删去一个元素,删除成功返回-1，删除失败返回0
int hashset_remove(HSetp hashset,void* toDel);

//销毁哈希表
void free_hashset(HSetp hashset);

//使用的哈希函数,根据值使用的哈希函数，使用的是数组的值的前面4个字节，
//如果对小于int大小的数据使用哈希，需要修改hash函数，避免访问越界
int hash(void* value);


//获取加入hash中的字符串的一个可访问串,返回一个连续空间
void* hashset_toArr(HSetp hashset);


#endif