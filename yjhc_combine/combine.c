#include "all.h"

//д���Ͷ���
int writeTypeDef(FILE* fin,FILE* fout);

//д������
int writeValDef(FILE* fin,FILE* fout);

//д���������ͺ�������
int writeFunc(FILE* typeFin,FILE* funcHeadFin,FILE* funcBodyFin,FILE* fout);

//�����������ͷ
int writeSingleFunc(FuncTbl* funcTbl,Func* func,FILE* fout);

//���������������
int writeSingleFuncDef(FuncTbl* funcTbl,Func* func,FILE* fout);



int main(int argc,char* argv[]){

  //cͷ�ļ������,��ʽ����#include�ĸ�ʽ
  char* libPath="../res/yjhclib.txt";
  //�ϲ���Ҫ���ͱ�
  char* typePath="../out/type.txt";

  char* valPath="../out/global.txt";
  //����ͷ��
  char* funcHeadPath="../out/func_head.txt";
  //������
  char* funcBodyPath="../out/func_format2.txt";

  char* outPutPath="../out/final.txt"; //�������c���Դ����ļ���

  

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
  //TODO,���м��
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
  
  //��ȡ��д��lib�е�����
  char tmp[1000];
  char end;
  while((end=myfgets(tmp,"\n",libFin))!=EOF){
    fprintf(fout,"%s%c",tmp,end);
  }
  fprintf(fout,"\n\n");
  fclose(libFin);

  
  //
  //��д��������Ϣ,������Ϣֱ�ӷ����ļ���д��
  //д��ֱ��
  writeTypeDef(typeFin,fout);
  fprintf(fout,"\n\n");

  //Ȼ��д��ȫ������Ϣ,ȫ������Ϣֱ��д��������
  writeValDef(valFin,fout);
  fprintf(fout,"\n\n");


  //Ȼ��д�뺯��ͷ��Ϣ,����ͷ��Ϣ��Ҫ���ͱ��������
  rewind(typeFin);
  writeFunc(typeFin,funcHeadFin,funcBodyFin,fout);
  fprintf(fout,"\n\n");


  //����ͷ���Դ

  fclose(fout);
  fclose(typeFin);
  fclose(valFin);
  fclose(funcBodyFin);
  fclose(funcHeadFin);

  return 0;
}

//д���Ͷ���
int writeTypeDef(FILE* fin,FILE* fout){
  //Ҫ���������Ͷ���
  //��ȡÿһ��,��ÿһ�н��д���
  char tmp[1000];
  char end;

  while((end=myfgets(tmp,"\n",fin))!=EOF||strlen(tmp)!=0){
    //��ȡ����ͷ��,��ȡֱ������{,��{��������ݽ���ѡ��
    //�ж����Ͷ����������
    char* track=tmp;
    char tmp2[1000];
    //���û�ж�ȡ����һ���ո�,�쳣
    if(mysgets(tmp2," ",tmp)!=' '){
      return 0;
    }
    if(strcmp(tmp2,"typedef")==0){
      fprintf(fout,"%s ",tmp2);
      track+=strlen(tmp2)+1;
    }
    //�Ƿ�Ҫ����ؼ�Ҫ�ж��Ƿ��Ƕ�����union����struct�Ķ���
    char tmp3[1000];
    if(mysgets(tmp3,"{",track)!='{'){
      fprintf(fout,"%s;\n",track);
      continue;
    }
    fprintf(fout,"%s{\n",tmp3);
    track+=strlen(tmp3)+1;
    //�ж�����
    end=mysgets(tmp2," ",tmp3);
    if(end!=' '){
      return 0;
    }
    //
    if(strcmp(tmp2,"enum")==0){
      //��ȡÿ��ֱ������,����
      while(end=mysgets(tmp3,",}",track)!='}'&&end!='\0'){
        fprintf(fout,"%s,\n",tmp3);
        track+=strlen(tmp3)+1;
      }
      if(end=='\0') return 0;
      //��ӡʣ�ಿ��
      fprintf(fout,"}%s;\n",track);
    }
    else if(strcmp(tmp2,"struct")==0||strcmp(tmp2,"union")==0){
      //��ȡÿ��ֱ������,����
      while((end=mysgets(tmp3,";}",track))!='}'&&end!='\0'){
        //����ȥ��ǰ��ո�
        track+=strlen(tmp3)+1;
        myStrStrip(tmp3," "," ");
        //����ǳ�Ա��������������
        if(strlen(tmp3)==0) continue;
        if(myIsCharInStr(tmp3,'(')&&tmp3[strlen(tmp3)-1]==')'){
          continue;
        }
        //�����ǳ�Ա��������,��ӡ
        fprintf(fout,"%s;\n",tmp3);
      }
      if(end=='\0') return 0;
      //��ӡʣ�ಿ��
      myStrStrip(track," "," ");
      fprintf(fout,"%s;\n",track);
    }
    else{
      return 0;
    }
  }
  return 1;
} 

