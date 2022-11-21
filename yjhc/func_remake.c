#include <stdlib.h>
#include <stdio.h>



#define FUNC_TABLE_ARR_SIZE 200

//�����ṹ��
struct func{
  //������������ĳ���ṹ�����͵ķ����Ļ�,��ownerΪ����ṹ�����͵�����
  //������������ȫ�ֺ����Ļ�,��ownerΪNULL
  char* owner; 
  char* returnType; //����ֵ����,����ǻ����������͵Ļ�,������������ܹ��ٵ��÷���
};

//Ϊ��ʵ�ּ�,���������������ʽʵ�ֹ�ϣ��

//���غ�����,�������ٲ��Һ���,Ϊ�˿��ٲ���
//����һ����ϣ�����汣��ĳ�Ա�Ƕ�̬����ռ�ĺ����ṹ���ָ��
//����ʹ�ù�ϣ��ķ�ʽ�������������,Ҳ����ʹ�÷ǹ�ϣ��ķ�ʽ����
//Ҳ�����������Ե�����ϣ����߶�̬������
struct func_table
{
  //���������ж�ĳ�������Ƿ�����Լ��ж����������λ��
  struct func* funcs[FUNC_TABLE_ARR_SIZE];
  struct func_table* next;  //���nextΪnull��ʱ���˵����һ�ռ�û��
  /* data */
};

//�������溯���Ķ�̬����
struct func_arr
{
  struct func_table;
  int num;  //���涯̬������Ԫ�ص�����
  /* data */
};



//��ʼ������
void initFunc(struct func* func);
//��ʼ��������
void initFuncTable(struct func_table* funcTable);
//������̬�����ʼ��
void initFuncArr(struct func_arr* func);
//�Ӻ�����ϣ����ȡ��Ա,��������ȡ��Ա
struct func* getFromFuncTable(struct func_table* funcTable,char* name,int* ifSuccess);
//����ϣ���м����Ա,�������ظ������Ա
struct func* putToFuncTable(struct func_table* funcTable,struct func* toAddFunc,int* ifSuccess);
//�Ӻ�����̬������ȡ��Ա,�����±�ȡ
struct func* getFromFuncArr(struct func_arr* funcArr,int index,int* ifSuccess);
//��������ĩβ����һ����Ա
struct func* appendFuncArr(struct func_arr*,struct func* toAppendFunc);
//����ɢ���õĹ�ϣֵ
int hashCode(char* funcName,char* funcOwner);
//���ļ������һ�����͵���Ϣ
int fputFunc(struct func* toPutFunc,FILE* fout);

//��Ҫ���̺���
//��ǰ�沽����ȡ�ĺ����ļ��м��غ�����Ϣ����ϣ����,�������������Ƿ�ṹ�巽���Լ�����ֵ����
int loadFunc(struct func_table* funcTable,struct func_arr* funcArr);


int main(){
  
  
  return 0;
}