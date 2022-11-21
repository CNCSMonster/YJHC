#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//对于去掉注释和多余空格的文件
//从中提取出所有预编译包含命令和常量定义输出到一个文件中


//提取出所有宏常量到到某个文件中
//命令为pound_remove inputfile poundfile1 poundfile2
//其中poundfile1中保存
int pound_remove(FILE* fin,FILE* fout,FILE* pound_fout);



//使用
int main(int argc,char* argv[]){
  //把含有井的预编译命令和宏常量抵挡一提取出来
  if(argc!=4){
    printf("wrong!we need and only need 3 argument,pleas check your input");
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
  FILE* pound_fout=fopen(argv[3],"w");  //打开要写入提取出的宏定义的文件
  if(!pound_fout){
    fclose(fin);
    printf("fail to open %s for write",argv[23]);
    exit(-1);
  }
  pound_remove(fin,fout,pound_fout);
  fclose(fin);fclose(fout);fclose(pound_fout);
  return 0;
}



//返回1为表示完成了宏常量和预编译包含语句提取，如果返回0说明出现了异常退出
int pound_remove(FILE* fin,FILE* fout,FILE* pound_fout){
  char c;
  int ifPrintEnter=0;
  //然后开始读取,每次读取到预编译符号开始提取
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
      char s[200];  //不允许预编译包含或者宏定义长度超过200
      fscanf(fin,"%s",s);
      //判断前缀是否是define
      if(memcmp(s,"define",strlen("define"))==0){
        //如果
        if(s[strlen("define")]!='\0'){
          printf("wrong #define syntax");
          return 0;
        }
        //否则就是正确的,提取到宏定义中
        else{
          fscanf(fin,"%s",s);
          //前面提取出%s的名字
          fprintf(pound_fout,"#define %s",s);
          fscanf(fin,"%s",s);
          fprintf(pound_fout," %s\n",s);
        }
      }
      //判断前缀是否是include
      else if(memcmp(s,"include",sizeof("include"))==0){
        //如果仅仅读取了#include
        
        if(s[strlen("include")]=='\0'){
          //如果直接文件结尾
          //则读取后面一个连续非空字符序列
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
        //如果后面紧跟着包含的文件名,则截取判断
        else if(s[strlen("include")]=='<'||s[strlen("include")]=='\"'){
          //
          strcpy(s,s+strlen("include"));
          if((s[0]=='\"'&&s[strlen(s)-1]=='\"')||(s[0]=='<'&&s[strlen(s)-1]=='>')){
            //如果是正确的，提取到宏文件中
            s[strlen(s)-1]='\0';
            fprintf(pound_fout,"#include %s\n",s+1);
          }else{
            printf("wrong #include syntax1");
            return 0;
          }
        }else{
          //如果是其他情况，则报错
          printf("wrong #include syntax2");
          return 0;
        }
      }
      //否则不是预编译指令,报错
      else{
        printf("wrong # use syntax");
        return 0;
      }
    }
  }
  return 0;
}

