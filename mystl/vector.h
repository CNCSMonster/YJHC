#ifndef _VECTOR_H
#define _VECTOR_H
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define VECTOR_ARR_BASE_SIZE 100
//�ռ��������,ռ���ʳ����������������
#define VECTOR_EXPAND_LIMIT 0.8
//�ռ���С����,ռ����С�������������С
#define VECTOR_SHRINK_LIMIT 0.3

//�ռ�����ϵ��
#define VECTOR_EXPAND_COE VECTOR_EXPAND_LIMIT*2

//�ռ���Сϵ��
#define VECTOR_SHRINK_COE VECTOR_SHRINK_LIMIT*2

typedef struct struct_vector_node{
  char* val;  //��������ֵ
}vector_node;

typedef struct struct_vector
{
  int valSize; //��¼vector��Ԫ�ش�С
  vector_node* vals;  //��̬����
  int size; //��¼��̬����Ԫ������,ע��,��������Ӧ�������ⲿ�޸�
  int capacity; //��¼��̬�����Ԫ������,ע�������Ҳ��Ӧ�������ⲿ�޸�
}vector;

//���vector��Ԫ��
vector getVector(int size);

//��vectorĩβѹ��Ԫ��
void vector_push_back(vector* vec,void* val);

//����vector��Ӧλ�õ�����,ͨ��dirָ��,����±�����,���ص�indexλ������,����±��쳣����0ֵ
void vector_get(vector* vec,int index,void* dir);

//����vector��Ӧλ�õ�����,�����Ӧλ��ԭ��������,��ͨ��oldָ�뷵��,ɾ��ʧ�ܷ���NULL
void vector_set(vector* vec,int index,void* val,void* old);


//ɾ��Ԫ��,ͨ��delָ�뷵��ɾȥ������,���ɾ��ʧ��,����NULL
void vector_del(vector* vec,int index,void* del);

//��vector���з������
void vector_resize(vector* vec);
//���Ԫ��
void vector_clear(vector* vec);

//��ֵתΪ��ά���鷵��,���ص��������ֵ��ʵ��ֵ��ǳ����
void* vector_toArr(vector* vec);

#endif


