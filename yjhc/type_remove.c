#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "mystring.c"
#include "mystring.h"



int type_func_extract(FILE* fin,FILE* func_fout,FILE* type_fout);

int main(int argc,char* argv[]){
  //�Ѿ���ȥע��,ȥ�곣����Ԥ����ָ�ȥȫ�ֳ����ͱ�������ļ����д���
  //������ļ������ʱ��ֻ���к��������Ͷ���
  //����Ҫ��������ļ�,һ���Ǻ����ļ���һ�������Ͷ����ļ�
  char* inputPath;
  char* funcPath;
  char* typePath;
  if(argc!=4){
    exit(-1);
  }else{
    inputPath=argv[1];
    funcPath=argv[2];
    typePath=argv[3];
  }
  FILE* fin=fopen(inputPath,"r");
  if(!fin){
    printf("fail to open inputPath:%s",inputPath);
    exit(-1);
  }
  FILE* func_fout=fopen(funcPath,"w");
  if(!func_fout){
    fclose(fin);
    printf("fail to open funcPath:%s",funcPath);
    exit(-1);
  }
  FILE* type_fout=fopen(typePath,"w");
  if(!type_fout){
    fclose(fin);fclose(func_fout);
    printf("fail to open typePath:%s",typePath);
    exit(-1);
  }
  //Ȼ�������ʽ����������ȡ
  if(!type_func_extract(fin,func_fout,type_fout)){
    printf("syntax wrong");
  }
  fclose(fin);fclose(func_fout);fclose(type_fout);
  return 0;
}

//���ͣ�������ȡ�Լ�����
int type_func_extract(FILE* fin,FILE* func_fout,FILE* type_fout){
  //ÿ������Ԫ֮���û��з���β
  //��ȡ��֮��ÿ����Ԫ�ĵ�һ���ʶ��ǹؼ��֣�Ҫô��type,Ҫô�ǻ�����������
  char first[500];
  char end;
  while(!feof(fin)){
    myfgets(first," ",fin);
    if(strcmp(first,"")==0){
      continue;
    }
    printf("%s,\n",first);
    //��������Ԫ�ĵ�һ������typedef���������Ԫһ�������Ͷ���ʽ
    if(strcmp(first,"typedef")==0){
      myfgets(first,"\n",fin);
      fprintf(type_fout,"typedef %s\n",first);
    }
    //��������Ԫ�ĵ�һ������struct,������ǽṹ�嶨������ǽṹ�巽�����壬�ֻ�����ȫ�ַ�������
    else if(strcmp(first,"struct")==0){
      end=myfgets(first,">({",fin);
      //����ȶ�ȡ��'-'��ǰ����һ���������һ���ַ���'>'��˵���ǽṹ�巽����Ԫ
      if(end=='>'){
        fprintf(func_fout,"struct %s->",first);
        //��ȡ����ֱ������Ϊֹ������д�뷽������
        while((end=fgetc(fin))!=EOF&&end!='\n'){
          fputc(end,func_fout);
        }
        fputc(end,func_fout);
      }
      //���������������,˵����ȫ�ֺ���,д�뷽����
      else if(end=='('){
        fprintf(func_fout,"struct %s(",first);
        while((end=fgetc(fin))!=EOF&&end!='\n'){
          fputc(end,func_fout);
        }
        fputc(end,func_fout);
      }
      //���������{,˵���ǽṹ�嶨��,д�����Ͷ�����
      else if(end=='{'){
        fprintf(type_fout,"struct %s{",first);
        myfgets(first,"\n",fin);
        fprintf(type_fout,"%s\n",first);
      }

    }
    //�����һ�����ǻ�����������,��һ���Ǻ�������
    else if(isBaseType(first)){
      fprintf(func_fout,"%s ",first);
      myfgets(first,"\n",fin);
      fprintf(func_fout,"%s\n",first);
    }
    //�����union����enum�����������Ͷ��壬�����Ƿ�������
    else if(isTypeDefKeyWords(first)){
      char term[15];
      strcpy(term,first);
      end=myfgets(first,"{(",fin);
      //��������Ͷ���
      if(end=='{'){
        fprintf(type_fout,"%s %s{",term,first);
        myfgets(first,"\n",fin);
        fprintf(type_fout,"%s\n",first);
      }
      //����Ǻ�������
      else if(end=='('){
        fprintf(func_fout,"%s(",term);
        while((end=fgetc(fin))!=EOF&&end!='\n'){
          fputc(end,func_fout);
        }
        fputc(end,func_fout);
      } 
      //
      else return 0;
    }
    //����������Զ������͵ı���,�����ں���
    else{
      char term[15];
      strcpy(term,first);
      end=myfgets(first,"{(",fin);
      //����Ǻ�������
      if(end=='('){
        fprintf(func_fout,"%s(",term);
        while((end=fgetc(fin))!=EOF&&end!='\n'){
          fputc(end,func_fout);
        }
        fputc(end,func_fout);
      } 
      //
      else return 0;
    }
  }
  return 1;
}