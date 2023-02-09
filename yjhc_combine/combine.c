#include "all.h"

//写类型定义
int writeTypeDef(FILE* fin,FILE* fout);

//写量定义
int writeValDef(FILE* fin,FILE* fout);

//写函数声明和函数定义
int writeFunc(FILE* typeFin,FILE* funcHeadFin,FILE* funcBodyFin,FILE* fout);

//输出单个函数头
int writeSingleFunc(FuncTbl* funcTbl,Func* func,FILE* fout);

//输出单个函数定义
int writeSingleFuncDef(FuncTbl* funcTbl,Func* func,FILE* fout);



int main(int argc,char* argv[]){

  //c头文件引入表,格式就是#include的格式
  char* libPath="../res/yjhclib.txt";
  //合并需要类型表
  char* typePath="../out/type.txt";

  char* valPath="../out/global.txt";
  //函数头表
  char* funcHeadPath="../out/func_head.txt";
  //函数体
  char* funcBodyPath="../out/func_format2.txt";

  char* outPutPath="../out/final.txt"; //输出最终c语言代码文件名

  

  if(argc<7){
    printf("miss args\n");
    exit(0);
  }
  else{
    libPath=argv[1];
    typePath=argv[2];
    valPath=argv[3];
    funcHeadPath=argv[4];
    funcBodyPath=argv[5];
    outPutPath=argv[6];
  }
  FILE* libFin=fopen(libPath,"r");
  FILE* typeFin=fopen(typePath,"r");
  FILE* valFin=fopen(valPath,"r");
  FILE* funcHeadFin=fopen(funcHeadPath,"r");
  FILE* funcBodyFin=fopen(funcBodyPath,"r");
  FILE* fout=fopen(outPutPath,"w");
  //TODO,进行检查
  int ai=0;
  if(!(libFin&&typeFin&&valFin&&funcHeadFin&&funcBodyFin&&fout)){

    if(libFin!=NULL) fclose(libFin);
    else printf("1\n");
    if(typeFin!=NULL) fclose(typeFin);
    else printf("2\n");
    if(valFin!=NULL) fclose(valFin);
    else printf("3\n");
    if(funcHeadFin!=NULL) fclose(funcHeadFin);
    else printf("4\n");
    if(funcBodyFin!=NULL) fclose(funcBodyFin);
    else printf("5\n");
    if(fout!=NULL) fclose(fout);
    else printf("6\n");
    return 0;
  }
  
  //读取并写入lib中的内容
  char tmp[1000];
  char end;
  while((end=myfgets(tmp,"\n",libFin))!=EOF){
    fprintf(fout,"%s%c",tmp,end);
  }
  fprintf(fout,"\n\n");
  fclose(libFin);

  
  //
  //先写入类型信息,类型信息直接分析文件可写出
  //写入直到
  writeTypeDef(typeFin,fout);
  fprintf(fout,"\n\n");

  //然后写入全局量信息,全局量信息直接写入量就行
  writeValDef(valFin,fout);
  fprintf(fout,"\n\n");


  //然后写入函数头信息,函数头信息需要类型表才能做出
  rewind(typeFin);
  writeFunc(typeFin,funcHeadFin,funcBodyFin,fout);
  fprintf(fout,"\n\n");


  //最后释放资源

  fclose(fout);
  fclose(typeFin);
  fclose(valFin);
  fclose(funcBodyFin);
  fclose(funcHeadFin);

  return 0;
}

//写类型定义
int writeTypeDef(FILE* fin,FILE* fout){
  //要完成这个类型定义
  //读取每一行,对每一行进行处理
  char tmp[1000];
  char end;

  while((end=myfgets(tmp,"\n",fin))!=EOF||strlen(tmp)!=0){
    //读取类型头部,读取直到遇到{,则{里面的内容进行选择
    //判断类型定义语句内容
    char* track=tmp;
    char tmp2[1000];
    //如果没有读取到第一个空格,异常
    if(mysgets(tmp2," ",tmp)!=' '){
      return 0;
    }
    if(strcmp(tmp2,"typedef")==0){
      fprintf(fout,"%s ",tmp2);
      track+=strlen(tmp2)+1;
    }
    //是否要处理关键要判断是否是读到了union或者struct的定义
    char tmp3[1000];
    if(mysgets(tmp3,"{",track)!='{'){
      fprintf(fout,"%s;\n",track);
      continue;
    }
    fprintf(fout,"%s{\n",tmp3);
    track+=strlen(tmp3)+1;
    //判断类型
    end=mysgets(tmp2," ",tmp3);
    if(end!=' '){
      return 0;
    }
    //
    if(strcmp(tmp2,"enum")==0){
      //读取每个直到遇到,结束
      while(end=mysgets(tmp3,",}",track)!='}'&&end!='\0'){
        fprintf(fout,"%s,\n",tmp3);
        track+=strlen(tmp3)+1;
      }
      if(end=='\0') return 0;
      //打印剩余部分
      fprintf(fout,"}%s;\n",track);
    }
    else if(strcmp(tmp2,"struct")==0||strcmp(tmp2,"union")==0){
      //读取每个直到遇到,结束
      while((end=mysgets(tmp3,";}",track))!='}'&&end!='\0'){
        //首先去除前后空格
        track+=strlen(tmp3)+1;
        myStrStrip(tmp3," "," ");
        //如果是成员方法定义则跳过
        if(strlen(tmp3)==0) continue;
        if(myIsCharInStr(tmp3,'(')&&tmp3[strlen(tmp3)-1]==')'){
          continue;
        }
        //否则是成员变量定义,打印
        fprintf(fout,"%s;\n",tmp3);
      }
      if(end=='\0') return 0;
      //打印剩余部分
      myStrStrip(track," "," ");
      fprintf(fout,"%s;\n",track);
    }
    else{
      return 0;
    }
  }
  return 1;
} 

