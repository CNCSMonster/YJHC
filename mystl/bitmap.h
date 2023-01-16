#ifndef _BITMAP_H
#define _BITMAP_H

#include <string.h>

//׼��λͼ�Ĵ�С

typedef struct struct_bitmap{
  char* map;  //���ݱ�����
}BitMap;

//λͼ����,�ù��ߴ�����Ͳ��ܹ�����
typedef struct struct_bitmap_util{
  const int mapSize;  //λͼ�Ĵ�С
}BitMapUtil;

//������λͼ
BitMap getBitMap(BitMapUtil* bmUtil);

//����λͼ���кϲ�����,�յ�λͼ�����ݺϲ������λͼ��
void merge_bitmap(BitMapUtil* bmUtil,BitMap* target,BitMap* toMerge);

//����һ��λͼ
BitMap copy_bitmap(BitMapUtil* bmUtil,BitMap* bm);

//����λͼ���н�����
void inter_bitmap(BitMapUtil* bmUtil,BitMap* target,BitMap* toInter);

//�ж�ĳ��λ���ڲ���λͼ��,����ڷ��ط�0ֵ,������ڷ���0
int in_bitmap(BitMapUtil* bmUtil,BitMap* bm,int index);

//��λͼ��ȡ��λ����һ��������
void get_bitmap(BitMapUtil* bmUtil,BitMap* bm,int* retIndexs,int* retSize);

//��λͼ�м���λ��
void put_bitmap(BitMapUtil* tmUtil,BitMap* bm,int index);

//���λͼ���Ѿ������λ��
void clear_bitmap(BitMapUtil* bmu,BitMap* bm);


//�ͷ�λͼ�ռ�
void delBitMap(BitMap* bm);

//�Է���c���Ը�ʽ�Ķ����ƴ�ӡ���
void showInBinary_bitmap(BitMapUtil* bmu,BitMap* bm);




#endif