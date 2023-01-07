#ifndef _ID_STRING_H
#define _ID_STRING_H
#include <stdio.h>
#include <stdlib.h>
#include <mem.h>

#define ID_STRING_NUM 200

//��������ϵ��
#define ID_ALLOCATOR_SHINK_COE  (2/10.0)
#define ID_ALLOCATOR_EXPAND_COE (8/10.0)



//�ڲ�ʹ�ö�̬����ʵ�ֵķ�����
typedef struct IdAllocator{
  char** val;
  int* useTimes;
  int alcSt;  //�������
  int size; //����Ԫ�ص�����
  int* toReuse;
  int toReuseNum; //�����յ�Ԫ�ص�����;
  int toReuseSize;
}IdAl,*IdAlp;

//�����������һ��id�����ô���Ϊ0����û�з���?���ݶ�Ӧ��char* val��
//�����û�з���Ķ�Ӧ��valΪNULL


//�м���������
//�м������Ҫ����Ϣ��,���ô���,�ɷ�����
//�ɷ����ţ�����˳��ӷ�����㿪ʼ����,����
//�������˳�����������,�ѻ�������Ҳ���Է���
//׼��һ��˳���,������պ���
//׼��һ����ϣ��,�����м�������Ӧ�ַ���

//��ȡһ��id������
struct IdAllocator getIdAllocator();


//���ַ���str����һ��id,�������ɹ�,���ط����id(һ���Ǹ���)
int allocateId(IdAlp idAllocator,char* str);

//����id�����ô���,�����ǰ����ַ�����id�����������ô���,������ַ����Ѿ�����id,���ܹ��������ô���
//�����������ô������ط�0ֵ,���򷵻�0
int increaseIdUseTimes(IdAlp idAllocator,int id,int increaseTimes);

//����һ������id�����ô���,�������ٷ��ط�0ֵ,�쳣�������0
int dropIdUseTimes(IdAlp idAllocator,int id,int dropTimes);


//�ض�λһ���ѷ���δ���յ�id���µ��ַ���
int resetIdString(IdAlp idAllocator,int id,char* newStr);

//��ȡid��Ӧ���ַ���,��ȡ�ɹ����ط���ռ��str,��ȡʧ�ܷ���NULL
char* getIdString(IdAlp idAllocator,int id);

//ǿ�ƻ���һ��id
int releaseId(IdAlp idAllocator,int id);

//���id��ĳ���ַ����İ�,���ǲ����̻���,���ǻ�ȵ����ü���Ϊ0ʱ�Ż��ա�����������ط�0ֵ,����쳣����0
int delString(IdAlp IdAllocator,int id);

//�ռ���չ������С���
int spaceAllocateForIdAllocator(IdAlp idAllocator);


//����id������
int freeIdAllocator(IdAlp idAllocator);


#endif