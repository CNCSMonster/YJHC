#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mystring.h"
#include "mystring.c"

//���ھֲ��ṹ�嶨��,�ֲ��ṹ�嶨�岻֧�ֺ���
//����궨��ṹ��
struct poundline{
  char* name;
  char* value;
};

//���ļ��ж�ȡһ���궨���,ͨ��ָ�뷵�ض�ȡ���ĺ궨���Ĵ�С
struct poundline* loadPLs(FILE* pound_fin,int* returnSize);


//���к��滻,��һ���ַ����滻��ͬ,���滻��һ���������
char* pound_substitute_string(char* toChange,struct poundline val);

//ȥ����Ƕ��
char* pound_remove_nest(struct poundline* PLs,int size);


//��Ҫ���ܷ���
int pound_substitute_file(FILE* code_fin,FILE* pound_fin,FILE* fout);





/*
����:
1.���ļ��м��غ�,ѡ����ʵĽṹ����
2.�Ժ���л���ȥ��Ƕ��
3.���ļ����ݽ����滻

1.���غ�
��Ϣ�к���,��ֵ,��Ϊ�ַ���
ʹ�ö�̬�����ȡ,���浽�̶�����������
��Ϊʵ���Ϻ�ĳ���Ӧ�������޵�,����ĳ����к��ʹ��ֵ���������ܳ���2000����
2.�껯��
ʹ��һ���ֲ��Ĵ��ַ���������������
�������������Էֽ�����ж�ȡ,���ƥ�䵽������ݾ��滻,����Ͳ��滻
ÿ���궼���������к���к��滻����
���õ��ĺ�֮���������������滻��ϵ
3.�ļ��滻
ʹ�û����ĺ���ļ����ݽ����滻
*/


/*
�������:
����
#define M a_b
����
int M_c;//�������в���ƥ���M���ᱻ�滻
int a_b=3;
printf("%d",M); //��������M�ᱻ�滻��a_b

�ܽ����ֻ�ж���Ԫ���ܹ����к��滻
����Ԫ����,��Ϊ����������,��Ϊ������������,�����ǳ��ֵ�һ����Ԫ
Ҳ����˵����Ԫǰ��Ӧ���Ƕ��Ż��߷ֺŻ��߿ո���߻��л���С���Ż�����������߷�����
�����Ķ���Ԫ���ܹ����к��滻

������Ҫע����Ǻ�Ƕ�׵Ĵ���


*/



int main(int argc ,char* argv[]){

  if(argc!=4){
    printf("wrong!miss argument in doc_del");
    exit(-1);
  }
  FILE* code_fin=fopen(argv[1],"r"); //��Ҫ��ȡ�ļ�����ȡ���
  // FILE* fin=fopen("code.yjhc","r"); //��Ҫ��ȡ�ļ�����ȡ���
  if(!code_fin){
    //������ļ�ʧ��
    printf("fail to open %s",argv[1]);
    exit(-1);
  }
  FILE* pound_fin=fopen(argv[2],"r");
  // FILE* fout=fopen("out.txt","w"); //��Ҫ��ȡ�ļ�����ȡ���
  if(!code_fin){
    fclose(code_fin);
    printf("fail to open %s for write",argv[2]);
    exit(-1);
  }
  FILE* fout=fopen(argv[3],"w");
  // FILE* fout=fopen("out.txt","w"); //��Ҫ��ȡ�ļ�����ȡ���
  if(!fout){
    fclose(code_fin);
    fclose(pound_fin);
    printf("fail to open %s for write",argv[3]);
    exit(-1);
  }
  if(
    pound_substitute_file(code_fin,pound_fin,fout)
  ){
    printf("syntax error for pound_substitute");
  }
  fclose(code_fin);
  fclose(pound_fin);
  fclose(fout);
  return 0;
}



