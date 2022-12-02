#ifndef _SYNTAX_SYMBOL_H
#define _SYNTAX_SYMBOL_H

//�������,����Ƕ�����һ�����ǰ,����һ�����������init
enum syntax_symbol
{
  /*
  һЩ����.
  �����������:if��,for��,while��,do���ǻ����κ�ʱ�򶼿��Խ����,����Ҫ�ÿ�����
  ��������:����else if���else�飬ֻ��ǰ���������if����else if�ṹ��ʱ����ܹ�����
  */
  INIT,     //��û�ж�ȡ���뺯����״̬,Ψһ���ܵ�token����left brace,����left_braceѹ��,Ȼ����,������������򱨴�
  BLOCK_IN, //���������condition�����־��ѹ��ջ��,�����ֺŻ���,����ͬ��ͬ������,�����һ������򵯳�
  // if-elif-if�ṹ
  IF_CONDITION_IN,   //�κ�ʱ������iftokenѹ��÷���,������,������Բ����ѹ��ջ��,�����������ֺ�,�����������򵯳�,ѹ��if_block_in
  LEFT_PARENTHESIS,  //���С������ջ��,˵����ĳ��condition��,���������Բ����ѹ��ջ��,������Բ���Ű���Բ���ŵ��������������ֺ�
  IF_BLOCK_IN,       //�����ֺŻ���,���ԶԿ����condition�����־��Ӧ,�����һ����ŵ���,ѹ��if_block_out,����
  IF_BLOCK_OUT,      //����else����,ѹ��else_condition_in,����else if����,,ѹ��else if condition in,�������������򵯳�if_block_out����֮ǰջ��Ԫ�����ж�
  ELIF_CONDITION_IN, //������Բ����ѹ��,���������ŵ���������,ѹ��elif_block_in,�����,�����ֺܷ�,�������д�ӡ
  ELIF_BLOCK_IN,     //���������������,�����ֺŻ���,���������ŵ���,����elif_block_out
  ELIF_BLOCK_OUT,    //����else/else if token�������,���������ṹ,����elif_block_out,��������ջ�����ж�
  ELSE_BLOCK_IN,     //���������������,�����ֺŻ���,�����һ����ŵ���,
  ELSE_BLOCK_OUT,    //����else if/else�ṹ�����ǿɽ��뻻�У����������򵯳���ʹ����ջ�����ż����ж�
  // do-while�ṹ,
  DO_PRE,          //���������ŵ���,ѹ��do_block_in,���ܽ��������κ�token
  DO_BLOCK_IN,     //�����ֺŻ���,�ܹ�������������,���������Ž���,����DO-block-in�������У��������DO_BLOCK_OUT
  DO_BLOCK_OUT,    //����while������,����DO_CONDITION_IN
  DO_CONDITION_IN, //����������ѹ��,�����ֺŻ��У�����
  // while�ṹ
  WHILE_CONDITION_IN, //����while�ؼ���ѹ��,����������ѹ��,��߻����ŵ���,ѹ��while_block_in
  WHILE_BLOCK_IN,     //������������,�����ֺŻ���,���������Ž���,����WHILE_BLOCK_IN,����,����,��ѹ��
  // for�ṹ
  FOR_PRE,          //����for��ѹ��FOR_PRE,for_preջ������������,����,ѹ��for_init_in
  FOR_INIT_IN,      //�����ֺŲ�����,����,ѹ��for_condition_in,
  FOR_CONDITION_IN, //����������ѹ��,���������ŵ���,�����ֺŲ�����,����,ѹ��FOR_REFRESH_IN
  FOR_REFRESH_IN,   //����������ѹ��,���������ŵ���,ѹ��FOR_REFRESH_OUT,
  FOR_REFRESH_OUT,  //���������ŵ���,ѹ��FOR_BODY_IN,����
  FOR_BODY_IN,      //������������,�����ֺŻ���,�����һ����ŵ���,����,����
  sentence_kind_num //�����һ��enum����¼��Щenum������
};




#endif