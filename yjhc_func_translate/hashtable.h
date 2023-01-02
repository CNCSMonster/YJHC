#ifndef _HASHTABLE_H
#define _HASHTABLE_H

#include <string.h>
#include <stdlib.h>



//哈希表节点
typedef struct struct_hashtable_node{
    struct{
        void* key;  
        void* val;  
    }keyVal;        //设置键值对
    struct struct_hashtable_node* next;
}hashtable_node;


//定义哈希表结构体
typedef struct struct_hashtable{
    //里面使用了可扩展的动态数组作为表开始
    hashtable_node** nodes;    //节点数组
    int cap;    //初始容量,不能够是小于1的数字
    int keySize;    //key元素的大小
    int valSize;    //val元素的大小
    int size;   //记录保存的元素的数量
    int (*keyEq)(const void*,const void*);  //用来进行func的函数匹配的指针
    int (*hash)(const void*);       //函数指针,key使用到的哈希
}hashtbl;   

hashtbl getHashTbl(int cap,int keySize,int valSize,int (*hash)(const void*),int (*keyEq)(const void*,const void*));


//必须原本不存在对应的key,val对才能够put成功
//成功返回非0值，失败返回0
int hashtbl_put(hashtbl* htbl,void* key,void* val);

//替换
int hashtbl_replace(hashtbl* htbl,void* key,void* newVal);

//获取key对应的值,写入valRet指针指向地址
//获取成功返回非0值，失败返回0
int hashtbl_get(hashtbl* htbl,void* key,void* valRet);

//删除对应的键,删除成功返回非0值,删除失败返回0
int hashtbl_del(hashtbl* htbl,void* key);


//key,val对数组
void* hashtbl_toArr(hashtbl* htbl,void* keys,void* vals);


//清空哈希表中所有元素
void hashtbl_clear(hashtbl* htbl);

//释放哈希表空间，调用这个函数后哈希表不应该再使用
void hashtbl_release(hashtbl* htbl);

#endif