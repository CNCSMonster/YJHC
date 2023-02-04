#ifndef _FUNC_TRANSLATE_H
#define _FUNC_TRANSLATE_H

#include "lib_use.h"
#include "syntax_symbol.h"
#include "func.h"
#include "type.h"
#include "val_tbl.h"
#include "token_reader.h"
#include "config.h"



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
  SELF_FIELD_VISIT_FTK,                //�����Ա���Է������
  SELF_FUNC_VISIT_FTK,  //�������������
  SET_EXP_FTK,  //��������{a1,a2}������ö�ٱ��ʽ
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

//����ö�ٱ��ʽ
TBNode* translateSetExp(FuncTranslator* funcTranslator,TBNode* tokens);

//�����������
TBNode* translateCountDef(FuncTranslator* functranslator,TBNode* tokens);

//�������������Ӻ���


//�������������Ӻ���

//ȥ�����ű��ʽ
TBNode* removeParExp(FuncTranslator* translator,TBNode* tokens);

//ȥ�����б��ʽ
TBNode* removeSetExp(FuncTranslator* translator,TBNode* tokens);

//ȥ��self���ʽ
TBNode* removeSelfExp(FuncTranslator* translator,TBNode* tokens);

//ȥ�����г�Ա���ʱ��ʽ��������ʱ��ʽ,(��ʵ��,��Ϊ��Ա���ʵ�Ч����������ʵ�Ч���Ǳ���һ����)
TBNode* removeMemVisitAndArrVisit(FuncTranslator* translator,TBNode* tokens);

//ȥ�������,Ҫ�������ȼ�,��ȥ�����ȼ���ߵ������
TBNode* removeOP(FuncTranslator* translator,TBNode* tokens);


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

//�����������Ե������
TBNode* translateSelfFieldVisit(FuncTranslator* funcTranslator,TBNode* tokens);

//�����������������
TBNode* translateSelfFuncVisit(FuncTranslator* funcTranslator,TBNode* tokens);


//���䷭�븨��,�ж��ַ����Ƿ����ܹ����ӵ���������������
int preValNameAddJudge(FuncTranslator* funcTranslator,char* s);


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
  [SELF_FIELD_VISIT_FTK] translateSelfFieldVisit,
  [SELF_FUNC_VISIT_FTK] translateSelfFuncVisit,
  [SET_EXP_FTK] translateSetExp,  //����ö�ٱ��ʽ,����˵���뼯�ϱ��ʽ
  [MEMBER_FIELD_USE_FTK] translateMemberFieldVisit, //��Ա���Է������
  [MEMBER_FUNCTION_USE_FTK] translateTypeMethodUse   //���ͷ����������
};

//��TBNode*ĳ��λ����,��ȡһ�������ű��ʽ,����[dsds] ��ȡ�Ľ����head��tail��Ϊ[dsds]
int searchBracketExpression(TBNode* nodes,TBNode** head,TBNode** tail);

//�ڲ����Ŀ�ͷ�����������ʽ�Ľ�β,ͨ������ָ�뷵�ؽ�β,�����ɹ����ط�0ֵ�������쳣����0
int searchArgExpression(TBNode* head,TBNode** tail);

//��head�������ʽ֪������ĳ�����͵�token,�������������ʽ��ͷΪֹ(ע��,�м����Ű������Ĳ��ֱ���Ϊ����)
//��������������ط�0ֵ,���������������ڷ���0,��������쳣Ҳ����0
int searchExpressUntil(TBNode* head,TBNode** retTail,TokenKind* kinds,int kindSize);

#endif