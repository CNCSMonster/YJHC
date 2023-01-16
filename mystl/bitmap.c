#include "bitmap.h"

//创建空位图
BitMap getBitMap(BitMapUtil* bmUtil){
  BitMap bm={
    .map=malloc(bmUtil->mapSize)
  };
  memset(bm.map,0,bmUtil->mapSize);
  return bm;
}

//两个位图进行合并运算,终点位图的内容合并到起点位图中
void merge_bitmap(BitMapUtil* bmUtil,BitMap* target,BitMap* toMerge){
  //每个位置进行或预算
  for(int i=0;i<bmUtil->mapSize;i++){
    target->map[i]|=toMerge->map[i];
  }
}

//复制一个位图
BitMap copy_bitmap(BitMapUtil* bmUtil,BitMap* bm){
  BitMap out={
    .map=malloc(bmUtil->mapSize)
  };
  memcpy(out.map,bm->map,bmUtil->mapSize);
  return out;
}

//两个位图进行交运算
void inter_bitmap(BitMapUtil* bmUtil,BitMap* target,BitMap* toInter){
  for(int i=0;i<bmUtil->mapSize;i++){
    target->map[i]&=toInter->map[i];
  }
}

//判断某个位数在不在位图中,如果在返回非0值,如果不在返回0
int in_bitmap(BitMapUtil* tmUtil,BitMap* bm,int index){
  //首先计算位置
  if(index/(sizeof(char)*8)>=tmUtil->mapSize){
    return 0;
  }
  //找到对应位置
  int off=index%(sizeof(char)*8);
  int i=index/(sizeof(char)*8);
  int base=sizeof(char)*8;
  char t=bm->map[i];
  t<<=(base-off-1);
  t>>=(base-1);
  return t==0?0:-1;
}

//从位图中取出位数到一个数组中
void get_bitmap(BitMapUtil* bmUtil,BitMap* bm,int* retIndexs,int* retSize){
  *retSize=0;
  for(int i=0;i<bmUtil->mapSize;i++){
    char byte=bm->map[i];
    for(int off=0;off<sizeof(char);off++){
      //判断这个位置有没有
      int base=sizeof(char)*8;
      int t=byte<<(base-off-1)>>(base-1);
      if(t){
        retIndexs[*retSize]=off+i*base;
        (*retSize)++;
      }
    }
  }
}

//往位图中加入位数
void put_bitmap(BitMapUtil* tmUtil,BitMap* bm,int index){
  //首先计算位置
  if(index/(sizeof(char)*8)>=tmUtil->mapSize){
    return ;
  }
  //找到对应位置
  int off=index%(sizeof(char)*8);
  //生成用来加入的数据
  char t=1<<off;
  bm->map[index/(sizeof(char)*8)]|=t;
}

//清空位图中已经加入的位置
void clear_bitmap(BitMapUtil* bmu,BitMap* bm){
  memset(bm->map,0,bmu->mapSize);
}


//释放位图空间
void delBitMap(BitMap* bm){
  free(bm->map);
}

//以符合c语言格式的二进制打印结果
void showInBinary_bitmap(BitMapUtil* bmu,BitMap* bm){
  printf("0b");
  for(int i=0;i<bmu->mapSize;i++){
    //判断这个位置是否存在
    int t=bm->map[i];
    //一个字节8比特,查找
    for(int i=0;i<7;i++){
      if(t%2==0) printf("0");
      else printf("1");
      t>>1;
    }
    if(t==0) printf("0");
    else printf("1");
  }
}


