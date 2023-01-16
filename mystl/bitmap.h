#ifndef _BITMAP_H
#define _BITMAP_H

#include <string.h>

//准备位图的大小

typedef struct struct_bitmap{
  char* map;  //数据保存者
}BitMap;

//位图工具,该工具创建后就不能够更改
typedef struct struct_bitmap_util{
  const int mapSize;  //位图的大小
}BitMapUtil;

//创建空位图
BitMap getBitMap(BitMapUtil* bmUtil);

//两个位图进行合并运算,终点位图的内容合并到起点位图中
void merge_bitmap(BitMapUtil* bmUtil,BitMap* target,BitMap* toMerge);

//复制一个位图
BitMap copy_bitmap(BitMapUtil* bmUtil,BitMap* bm);

//两个位图进行交运算
void inter_bitmap(BitMapUtil* bmUtil,BitMap* target,BitMap* toInter);

//判断某个位数在不在位图中,如果在返回非0值,如果不在返回0
int in_bitmap(BitMapUtil* bmUtil,BitMap* bm,int index);

//从位图中取出位数到一个数组中
void get_bitmap(BitMapUtil* bmUtil,BitMap* bm,int* retIndexs,int* retSize);

//往位图中加入位数
void put_bitmap(BitMapUtil* tmUtil,BitMap* bm,int index);

//清空位图中已经加入的位置
void clear_bitmap(BitMapUtil* bmu,BitMap* bm);


//释放位图空间
void delBitMap(BitMap* bm);

//以符合c语言格式的二进制打印结果
void showInBinary_bitmap(BitMapUtil* bmu,BitMap* bm);




#endif