//���ļ��ж�ȡһ���궨���,ͨ��ָ�뷵�ض�ȡ���ĺ궨���Ĵ�С
struct poundline* loadPLs(FILE* pound_fin,int* returnSize){
  char tmp[2000];
  char tmp1[1000];
  int n=0;  //����ͳ�ƶ�ȡ����pound_fin������
  int capacity=100; //�����ʼ����
  //ʹ��һ����̬�������洢pound_fin
  struct poundline* PLs=(struct poundline*)malloc(sizeof(struct poundline)*capacity);

  char end=myfgets(tmp,"\n",pound_fin);
  while(end!=EOF&&end=='\n'){
    //��ʼ��ȡ
    mysgets(tmp1," ",tmp);
    //�����Ԥ�������ָ��,����������
    if(strcmp(tmp1,"#include")==0){
      end=myfgets(tmp,"\n",pound_fin);
      continue;
    }
    //����Ǻ��滻���,�������ȡ
    else if(strcmp(tmp1,"#define")){
      end=mysgets(tmp1," ",tmp+strlen("#define")+1);  //��ȡ��һ���ַ���
      //��������쳣,�쳣����
      if(end!=' '){
        for(int k=0;k<n;k++){free(PLs[k].name);free(PLs[k].value);}free(PLs);
        *returnSize=0;
        return NULL;
      }
      //���û�������쳣,
      PLs[n].name=(char*)malloc(sizeof(char)*(strlen(tmp1)+1));
      strcpy(PLs[n].name,tmp1);
      //�ڶ����ַ�Ӧ����\0��Ϊ�ս��
      char* next=tmp+strlen("#define")+1+strlen(tmp1)+1;
      end=mysgets(tmp1,"\0",next);  //��ȡ��һ���ַ���
      //��������쳣,�쳣����
      if(end!='\0'){
        //�����﷨�쳣
        printf("syntax error!each pound sentence should take and only take single line!");
        for(int k=0;k<n;k++){free(PLs[k].name);free(PLs[k].value);}free(PLs);
        *returnSize=0;
        return NULL;
      }
      PLs[n].value=(char*)malloc(sizeof(char)*(strlen(tmp1)+1));
      strcpy(PLs[n].value,tmp1);
      n++;
      //�ж�n�Ĵ�С�Ƿ��Ѿ��ﵽ������,����ﵽ��,����һ��+1
      //���������������,��������
      if(n==capacity){
        int newCapacity=capacity*3/2+1;
        void* newMem=malloc(sizeof(struct poundline)*newCapacity);
        capacity=newCapacity;
        memcpy(newMem,PLs,sizeof(struct poundline)*n);
        free(PLs);
      }
      //���������ȡ��һ��,������һ�����
      end=myfgets(tmp,"\n",pound_fin);
    }
    else{
      //�����﷨�쳣
      printf("syntax error!each pound sentence should take and only take single line!");
      //���֮ǰ��������пռ�,������NULL
      //������
      for(int k=0;k<n;k++){free(PLs[k].name);free(PLs[k].value);}free(PLs);
      *returnSize=0;
      return NULL;
    }
  }
  //�������쳣����
  if(end!=EOF){
    printf("syntax error!the last pound line should have a enter");
    //���֮ǰ��������пռ�,������NULL
    //������
    for(int k=0;k<n;k++){free(PLs[k].name);free(PLs[k].value);}free(PLs);
    *returnSize=0;
    return NULL;
  }
  //���ز���
  void* out=malloc(sizeof(struct poundline)*n);
  memcpy(out,PLs,sizeof(struct poundline)*n);
  free(PLs);
  *returnSize=n;
  return (struct poundline*)out;
}



