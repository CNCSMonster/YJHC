#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//����ȥ��ע�ͺͶ���ո���ļ�
//������ȡ������Ԥ�����������ͳ������������һ���ļ���


//��ȡ�����к곣������ĳ���ļ���
//����Ϊpound_remove inputfile poundfile1 poundfile2
//����poundfile1�б���
int pound_remove(FILE* fin,FILE* fout,FILE* pound_fout);



//ʹ��
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
  FILE* pound_fout=fopen(argv[3],"w");  //��Ҫд����ȡ���ĺ궨����ļ�
  if(!pound_fout){
    fclose(fin);
    printf("fail to open %s for write",argv[23]);
    exit(-1);
  }
  pound_remove(fin,fout,pound_fout);
  fclose(fin);fclose(fout);fclose(pound_fout);
  return 0;
}



//����1Ϊ��ʾ����˺곣����Ԥ������������ȡ���������0˵���������쳣�˳�
int pound_remove(FILE* fin,FILE* fout,FILE* pound_fout){
  char c;
  int ifPrintEnter=0;
  //Ȼ��ʼ��ȡ,ÿ�ζ�ȡ��Ԥ������ſ�ʼ��ȡ
  while((c=fgetc(fin))!=EOF){
    if(c=='\n'){
      if(ifPrintEnter)  fputc('\n',fout);
      ifPrintEnter=0;
    }
    else if(c!='#'){
      fputc(c,fout);
      ifPrintEnter=1;
    }
    else if(c=='#'){
      char s[200];  //������Ԥ����������ߺ궨�峤�ȳ���200
      fscanf(fin,"%s",s);
      //�ж�ǰ׺�Ƿ���define
      if(memcmp(s,"define",strlen("define"))==0){
        //���
        if(s[strlen("define")]!='\0'){
          printf("wrong #define syntax");
          return 0;
        }
        //���������ȷ��,��ȡ���궨����
        else{
          fscanf(fin,"%s",s);
          //ǰ����ȡ��%s������
          fprintf(pound_fout,"#define %s",s);
          fscanf(fin,"%s",s);
          fprintf(pound_fout," %s\n",s);
        }
      }
      //�ж�ǰ׺�Ƿ���include
      else if(memcmp(s,"include",sizeof("include"))==0){
        //���������ȡ��#include
        
        if(s[strlen("include")]=='\0'){
          //���ֱ���ļ���β
          //���ȡ����һ�������ǿ��ַ�����
          fscanf(fin,"%s",s);
          // printf("1:%s\n",s);
          if((s[0]=='\"'&&s[strlen(s)-1]=='\"')||(s[0]=='<'&&s[strlen(s)-1]=='>')){
            s[strlen(s)-1]='\0';
            // printf("2:%s,%c\n",s,s[strlen(s)-1]);
            fprintf(pound_fout,"#include %s\n",s+1);
          }else{
            printf("wrong #include syntax3,%s",s);
            return 0;
          }
        }
        //�����������Ű������ļ���,���ȡ�ж�
        else if(s[strlen("include")]=='<'||s[strlen("include")]=='\"'){
          //
          strcpy(s,s+strlen("include"));
          if((s[0]=='\"'&&s[strlen(s)-1]=='\"')||(s[0]=='<'&&s[strlen(s)-1]=='>')){
            //�������ȷ�ģ���ȡ�����ļ���
            s[strlen(s)-1]='\0';
            fprintf(pound_fout,"#include %s\n",s+1);
          }else{
            printf("wrong #include syntax1");
            return 0;
          }
        }else{
          //���������������򱨴�
          printf("wrong #include syntax2");
          return 0;
        }
      }
      //������Ԥ����ָ��,����
      else{
        printf("wrong # use syntax");
        return 0;
      }
    }
  }
  return 0;
}

