#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mystring.c"
#include "mystring.h"


int global_remove(FILE* fin,FILE* fout,FILE* global);

/*
finΪȥ���궨�����ļ���ָ��,foutΪд��ȥ��ȫ��������ļ���ָ��,globalΪȫ�ֱ�����λ��
��ȫ�ֱ�����ȫ�ֳ���(�Ǻ궨�峣��)��ȡ��һ���ļ�֮��
ȫ�ֳ�������:
const int m;
const int arr[2]={1,2};

ȫ�ֳ������� ����:
int m;
int arr[2]={1,2};

c������ȫ�ֳ����Ķ�����const��ͷ���ԷֺŽ�β�����ҿ��Ը�ֵ���������һ��Ĭ�ϵĳ�ʼֵ
ȫ�ֱ�����������������ͷ���ԷֺŽ�β����ʼ��һ��Ҫ�ó�����ֵ�����߲���ʼ��������һ��Ĭ�ϵĳ�ʼֵ


*/



int global_remove(FILE* fin,FILE* fout,FILE* global){
  //���û�ж����ļ���ͷ,�������ȡ
  //ÿ�����Ŀ�ͷһ����һ���ؼ�������,Ҫô����������,Ҫô�ǽṹ����Ż��߹��������
  //����������������,�����ǳ��������ؼ���
  char first[400]; //��ȡ��һ���ؼ���
  while(!feof(fin)){
    char end;
    end=myfgets(first,"; \n",fin);
    if(strcmp(first,"")==0) continue;   //�ų����ܶ������ַ������
    printf("^%s$\n",first);
    //�ж��Ƿ�������������
    if(strcmp(first,"typedef")==0){
      printf("c1\n");
      //���������������,��һ������ȫ�ֳ�����ȫ�ֱ������壬
      //ֻ���������Ͷ���
      fprintf(fout,"%s ",first);
      //��ȡ������
      end=myfgets(first,"{;",fin);
      if(end==';'){
        mystrReplace(first,'\n',' ');
        fprintf(fout,"%s\n",first); //�﷨��Ԫ��ͳһ�û��зָ�
        return 0;    //Ӧ���ܹ�������;��β
      }
      else if(end=='{'){
        //���typedef�������ȶ�����{}
        //�������ŵ�Ӧ����struct,enum,union��Щ�Զ������͵Ķ���
        //��Ӧ���ȶ����һ�����д��,Ȼ���ٶ������
        mystrReplace(first,'\n',' ');
        fprintf(fout,"%s{",first);
        //Ȼ�������Ӧ�ö�ȡ�һ������Լ��м�����
        end=myfgets(first,"}",fin);
        mystrReplace(first,'\n',' ');
        fprintf(fout,"%s}",first);
        //��ȡ���һ����ź�Ҫ��ȡһ������
        end=myfgets(first,";",fin);
        fprintf(fout,"%s\n",first);
      }
      else return 0;
    }
    //�ж��Ƿ��ǳ�����������
    else if(strcmp(first,"const")==0){
      printf("c2\n");
      //˵���ǳ�������
      end=myfgets(first,";",fin); //��ȡ����һ���ֺ�Ϊֹ
      //��������д���ļ���ȥ���ֺ�,���Ұѳ���������������л����滻�ɿո�
      mystrReplace(first,'\n',' ');
      fprintf(global,"const %s\n",first);
    }
    //�ж��Ƿ��ǽṹ��ؼ���,����ǽṹ������,�����ǽṹ�巽����������,Ҳ�����ǽṹ�嶨�����
    else if(isTypeDefKeyWords(first)||isBaseType(first)){
      printf("c3\n");
      char last[100];
      strcpy(last,first);
      end=myfgets(first,";{=",fin);
      //����������ֺŽ���,��˵����ȫ�ֱ������������ߺ�������
      if(end==';'){
        //���ǰ�������������,˵���Ǻ����������,����ֱ��ʡ��
        if(first[strlen(first)-1]==')'||first[strlen(first)-2]==')'){
          printf("func anouncement:%s",first);
          continue;
        }
        mystrReplace(first,'\n',' ');
        fprintf(global,"%s %s\n",last,first);
      }
      //����������ȺŽ�����˵����ȫ�ֱ�����������ҽ����˳�ʼ��
      else if(end=='='){
        mystrReplace(first,'\n',' ');
        fprintf(global,"%s %s =",last,first);
        end=myfgets(first,";",fin);
        printf("after=:%s\n",first);
        mystrReplace(first,'\n',' ');
        fprintf(global,"%s\n",first);
        // printf("1\n");  //�������
      }
      //�����Ǻ��������Զ������Ͷ��壬�������������forѭ��,���߻���������,���Ի�Ҫ���д���
      //ʹ��һ�������������������ж��Ƿ�����˺�����β
      else if(end=='{'){
        mystrReplace(first,'\n',' ');
        fprintf(fout,"%s %s{",last,first);
        int leftPar=1;  //��¼û��ƥ�������������
        while((end=myfgets(first,"{;}",fin))!=EOF){
          if(end=='}'&&leftPar==1) break;
          //������������ţ�˵�����������鸳ֵ�������ô����
          else if(end=='{'){
            leftPar++;
            mystrReplace(first,'\n',' ');
            fprintf(fout,"%s{",first);
          }else if(end=='}'){
            leftPar--;
            mystrReplace(first,'\n',' ');
            fprintf(fout,"%s}",first);
          }else{
            mystrReplace(first,'\n',' ');
            fprintf(fout,"%s;",first);
          }
        }
        if(end=='}') fprintf(fout," }\n");
        else return 0;
      }
      //�쳣
      else return 0;
    }
    //�������쳣���
    else{
      return 0;
    }
  }
  return 1;
}


int main(int argc,char* argv[]){
  //�Ѻ��о���Ԥ��������ͺ곣���ֵ�һ��ȡ����
  if(argc!=4){
    printf("wrong!we need and only need 3 argument,pleas check your input");
    exit(-1);
  }
  FILE* fin=fopen(argv[1],"r"); //��Ҫ��ȡ�ļ�����ȡ���
  // FILE* fin=fopen("code.yjhc","r"); //��Ҫ��ȡ�ļ�����ȡ���
  if(!fin){
    //������ļ�ʧ��
    printf("fail to open %s",argv[1]);
    exit(-1);
  }
  FILE* fout=fopen(argv[2],"w");
  // FILE* fout=fopen("out.txt","w"); //��Ҫ��ȡ�ļ�����ȡ���
  if(!fout){
    fclose(fin);
    printf("fail to open %s for write",argv[2]);
    exit(-1);
  }
  FILE* global_fout=fopen(argv[3],"w");  //��Ҫд����ȡ���ĺ궨����ļ�
  if(!global_fout){
    fclose(fin);
    printf("fail to open %s for write",argv[23]);
    exit(-1);
  }
  if(!global_remove(fin,fout,global_fout)){
    printf("fail to pick up global sentence from input");
  }
  fclose(fin);fclose(fout);fclose(global_fout);
  return 0;
}