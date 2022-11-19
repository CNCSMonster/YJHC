#include <stdio.h>
#include <stdlib.h>


void showError();
int stateDo(int* state,char cur,FILE* fout);
int doc_del(int argc,char* argv[]);

//�ó�������������ȥ���ļ��е�ע�ͺͶ���ո�ͻ��з�
//ʹ�������в����ٿ�
/*
ʹ��˵��:
ʹ��doc_del inputfile outputfile

*/

void showError(){
  printf("syntax error");
}



int main(int argc,char* argv[]){
  doc_del(argc,argv);
  return 0;
}


//ʹ���Զ����ķ�ʽʵ������߼�,��ʮ����״̬������������

//����ֵ������ʾ���ܻ����˳�,���Ϊ0��ʾ�쳣�˳���Ϊ��0ֵ��ʾ����
int stateDo(int* state,char cur,FILE* fout){
  //ʹ��һ�������Զ������
switch (*state)
{
case 0:
  if(cur==' '||cur=='\n') ;
  //���������б��
  else if(cur=='/') *state=1;
  else{
    fprintf(fout,"%c",cur);
    *state=2;
  }
  break;
case 1:
  if(cur=='/') *state=3;
  else if(cur=='*') *state=4;
  else{
    return 0;
  }
  break;
case 2:
  if(cur==' '||cur=='\n'){
    fprintf(fout,"%c",cur);
    *state=33;
  }
  else if(cur=='/') *state=10;
  else{
    fprintf(fout,"%c",cur);
    *state=2;
  }
  break;
case 3:
  if(cur=='\n') *state=0;
  else *state=3;
  break;
case 33:
  //��С������״̬��д��3�ˣ�
  if(cur==' '||cur=='\n') *state=33;
  else if(cur=='/') *state=6;
  else{
    fprintf(fout,"%c",cur);
    *state=2;
  }
  break;
case 4:
  if(cur=='*') *state=5;
  else *state=4;
  break;
case 5:
  if(cur=='*') *state=5;
  else if(cur=='/') *state=0;
  else  *state=4;
  break;
case 6:
  if(cur=='*') *state=8;
  else if(cur=='/') *state=7;
  else if(cur==' '||cur=='\n'){
    fprintf(fout,"/\n");
    *state=33;
  }else{
    fprintf(fout,"/%c",cur);
    *state=2;
  }
  break;
case 7:
  if(cur=='\n') *state=33;
  else *state=7;
  break;
case 8:
  if(cur=='*') *state=9;
  else *state=8;
  break;
case 9:
  if(cur=='/') *state=33;
  else if(cur=='*') *state=9;
  else *state=8;
  break;
case 10:
  if(cur=='\n'||cur==' '){
    fprintf(fout,"%c",cur);
    *state=33;
  }
  else if(cur=='/') *state=11;
  else if(cur=='*') *state=12;
  else{
    fprintf(fout,"/%c",cur);
    *state=2;
  }
  break;
case 11:
  if(cur=='\n'){
    fprintf(fout,"%c",cur);
    *state=33;
  }
  else *state=11;
  break;
case 12:
  if(cur=='*') *state=13;
  else *state=12;
  break;
case 13:
  if(cur=='/') *state=2;
  else if(cur=='*') *state=13;
  else *state=12;
  break;
default:
//������Զ���û�ж���Ĳ���,���ش����߼�
  return 0;
  break;
}
  return 1; //���û���ж��˳�,����1
}

int doc_del(int argc,char* argv[]){
  if(argc!=3){
    printf("wrong!miss argument in doc_del");
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
  int state=0;  //����״̬�ж�
  //stateΪ0��ʾ��ע��״̬,stateΪ1��ʾ����ע��״̬��stateΪ2��ʾ����ע��״̬
  char cur;
  char next; 
  int isInit=1; //�ж��Ƿ��ӡ�˵�һ�� 
  int preSpace=0; //�����ж�ǰ���Ƿ�����һ������ɷ�֮���ӡ�˿ո�
  //ʹ�ó�ǰ�����ķ���,��ǰһλ����
  while((cur=fgetc(fin))!=EOF&&stateDo(&state,cur,fout));
  if(cur!=EOF||state==4||state==5||state==8||state==9||state==12||state==13) showError();
  fclose(fin);
  fclose(fout);
  return 0;
}