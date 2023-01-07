#ifndef _HASHTABLE_H
#define _HASHTABLE_H

#include <string.h>
#include <stdlib.h>



//��ϣ��ڵ�
typedef struct struct_hashtable_node{
    struct{
        void* key;  
        void* val;  
    }keyVal;        //���ü�ֵ��
    struct struct_hashtable_node* next;
}hashtable_node;


//�����ϣ��ṹ��
typedef struct struct_hashtable{
    //����ʹ���˿���չ�Ķ�̬������Ϊ��ʼ
    hashtable_node** nodes;    //�ڵ�����
    int cap;    //��ʼ����,���ܹ���С��1������
    int keySize;    //keyԪ�صĴ�С
    int valSize;    //valԪ�صĴ�С
    int size;   //��¼�����Ԫ�ص�����
    int (*keyEq)(const void*,const void*);  //��������func�ĺ���ƥ���ָ��
    int (*hash)(const void*);       //����ָ��,keyʹ�õ��Ĺ�ϣ
}hashtbl;   

hashtbl getHashTbl(int cap,int keySize,int valSize,int (*hash)(const void*),int (*keyEq)(const void*,const void*));


//����ԭ�������ڶ�Ӧ��key,val�Բ��ܹ�put�ɹ�
//�ɹ����ط�0ֵ��ʧ�ܷ���0
int hashtbl_put(hashtbl* htbl,void* key,void* val);

//�滻
int hashtbl_replace(hashtbl* htbl,void* key,void* newVal);

//��ȡkey��Ӧ��ֵ,д��valRetָ��ָ���ַ
//��ȡ�ɹ����ط�0ֵ��ʧ�ܷ���0
int hashtbl_get(hashtbl* htbl,void* key,void* valRet);

//ɾ����Ӧ�ļ�,ɾ���ɹ����ط�0ֵ,ɾ��ʧ�ܷ���0
int hashtbl_del(hashtbl* htbl,void* key);


//key,val������
void* hashtbl_toArr(hashtbl* htbl,void* keys,void* vals);


//��չ�ϣ��������Ԫ��
void hashtbl_clear(hashtbl* htbl);

//�ͷŹ�ϣ��ռ䣬��������������ϣ��Ӧ����ʹ��
void hashtbl_release(hashtbl* htbl);

#endif