//写量定义
int writeValDef(FILE* fin,FILE* fout){
  //
  char end;
  char tmp[1000];
  while((end=myfgets(tmp,"\n",fin))!=EOF||strlen(tmp)!=0){
    fprintf(fout,"%s;\n",tmp);
  }
  return 1;
}

//写函数声明和函数定义
int writeFunc(FILE* typeFin,FILE* funcHeadFin,FILE* funcBodyFin,FILE* fout){
  TypeTbl typeTbl=getGlobalTypeTbl();
  loadFile_typeTbl(&typeTbl,typeFin);
  FuncTbl funcTbl=getFuncTbl(&typeTbl);
  loadFile_functbl(&funcTbl,funcHeadFin);
  //然后每读取一个函数头,对函数头进行改名
  int isRight=1;
  //函数声明
  for(int i=0;i<funcTbl.funcKeys.size&&isRight;i++){
    char* funcKey;
    vector_get(&funcTbl.funcKeys,i,&funcKey);
    //查找函数
    Func* func;
    if(!hashtbl_get(&funcTbl.funcs,&funcKey,&func)){
      isRight=0;
      break;
    }
    //如果找到函数,对函数进行处理
    //获取函数的改名函数,或者直接输出参数声
    writeSingleFunc(&funcTbl,func,fout);
    
    fprintf(fout,";\n");
  }
  fprintf(fout,"\n\n");
  //这里面的funcBody应该是经过翻译的函数文件
  for(int i=0;i<funcTbl.funcKeys.size&&isRight;i++){
    char* funcKey;
    vector_get(&funcTbl.funcKeys,i,&funcKey);
    //查找函数
    Func* func;
    if(!hashtbl_get(&funcTbl.funcs,&funcKey,&func)){
      isRight=0;
      break;
    }
    writeSingleFunc(&funcTbl,func,fout);
    char tmp[1000];
    char end;
    end=myfgets(tmp,"\n",funcBodyFin);
    if(tmp[0]!='{') return 0;
    fprintf(fout,"{\n");
    int lp=1;
    while((end=fgetc(funcBodyFin))){
      if(end=='{') lp++;
      else if(end=='}') lp--;
      fprintf(fout,"%c",end);
      if(lp==0){
        end=fgetc(funcBodyFin);
        fprintf(fout,"\n");
        break;
      }
    }
  }

  if(!isRight) return 0;

  return 1;
}

//输出单个函数头
int writeSingleFunc(FuncTbl* funcTbl,Func* func,FILE* fout){
  Func* t=NULL;
  if(func->owner!=NULL){
    t=get_rename_member_func(funcTbl,func);
    func=t;
  }
  // 首先输出返回值
  Type retType;
  int layer;
  findTypeById(funcTbl->globalTypeTbl, func->retTypeId, &retType, &layer);
  char *typeName = getTypeName(retType.defaultName, layer);
  fprintf(fout, "%s ", typeName);
  free(typeName);
  fprintf(fout, "%s(", func->func_name);
  // 打印参数列表
  for (int i = 0; i < func->args.size; i++)
  {
    Arg arg;
    vector_get(&func->args, i, &arg);
    findTypeById(funcTbl->globalTypeTbl, arg.typeId, &retType, &layer); // 查找参数类型
    // 获取参数类型名
    char *argTypeName = getTypeName(retType.defaultName, layer);
    fprintf(fout, "%s %s", argTypeName, arg.name);
    free(argTypeName);
    // 判断是否要打印逗号
    if (i != func->args.size - 1)
      fprintf(fout, ",");
  }
  fprintf(fout, ")");
  if(t!=NULL){
    del_func(t);
    free(t);
  }
  return 1;
}





