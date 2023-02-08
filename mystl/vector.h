#ifndef _VECTOR_H
#define _VECTOR_H
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define VECTOR_ARR_BASE_SIZE 100
//空间扩大参数,占有率超过这个参数就扩大
#define VECTOR_EXPAND_LIMIT 0.8
//空间缩小参数,占有率小于这个参数就缩小
#define VECTOR_SHRINK_LIMIT 0.3

//空间扩大系数
#define VECTOR_EXPAND_COE VECTOR_EXPAND_LIMIT*2

//空间缩小系数
#define VECTOR_SHRINK_COE VECTOR_SHRINK_LIMIT*2

typedef struct struct_vector_node{
  char* val;  //用来保存值
}vector_node;

typedef struct struct_vector
{
  int valSize; //记录vector的元素大小
  vector_node* vals;  //动态数组
  int size; //记录动态数组元素数量,注意,该数量不应该允许外部修改
  int capacity; //记录动态数组的元素容量,注意该数量也不应该允许外部修改
}vector;

//获得vector中元素
vector getVector(int size);

//往vector末尾压入元素
void vector_push_back(vector* vec,void* val);

//返回vector对应位置的内容,通过dir指针,如果下标正常,返回第index位置内容,如果下标异常返回0值
void vector_get(vector* vec,int index,void* dir);

//设置vector对应位置的内容,如果对应位置原本有内容,会通过old指针返回,删除失败返回NULL
void vector_set(vector* vec,int index,void* val,void* old);


//删除元素,通过del指针返回删去的内容,如果删除失败,返回NULL
void vector_del(vector* vec,int index,void* del);

//对vector进行放缩检查
void vector_resize(vector* vec);
//清空元素
void vector_clear(vector* vec);

//把值转为二维数组返回,返回的数组里的值是实际值的浅拷贝
void* vector_toArr(vector* vec);

#endif


