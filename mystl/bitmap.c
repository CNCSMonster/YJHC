#include "bitmap.h"

//������λͼ
BitMap getBitMap(BitMapUtil* bmUtil){
  BitMap bm={
    .map=malloc(bmUtil->mapSize)
  };
  memset(bm.map,0,bmUtil->mapSize);
  return bm;
}

//����λͼ���кϲ�����,�յ�λͼ�����ݺϲ������λͼ��
void merge_bitmap(BitMapUtil* bmUtil,BitMap* target,BitMap* toMerge){
  //ÿ��λ�ý��л�Ԥ��
  for(int i=0;i<bmUtil->mapSize;i++){
    target->map[i]|=toMerge->map[i];
  }
}

//����һ��λͼ
BitMap copy_bitmap(BitMapUtil* bmUtil,BitMap* bm){
  BitMap out={
    .map=malloc(bmUtil->mapSize)
  };
  memcpy(out.map,bm->map,bmUtil->mapSize);
  return out;
}

//����λͼ���н�����
void inter_bitmap(BitMapUtil* bmUtil,BitMap* target,BitMap* toInter){
  for(int i=0;i<bmUtil->mapSize;i++){
    target->map[i]&=toInter->map[i];
  }
}

//�ж�ĳ��λ���ڲ���λͼ��,����ڷ��ط�0ֵ,������ڷ���0
int in_bitmap(BitMapUtil* tmUtil,BitMap* bm,int index){
  //���ȼ���λ��
  if(index/(sizeof(char)*8)>=tmUtil->mapSize){
    return 0;
  }
  //�ҵ���Ӧλ��
  int off=index%(sizeof(char)*8);
  int i=index/(sizeof(char)*8);
  int base=sizeof(char)*8;
  char t=bm->map[i];
  t<<=(base-off-1);
  t>>=(base-1);
  return t==0?0:-1;
}

//��λͼ��ȡ��λ����һ��������
void get_bitmap(BitMapUtil* bmUtil,BitMap* bm,int* retIndexs,int* retSize){
  *retSize=0;
  for(int i=0;i<bmUtil->mapSize;i++){
    char byte=bm->map[i];
    for(int off=0;off<sizeof(char);off++){
      //�ж����λ����û��
      int base=sizeof(char)*8;
      int t=byte<<(base-off-1)>>(base-1);
      if(t){
        retIndexs[*retSize]=off+i*base;
        (*retSize)++;
      }
    }
  }
}

//��λͼ�м���λ��
void put_bitmap(BitMapUtil* tmUtil,BitMap* bm,int index){
  //���ȼ���λ��
  if(index/(sizeof(char)*8)>=tmUtil->mapSize){
    return ;
  }
  //�ҵ���Ӧλ��
  int off=index%(sizeof(char)*8);
  //�����������������
  char t=1<<off;
  bm->map[index/(sizeof(char)*8)]|=t;
}

//���λͼ���Ѿ������λ��
void clear_bitmap(BitMapUtil* bmu,BitMap* bm){
  memset(bm->map,0,bmu->mapSize);
}


//�ͷ�λͼ�ռ�
void delBitMap(BitMap* bm){
  free(bm->map);
}

//�Է���c���Ը�ʽ�Ķ����ƴ�ӡ���
void showInBinary_bitmap(BitMapUtil* bmu,BitMap* bm){
  printf("0b");
  for(int i=0;i<bmu->mapSize;i++){
    //�ж����λ���Ƿ����
    int t=bm->map[i];
    //һ���ֽ�8����,����
    for(int i=0;i<7;i++){
      if(t%2==0) printf("0");
      else printf("1");
      t>>1;
    }
    if(t==0) printf("0");
    else printf("1");
  }
}


