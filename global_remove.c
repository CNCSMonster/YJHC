#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int myfgets(char* buf,char* stops,FILE* fin);
int global_remove(FILE* fin,FILE* fout,FILE* global);
//�ж��Ƿ����Զ������Ͷ���ؼ���
int isTypeDefKeyWords(char* s);
//�ж��Ƿ�һ���ַ����ǻ����������͹ؼ���,�Ƿ��ط�0ֵ�����Ƿ���0
int isBaseType(char* s);
int mystrReplace(char* s,char old,char new);
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
      fprintf(fout,"%s%c",first,end);
      end=myfgets(first,";",fin);
      if(end!=';') return 0;    //Ӧ���ܹ�������;��β
      fprintf(fout,"%s%c",first,end);
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
      //����������ֺŽ���,��˵����ȫ�ֱ����������
      //����������ȺŽ�����˵����ȫ�ֱ�����������ҽ����˳�ʼ��
      if(end==';'){
        mystrReplace(first,'\n',' ');
        fprintf(global,"%s %s\n",last,first);
      }
      else if(end=='='){
        mystrReplace(first,'\n',' ');
        fprintf(global,"%s %s =",last,first);
        end=myfgets(first,";",fin);
        printf("after=:%s\n",first);
        mystrReplace(first,'\n',' ');
        fprintf(global,"%s\n",first);
        // printf("1\n");  //�������
      }
      //�����Ǻ���
      else if(end=='{'){
        mystrReplace(first,'\n',' ');
        fprintf(fout,"%s %s{",last,first);
        while((end=myfgets(first,";}",fin))!=EOF){
          if(end=='}') break;
          mystrReplace(first,'\n',' ');
          fprintf(fout,"%s;",first);
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

//�ж��Ƿ�һ���ַ����ǻ����������͹ؼ���,�Ƿ��ط�0ֵ�����Ƿ���0
int isBaseType(char* s){
  char* basetypes[]={
    "int",
    "short",
    "double",
    "long",
    "char"
  };
  for(int i=0;i<sizeof(basetypes)/sizeof(basetypes[0]);i++){
    if(!strcmp(s,basetypes[i])) return 1;
  }
  return 0;
}

//�ж��Ƿ����Զ������Ͷ���ؼ���
int isTypeDefKeyWords(char* s){
  char* typeKeyWords[]={
    "struct",
    "union",
    "enum"
  };
  for(int i=0;i<sizeof(typeKeyWords)/sizeof(typeKeyWords[0]);i++){
    if(!strcmp(s,typeKeyWords[i])) return 1;
  }
  return 0;
}

//��ȡһ���ַ���ֱ��������ֹ���ż��еķ��Ż��߶�ȡ���ļ�ĩβ,����ֵΪ��ֹ��
int myfgets(char* buf,char* stops,FILE* fin){
  char c;
  int i=0;
  while ((c=fgetc(fin))!=EOF)
  {
    int ifstop=0;
    for(int j=0;j<strlen(stops);j++){
      if(stops[j]==c){
        ifstop=1;
        break;
      }
    }
    if(ifstop){
      break;
    }
    buf[i++]=c;
  }
  buf[i]='\0';
  return c;
}
int mystrReplace(char* s,char old,char new){
  for(int i=0;i<strlen(s);i++) if(s[i]==old) s[i]=new;
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