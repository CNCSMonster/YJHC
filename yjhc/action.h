#ifndef _ACTION_H
#define _ACTION_H

//�ж��Ƿ���Ҫ�ݹ�
enum RecurseAction{

};

//�ж��Ƿ���Ҫ�׳��쳣,Ҳ�����Ƿ���δ���岿��
enum ErrAction{

};

//����������ֿ���,
enum BlockAction{
  MOVEINTO, //�����µĿ�
  MOVEOUT,  //�뿪��
  STAY  //�����ڿ���
};

//�Ƿ��е����ֿ���
enum PrintAction{
  NOT_DEFINE_PRINTACTION,   //û�ж���Ĵ�ӡ����,������ʾ�﷨�������쳣���
  NEWLINE,  //����
  KEEPLINE  //������
};

//����������������
enum FuncTranslateAction
{
  FUNC_TS_NEED,   //��Ҫ���뺯��
  FUNC_TS_NOTNEED //����Ҫ���뺯��
};


#endif