#include "type.h"


//���ļ��ж�ȡ�������ͱ�,ע��,���ͱ��ǰ������Ҫ����Ϊ��������
TypeTbl getTypeTbl(FILE* fin){
  TypeTbl out;
  out.strIds=getStrIdTable();
  //��ȡtypeTbl���������
  vector vec=getVector(sizeof(Type));
  //ÿ���ȶ�ȡͷ
  do{
    //�����ж��Ƿ���typedef,�����һλ��typedef,����滹Ҫ��ȡ����
    char tmp[2000];
    char end=myfgets(tmp,"\n",fin);
    if(end==EOF&&tmp[0]=='\0') break;
    char tmp2[100];
    end=mysgets(tmp2," ",tmp);
    char typeBaseName[200];
    char typeOtherName[200];
    char typePointerName[200];
    if(strcmp(tmp2,"typedef")==0){
      //typedef���ƶԽṹ�����ʱ�������
      //����

    }
    //�����ж��Ƿ��ǽṹ��,Ȼ���ж��Ƿ�����ͨ���Ͷ���
    else if(strcmp(tmp2,"enum")==0){

    }
    else if(strcmp(tmp2,"struct")==0){

    }
    //Ȼ���ж��Ƿ���
    else if(strcmp(tmp2,"union")==0){

    }
    //���ǰ��ȡһλ�ж��Ƿ��ȡ����
    char c=fgetc(fin);
    if(c==EOF) break;
    else ungetc(c,fin);
  }while(1);
  out.kinds=vector_toArr(vec);
  out.size=vec.size;
  vector_clear(&vec); //ɾ���������ݵı���
  return out;
}






//�����������ֲ�ѯһ������
Type findType(TypeTbl* tbl,char* typeName){

}


//ע��һ������Ϊĳ�����͵ı�������nάָ���������������typedef�����
//����˵��,tbl���õ��ı�,other��Ҫע������͵ı���,target��Ҫע������ͱ���,layer�������˵���ڵĲ��
int assignOtherName(TypeTbl* tbl,char* other,char* target,int layer){

}


//ɾ�����ͱ�
int delTypeTbl(TypeTbl& tbl){

}



