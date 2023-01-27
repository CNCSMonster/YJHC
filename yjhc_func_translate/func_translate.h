#ifndef _FUNC_TRANSLATE_H
#define _FUNC_TRANSLATE_H

#include "lib_use.h"
#include "syntax_symbol.h"
#include "func.h"
#include "type.h"
#include "val_tbl.h"
#include "token_reader.h"
#include "config.h"
#include "sentence_kind.h"



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
  ValTbl* globalValTbl;  //ȫ������
  ValTbl* partialValTbl;  //�ֲ�����
  FuncTbl* funcTbl; //ȫ�ֺ�����
  //�����쳣��Ϣ,����쳣��Ϣ��������,����������ʾ
  int judSentenceKindErr;
  //ע��,token��ȡ��ʹ�õĵ���ģʽ
  Func* curFunc;  //��ǰ���뵽�ĺ���
  FILE* warningFout;  //������Ϣ���·��,һ��Ĭ����stdout
}FuncTranslator;

//��������������,�������ǰ�δ�����yjhc�ĺ�������tokenתΪc�ĺ�������token����
FuncTranslator getFuncTranslator(char* typePath,char* funcHeadPath,char* valPath);

//���з���ǰ���
int pre_translate_check(FuncTranslator* translator);

//�����Ƿ������������,�����������,���ط�0ֵ,���û�У�����0
int check_main_function(FuncTranslator* translator);


//ʹ�ú�����������ʼ����
int func_translate(FuncTranslator* funcTranslator,char* tokenInPath,char* tokenOutPath);


//��ȫ��Ա�������������ĵ���
TBNode* member_use_complement(FuncTranslator* funcTranslator,TBNode* nodes);


//���뵥������,�ɹ�����nodes,ʧ�ܷ���NULL
TBNode* process_singleLine(FuncTranslator* funcTranslator,TBNode* nodes);

//��������ͷź���������
int release_funcTranslator(FuncTranslator* funcTranslator);


//�жϷ�������
typedef enum func_translate_kind{
  NOT_LEAGAL_FTK,   //�ж������﷨����ľ��Ӿͻ᷵��NOT_LEAGAL_FTK
  NOT_TRANSLATE_FTK,    //���÷�������
  VAR_DEFINE_FTK,      //�����������
  CONST_DEFINE_FTK,     //�����������
  FUNC_USE_FTK,     //�����������
  COUNT_FTK,    //�������,�������������
  TYPEDEF_FTK,    //typedef��ͷ�����ͱ����������
  FUNCPOINTER_DEF_FTK,  //����ָ�����������
  ASSIGN_FTK, //��ֵ���
  MEMBER_FUNCTION_USE_FTK,  //���ͷ����������
  TYPE_CHANGE_FTK,  //����ǿת���
  MEMBER_FIELD_USE_FTK, //�������Ե������
  ARR_VISIT_FTK,  //����Ԫ�ط��ʵ������
  FTK_NUM   //token������
}FTK;

//�жϾ��ӵķ�������,��ѡ��ͬ�ķ������
FTK getTokenLineKind(FuncTranslator* funcTranslator,TBNode* tokens);


//�ж��Ƿ��ǲ���Ҫ�������÷���ľ���
int ifNotNeedFuncTranslate(FuncTranslator* translator,TBNode* nodes);



//���������ж��Ӻ���
//�ж��Ƿ��Ǻ���ָ�붨�����
int isFuncPointerDefSentence(FuncTranslator* funcTranslator,TBNode* nodes);


//�ж��Ƿ��Ǹ�ֵ���
int isAssignSentence(FuncTranslator* funcTranslator,TBNode* nodes);

//�ж��Ƿ��������Ա����
int isArrVisitSentence(FuncTranslator* funcTranslator,TBNode* nodes);

//�ж��Ƿ�������ǿת���
int isTypeChangeSentence(FuncTranslator* funcTranslator,TBNode* nodes);


//���빦���Ӵ���,����ɹ����ط�NULL,����ʧ�ܷ���NULL

//�����Ա���Է������
TBNode* translateMemberFieldVisit(FuncTranslator* functranslator,TBNode* tokens);

//����Ԫ�ط������
TBNode* translateArrVisit(FuncTranslator* funcTranslator,TBNode* tokens);

//��������ǿת���
TBNode* translateTypeChange(FuncTranslator* funcTranslator,TBNode* tokens);

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

//������ͷ���


//���뺯����ѡ���

TBNode* (*tranFuncs[]) (FuncTranslator*,TBNode*) = {
  [NOT_LEAGAL_FTK] NULL,
  [NOT_TRANSLATE_FTK] NULL,
  [VAR_DEFINE_FTK] translateVarDef,
  [CONST_DEFINE_FTK] translateConstDef,     //�����������
  [FUNC_USE_FTK] translateFuncUse,     //�����������
  [COUNT_FTK] translateCountDef,    //�������,�������������
  [TYPEDEF_FTK] translateTypedef,    //typedef��ͷ�����ͱ����������
  [FUNCPOINTER_DEF_FTK] translateFuncPointerDef,  //����ָ�����������
  [ASSIGN_FTK] translateAssign, //��ֵ���
  [ARR_VISIT_FTK] translateArrVisit,  //����Ԫ�ط������
  [TYPE_CHANGE_FTK] translateTypeChange, //����ǿת���
  [MEMBER_FIELD_USE_FTK] translateMemberFieldVisit, //��Ա���Է������
  [MEMBER_FUNCTION_USE_FTK] translateTypeMethodUse   //���ͷ����������
};



#endif