//���к��滻,��һ���ַ����滻��ͬ,���滻��һ���﷨Ҫ��
char* pound_substitute_string(char* toChange,struct poundline val){
  //
  //�����ַ�����,Ӧ�ý�������Ĵ���,��Ϊ�ַ����о���ƥ���˺���Ҳ���ᱻ�滻
  //��ֵ���治֧�ְ���ַ���,Ҳ����������ַ���ǰ��ֽ��"�ɶԳ���,
  //��Ҫע����ǳɶԳ��ֵķֽ�����������ת���\"��������,Ҳ����˵��
  //���ַ�������\��"���������һ�������ʱ��,��������س���
  //��֮,���滻֮�󲻻�����γ��µ��ַ���,һ���ַ���Ҫôԭ�����ں궨������,Ҫô���ں�����
  //���Ժ����ⲿ,�����֮�䲻���γ��µ��ַ���
  //���Ժ��ڲ���val�����滻��ȫ���滻����Ч�ӽṹ,
  //Ҳ����˵�����������ܹ��ȶԺ����֮��ĺ��������������滻������ȥ��Ƕ��
  //����,�������Ҫȥȥ��Ƕ�׵ĺ�value��һ������,Ҳ��������û�зֽ��,��Ӧ�ý����滻
  char* stops=";\n ,)(.[]{}+-*/^<>=&|\\\"";
  char tmp[2000];
  char tmp1[200];
  int i=0;
  char end;
  while((end=mysgets(tmp1,stops,toChange))!='\0'){
    toChange+=strlen(tmp1)+1;
    //��������մ�,����Ҫд��,Ҳ����Ҫ�滻
    if(strcmp(tmp1,"")==0){
      ;
    }
    //�����Ҫ�滻,�����滻,����˵,���滻�������д���ļ���
    else if(strcmp(tmp1,val.name)==0){
      strcpy(tmp+i,val.value);
      i+=strlen(val.value);
    }
    //������Ҫ�滻,ԭ��д��
    else{
      strcpy(tmp+i,tmp1);
      i+=strlen(tmp1);
    }
    tmp[i++]=end;
    //���ݶ�ȡ�����ս���Ƿ���˫�����ж��Ƿ�Ҫ���������ַ�������
    if(end=='\"'){
      //ѭ����ȡ��������е�
      char* stops2="\"\\";
      int len=strlen(tmp1);
      while((end=mysgets(tmp1,stops2,toChange))!='\0'&&end!='\"'){
        toChange+=strlen(tmp1)+1;
        //�������ַ��ͺ�������һ��Ĳ��ֹ���ת���ַ�,һ�����
        if(end=='\\'&&(
          toChange[0]=='"'||
          toChange[0]=='\\'||
          toChange[0]=='n'||
          toChange[0]=='t'||
          toChange[0]=='b'||
          toChange[0]=='r'
        )){
          tmp[i++]=toChange[0];
          toChange++;
        }
        //����ֻ�ǵ�����һ����б��,
        else{

        }

      }
      //��������ַ���ĩβ�˶�û�г����ַ�����˫����,���쳣
      if(end!='\"'){
        return NULL;  //����NULL��ʾ�쳣,Ҳ���Ǻ��滻�쳣,��Ϊ���滻ֻ�ܹ����ַ��������
      }else{
        continue;
      }
    }
    tmp[i++]=end;
  }
  //TODO,����\0�˳��������һ�η������tmp1Ҫ����







  //��������滻��,���ɺ��ʴ�С�Ŀռ䱣����������
  //�������Ҫ�����ַ���������
  tmp[i++]='\0';
  char* out=(char*)malloc(sizeof(char)*(i));
  strcpy(out,tmp);
  return out;
}

//ȥ����Ƕ��,����Ҫ����ֵ
void pound_remove_nest(struct poundline* PLs,int size){
  for(int i=0;i<size;i++){
    for(int j=0;j<size;j++){
      if(i==j) continue;  //������궨��������Ƕ��
      char* newVal=pound_substitute_string(PLs[i].value,PLs[j]);
      free(PLs[i].value);
      PLs[i].value=newVal;
    }
  }
}


//��Ҫ���ܷ���
int pound_substitute_file(FILE* code_fin,FILE* pound_fin,FILE* fout){
  //����,�ȼ��غ궨���
  int size;
  struct poundline* PLs=loadPLs(pound_fin,&size);
  //Ȼ��,�Ժ궨������ȥ��Ƕ�״���
  pound_remove_nest(PLs,size);
  //Ȼ��ʹ�ú궨�����ļ���������滻
  char tmp[1000];
  char* stops=",;"


}