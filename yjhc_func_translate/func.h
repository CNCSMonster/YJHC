#ifndef _FUNC_H
#define _FUNC_H


#include "stluse.h"
#include "type.h"


//��ʽ������Ϣ
typedef struct struct_func_arg{
  char* name; //�β���
  int isConst;  //�ж��Ƿ��ǳ���
  long long typeId; //�β�������Ϣ����
}Arg;


//��¼��������Ϣ,�����β�
//һ����������Ϣȷ�����ǲ�����ĵ�
//���Һ����Ĳ�������ʹ�õ���Ϣֻ������ȫ����Ϣ
//���Ա�Ҳ��ȷ����
typedef struct struct_func{
  long long retTypeId;  //����������Ϣ�ı���
  char* func_name;  //������
  char* owner;  //������������
  //����������Ӧ�úܶ�,������ʹ�ø���̬���鱣��
  vector args;  //Arg���б�
}Func;

//չʾһ������
void showFunc(Func* func);

void del_func(Func* func);


//��ø���̬����ռ���ַ���,��Ϊָ�����ļ�,
//����Ĳ����ֱ�Ϊ�������Լ������������͵�typeId
//����ú�������ĳ�����͵ķ���,����������Ϊunknown����
char* getFuncKey(char* funcName,long long typeId);

//����funcKey��ȡfunc�����֣�������,�ɹ����ط�0ֵ��ʧ�ܷ���0
int extractFuncNameAndOwnerFromKey(char* funcKey,char* funcName,long long* retOwnerId);



#endif


