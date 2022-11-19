#include <stdio.h>
#include <stdlib.h>


void showError();
int stateDo(int* state,char cur,FILE* fout);
int doc_del(int argc,char* argv[]);

//该程序作用是用来去除文件中的注释和多余空格和换行符
//使用命令行参数操控
/*
使用说明:
使用doc_del inputfile outputfile

*/

void showError(){
  printf("syntax error");
}



int main(int argc,char* argv[]){
  doc_del(argc,argv);
  return 0;
}


//使用自动机的方式实现这个逻辑,有十几种状态，四五种做法

//返回值用来表示接受或者退出,如果为0表示异常退出，为非0值表示正常
int stateDo(int* state,char cur,FILE* fout){
  //使用一个下推自动机编程
switch (*state)
{
case 0:
  if(cur==' '||cur=='\n') ;
  //如果遇到反斜杆
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
  //不小心两个状态都写成3了，
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
//如果是自动机没有定义的部分,返回错误逻辑
  return 0;
  break;
}
  return 1; //如果没有中断退出,返回1
}

int doc_del(int argc,char* argv[]){
  if(argc!=3){
    printf("wrong!miss argument in doc_del");
    exit(-1);
  }
  FILE* fin=fopen(argv[1],"r"); //打开要读取文件，获取句柄
  // FILE* fin=fopen("code.yjhc","r"); //打开要读取文件，获取句柄
  if(!fin){
    //如果打开文件失败
    printf("fail to open %s",argv[1]);
    exit(-1);
  }
  FILE* fout=fopen(argv[2],"w");
  // FILE* fout=fopen("out.txt","w"); //打开要读取文件，获取句柄
  if(!fout){
    fclose(fin);
    printf("fail to open %s for write",argv[2]);
    exit(-1);
  }
  int state=0;  //进行状态判断
  //state为0表示非注释状态,state为1表示单行注释状态，state为2表示多行注释状态
  char cur;
  char next; 
  int isInit=1; //判断是否打印了第一行 
  int preSpace=0; //用来判断前面是否在上一个语义成分之后打印了空格
  //使用超前搜索的方法,超前一位处理
  while((cur=fgetc(fin))!=EOF&&stateDo(&state,cur,fout));
  if(cur!=EOF||state==4||state==5||state==8||state==9||state==12||state==13) showError();
  fclose(fin);
  fclose(fout);
  return 0;
}