//д������
int writeValDef(FILE* fin,FILE* fout){
  //
  char end;
  char tmp[1000];
  while((end=myfgets(tmp,"\n",fin))!=EOF||strlen(tmp)!=0){
    fprintf(fout,"%s;\n",tmp);
  }
  return 1;
}

//д���������ͺ�������
int writeFunc(FILE* typeFin,FILE* funcHeadFin,FILE* funcBodyFin,FILE* fout){
  TypeTbl typeTbl=getGlobalTypeTbl();
  loadFile_typeTbl(&typeTbl,typeFin);
  FuncTbl funcTbl=getFuncTbl(&typeTbl);
  loadFile_functbl(&funcTbl,funcHeadFin);
  //Ȼ��ÿ��ȡһ������ͷ,�Ժ���ͷ���и���
  int isRight=1;
  //��������
  for(int i=0;i<funcTbl.funcKeys.size&&isRight;i++){
    char* funcKey;
    vector_get(&funcTbl.funcKeys,i,&funcKey);
    //���Һ���
    Func* func;
    if(!hashtbl_get(&funcTbl.funcs,&funcKey,&func)){
      isRight=0;
      break;
    }
    //����ҵ�����,�Ժ������д���
    //��ȡ�����ĸ�������,����ֱ�����������
    writeSingleFunc(&funcTbl,func,fout);
    
    fprintf(fout,";\n");
  }
  fprintf(fout,"\n\n");
  //�������funcBodyӦ���Ǿ�������ĺ����ļ�
  for(int i=0;i<funcTbl.funcKeys.size&&isRight;i++){
    char* funcKey;
    vector_get(&funcTbl.funcKeys,i,&funcKey);
    //���Һ���
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

//�����������ͷ
int writeSingleFunc(FuncTbl* funcTbl,Func* func,FILE* fout){
  Func* t=NULL;
  if(func->owner!=NULL){
    t=get_rename_member_func(funcTbl,func);
    func=t;
  }
  // �����������ֵ
  Type retType;
  int layer;
  findTypeById(funcTbl->globalTypeTbl, func->retTypeId, &retType, &layer);
  char *typeName = getTypeName(retType.defaultName, layer);
  fprintf(fout, "%s ", typeName);
  free(typeName);
  fprintf(fout, "%s(", func->func_name);
  // ��ӡ�����б�
  for (int i = 0; i < func->args.size; i++)
  {
    Arg arg;
    vector_get(&func->args, i, &arg);
    findTypeById(funcTbl->globalTypeTbl, arg.typeId, &retType, &layer); // ���Ҳ�������
    // ��ȡ����������
    char *argTypeName = getTypeName(retType.defaultName, layer);
    fprintf(fout, "%s %s", argTypeName, arg.name);
    free(argTypeName);
    // �ж��Ƿ�Ҫ��ӡ����
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





