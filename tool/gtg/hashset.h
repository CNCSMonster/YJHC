#ifndef _HASHSET_H
#define _HASHSET_H

#include <mem.h>
#include <stdio.h>
#include <stdlib.h>

//�ù�ϣ���ܹ�������ֵ


//������������ķ�ʽʵ�ּ򵥹�ϣ
#define HASHSET_DEFAULT_ARRLEN 200
//ÿ���ڵ������С
#define HASHSET_NODE_DEFAULT_MAXSIZE sizeof(int)

typedef struct hashset_node{
    char val[HASHSET_NODE_DEFAULT_MAXSIZE];
    struct hashset_node* next;
}HashNode,*HashNodep;

typedef struct hashset{
    HashNodep nodes[HASHSET_DEFAULT_ARRLEN];    //��̬��������ռ�
    int valSize;
    int num;    //��¼��ӵ�Ԫ�ص�����
}HSet,*HSetp;

//����һ��������С�Ĺ�ϣ����
HSet hashset_cre(int valSize);


//����ϣ���м���Ԫ��
int hashset_add(HSetp hashset,void* toAdd);


//�ڹ�ϣ���в���һ��Ԫ��,������ڷ��ط�0ֵ����������ڣ�����0
int hashset_contains(HSetp hashset,void* toFind);

//�ӹ�ϣ����ɾȥһ��Ԫ��,ɾ���ɹ�����-1��ɾ��ʧ�ܷ���0
int hashset_remove(HSetp hashset,void* toDel);

//���ٹ�ϣ��
void free_hashset(HSetp hashset);

//ʹ�õĹ�ϣ����,����ֵʹ�õĹ�ϣ������ʹ�õ��������ֵ��ǰ��4���ֽڣ�
//�����С��int��С������ʹ�ù�ϣ����Ҫ�޸�hash�������������Խ��
int hash(void* value);


//��ȡ����hash�е��ַ�����һ���ɷ��ʴ�,����һ�������ռ�
void* hashset_toArr(HSetp hashset);


#endif