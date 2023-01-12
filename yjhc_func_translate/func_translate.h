#ifndef _FUNC_TRANSLATE_H
#define _FUNC_TRANSLATE_H


#include "str_set.h"
#include "string_index.h"
#include "syntax_symbol.h"
#include "func.h"
#include "type.h"
#include "val_tbl.h"
#include "token_reader.h"

//TODO

//����������,���溯����������Ҫ��һЩ����
typedef struct func_translator{
  //��ṹ
  TypeTbl* gloabalTypeTbl; //ȫ�����ͱ�
  ValTbl* ValTbl;  //ȫ������
  ValTbl* partialValTbl;  //�ֲ�����
  FuncTbl* funcTbl; //ȫ�ֺ�����
  FILE* fout;
  //ע��,token��ȡ��ʹ�õĵ���ģʽ
}FuncTranslator;

//��������������,�������ǰ�δ�����yjhc�ĺ�������tokenתΪc�ĺ�������token����
FuncTranslator getFuncTranslator(char* typePath,char* funcHeadPath,char* valPath,char* tokenPath,char* tokenOutPath);

//ʹ�ú�����������ʼ����
int func_translate(FuncTranslator* funcTranslator,char*);


//��������ͷź���������
int release_FuncTranslator(FuncTranslator* funcTranslator);


//�жϷ�������
typedef enum func_translate_kind{
  NOT_TRANSLATE_FTK,    //���÷�������
  VAR_DEFINE_FTK,      //�����������
  CONST_DEFINE_FTK,     //�����������
  FUNC_USE_FTK,     //�����������
  COUNT_FTK,    //�������,�������������
  TYPEDEF_FTK,    //typedef��ͷ�����ͱ����������
  FUNCPOINTER_DEF_FTK,  //����ָ�����������
  ASSIGN_FTK, //��ֵ���
  TYPE_METHOD_USE_FTK,  //���ͷ����������
  FTK_NUM   //token������
}FTK;

//�жϾ��ӵķ�������,��ѡ��ͬ�ķ������
FTK getTokenLineKind(TBNode* tokens);

//���빦���Ӵ���,����ɹ����ط�0ֵ,����ʧ�ܷ���0

//��������������
int translateVarDef(TBNode* tokens);

//���볣���������
int translateConstDef(TBNode* tokens);

//�����������
int translateCountDef(TBNode* tokens);

//����ָ�붨�����
int translateFuncPointerDef(TBNode* tokens);

//typedef�������ͱ������
int translateTypedef(TBNode* tokens);

//���뺯���������
int translateFuncUse(TBNode* tokens);

//���븳ֵ���
int translateAssign(TBNode* tokens);

//�������ͷ����������
int translateTypeMethodUse(TBNode* tokens);



//���뺯����ѡ���

int (*tranFuncs[]) (TBNode*) = {
  [NOT_TRANSLATE_FTK] NULL,
  [VAR_DEFINE_FTK] translateVarDef,
  [CONST_DEFINE_FTK] translateConstDef,     //�����������
  [FUNC_USE_FTK] translateFuncUse,     //�����������
  [COUNT_FTK] translateCountDef,    //�������,�������������
  [TYPEDEF_FTK] translateTypedef,    //typedef��ͷ�����ͱ����������
  [FUNCPOINTER_DEF_FTK] translateFuncPointerDef,  //����ָ�����������
  [ASSIGN_FTK] translateAssign, //��ֵ���
  [TYPE_FUNCTION_USE_FTK] translateTypeMethodUse   //���ͷ����������
};






#endif