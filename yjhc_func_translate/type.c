#include "type.h"


//从文件中读取建立类型表,注意,类型表的前面内容要设置为基础类型
TypeTbl getTypeTbl(FILE* fin){
  TypeTbl out;
  out.strIds=getStrIdTable();
  //读取typeTbl里面的内容
  vector vec=getVector(sizeof(Type));
  //每次先读取头
  do{
    //首先判断是否是typedef,如果第一位是typedef,则后面还要读取别名
    char tmp[2000];
    char end=myfgets(tmp,"\n",fin);
    if(end==EOF&&tmp[0]=='\0') break;
    char tmp2[100];
    end=mysgets(tmp2," ",tmp);
    char typeBaseName[200];
    char typeOtherName[200];
    char typePointerName[200];
    if(strcmp(tmp2,"typedef")==0){
      //typedef限制对结构定义的时候起别名
      //不能

    }
    //首先判断是否是结构体,然后判断是否是普通类型定义
    else if(strcmp(tmp2,"enum")==0){

    }
    else if(strcmp(tmp2,"struct")==0){

    }
    //然后判断是否是
    else if(strcmp(tmp2,"union")==0){

    }
    //最后超前读取一位判断是否读取结束
    char c=fgetc(fin);
    if(c==EOF) break;
    else ungetc(c,fin);
  }while(1);
  out.kinds=vector_toArr(vec);
  out.size=vec.size;
  vector_clear(&vec); //删除里面内容的表面
  return out;
}






//根据类型名字查询一个类型
Type findType(TypeTbl* tbl,char* typeName){

}


//注册一个类型为某个类型的别名或者n维指针别名，用来处理typedef的情况
//参数说明,tbl是用到的表,other是要注册的类型的别名,target是要注册的类型本名,layer是相对来说属于的层次
int assignOtherName(TypeTbl* tbl,char* other,char* target,int layer){

}


//删除类型表
int delTypeTbl(TypeTbl& tbl){

}



