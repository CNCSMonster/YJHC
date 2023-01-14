#ifndef _FUNC_TRANSLATE_H
#define _FUNC_TRANSLATE_H

#include "lib_use.h"
#include "syntax_symbol.h"
#include "func.h"
#include "type.h"
#include "val_tbl.h"
#include "token_reader.h"


//Ŀǰֻ�Ǵ����ļ�������,���ļ���yjhc�������ͨ����̬���ӵĴ����ٽ��з���

//һЩ�����õĲ���,
//�ò��������������ṹ�巽��
#define PREFIX_OF_NEW_FUNC "__yjhc_"
//�ò��������������ṹ�巽���е��õ���������
#define PREFIX_OF_NEW_VAL "this."


//����������,���溯����������Ҫ��һЩ����
typedef struct func_translator{
  //��ṹ
  TypeTbl* gloabalTypeTbl; //ȫ�����ͱ�
  ValTbl* valTbl;  //ȫ������
  ValTbl* partialValTbl;  //�ֲ�����
  FuncTbl* funcTbl; //ȫ�ֺ�����
  //ע��,token��ȡ��ʹ�õĵ���ģʽ
}FuncTranslator;

//��������������,�������ǰ�δ�����yjhc�ĺ�������tokenתΪc�ĺ�������token����
FuncTranslator getFuncTranslator(char* typePath,char* funcHeadPath,char* valPath);

//ʹ�ú�����������ʼ����
int func_translate(FuncTranslator* funcTranslator,char* tokenInPath,char* tokenOutPath);

//���뵥������,�ɹ�����nodes,ʧ�ܷ���NULL
TBNode* process_singleLine(FuncTranslator* funcTranslator,TBNode* nodes);

//��������ͷź���������
int release_funcTranslator(FuncTranslator* funcTranslator);


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
FTK getTokenLineKind(FuncTranslator* funcTranslator,TBNode* tokens);

//���빦���Ӵ���,����ɹ����ط�NULL,����ʧ�ܷ���NULL

//��������������
TBNode* translateVarDef(FuncTranslator* functranslator,TBNode* tokens);

//���볣���������
TBNode* translateConstDef(FuncTranslator* functranslator,TBNode* tokens);

//�����������
TBNode* translateCountDef(FuncTranslator* functranslator,TBNode* tokens);

//����ָ�붨�����
TBNode* translateFuncPointerDef(FuncTranslator* functranslator,TBNode* tokens);

//typedef�������ͱ������
TBNode* translateTypedef(FuncTranslator* functranslator,TBNode* tokens);

//���뺯���������
TBNode* translateFuncUse(FuncTranslator* functranslator,TBNode* tokens);

//���븳ֵ���
TBNode* translateAssign(FuncTranslator* functranslator,TBNode* tokens);

//�������ͷ����������
TBNode* translateTypeMethodUse(FuncTranslator* functranslator,TBNode* tokens);



//���뺯����ѡ���

TBNode* (*tranFuncs[]) (FuncTranslator*,TBNode*) = {
  [NOT_TRANSLATE_FTK] NULL,
  [VAR_DEFINE_FTK] translateVarDef,
  [CONST_DEFINE_FTK] translateConstDef,     //�����������
  [FUNC_USE_FTK] translateFuncUse,     //�����������
  [COUNT_FTK] translateCountDef,    //�������,�������������
  [TYPEDEF_FTK] translateTypedef,    //typedef��ͷ�����ͱ����������
  [FUNCPOINTER_DEF_FTK] translateFuncPointerDef,  //����ָ�����������
  [ASSIGN_FTK] translateAssign, //��ֵ���
  [TYPE_METHOD_USE_FTK] translateTypeMethodUse   //���ͷ����������
};



#endif