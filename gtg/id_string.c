#include "id_string.h"


//��ȡһ��id������
struct IdAllocator getIdAllocator(){
  struct IdAllocator out;
  out.size=ID_STRING_NUM;
  out.toReuseSize=ID_STRING_NUM;
  out.toReuseNum=0;
  out.alcSt=0;  //���÷������Ϊ0
  out.toReuse=malloc(sizeof(int)*out.toReuseSize);
  out.useTimes=malloc(sizeof(int)*out.size);
  out.val=malloc(sizeof(char*)*out.size);
  memset(out.val,0,sizeof(char*)*out.size);
  memset(out.useTimes,0,sizeof(int)*out.size);
  return out;
}





//����һ���ַ���,�������ַ�������һ��id���������ɹ�����һ��id(Ϊ�Ǹ���)
int allocateId(IdAlp idAllocator,char* str){
  int id; //��¼�����id
  if(idAllocator->toReuseNum!=0){
    id=idAllocator->toReuse[--(idAllocator->toReuseNum)];
  }else{
    id=(idAllocator->alcSt)++;
  }
  //�ҵ���Ӧid���ڵ�λ��,�����㹻�ռ�
  idAllocator->val[id]=strcpy(malloc(strlen(str)+1),str);
  //����Ƿ���Ҫ���Ż��߷����ռ�
  return id;
}


int increaseIdUseTimes(IdAlp idAllocator,int id,int increaseTimes){
  if(idAllocator->val[id]==NULL){
    return 0;
  }
  idAllocator->val[id]+=increaseTimes;
  return 1;
}

int dropIdUseTimes(IdAlp idAllocator,int id,int dropTimes){
  if(idAllocator->useTimes[id]<dropTimes){
    return 0;
  }else{
    idAllocator->useTimes[id]-=dropTimes;
  }
  //����ǽ�����ַ����������ô�������Ϊ0��id,����ո�id
  if(idAllocator->val[id]==NULL&&idAllocator->useTimes[id]==0){
    idAllocator->toReuse[idAllocator->toReuseNum]=id;
    idAllocator->toReuseNum++;
  }
  //�����Զ��ռ����
  spaceAllocateForIdAllocator(idAllocator);
  return 1;
}

int releaseId(IdAlp idAllocator,int id){
  if(idAllocator->val==NULL||idAllocator->val[id]==NULL&&idAllocator->useTimes[id]==0){
    return 0;
  }
  if(idAllocator->val[id]!=NULL){
    free(idAllocator->val[id]);
    idAllocator->val[id]=NULL;
  }
  idAllocator->useTimes[id]=0;
  idAllocator->toReuse[idAllocator->toReuseNum]=id;
  idAllocator->toReuseNum++;
  spaceAllocateForIdAllocator(idAllocator);
  return 1;
}


int resetIdString(IdAlp idAllocator,int id,char* newStr){
  if(idAllocator->val[id]==NULL||idAllocator->val==NULL){
    return 0;
  }
  free(idAllocator->val[id]);
  idAllocator->val[id]=malloc(strlen(newStr)+1);
  strcpy(idAllocator->val[id],newStr);
  return 1;
}



//��ȡid��Ӧ���ַ���,��ȡ�ɹ����ط���ռ��str,��ȡʧ�ܷ���NULL
char* getIdString(IdAlp idAllocator,int id){
  if(id<0) return NULL;
  if(id>=idAllocator->alcSt||idAllocator->val[id]==NULL||idAllocator->val==NULL) return NULL;
  return strcpy(malloc(strlen(idAllocator->val[id])+1),idAllocator->val[id]);
}

int delString(IdAlp IdAllocator,int id){
  if(id>=IdAllocator->alcSt||IdAllocator->val[id]==NULL) return 0;
  free(IdAllocator->val[id]);
  IdAllocator->val[id]=NULL;
  return 1;
}


int spaceAllocateForIdAllocator(IdAlp idAllocator){
  // void* tmp;
  //���Ҫ����useTimes��val�ռ�
  if(idAllocator->alcSt>ID_ALLOCATOR_EXPAND_COE*idAllocator->size){
    int newSize=idAllocator->size+ID_STRING_NUM;
    char** tmpVal=malloc(newSize*sizeof(char*));
    int* tmpUseTimes=malloc(newSize*sizeof(int));
    memset(tmpVal,0,newSize*sizeof(char*));
    memset(tmpUseTimes,0,newSize*sizeof(int));
    memcpy(tmpVal,idAllocator->val,sizeof(char*)*idAllocator->size);
    memcpy(tmpUseTimes,idAllocator->useTimes,sizeof(int)*idAllocator->size);
    free(idAllocator->val);
    free(idAllocator->useTimes);
    idAllocator->val=tmpVal;
    idAllocator->useTimes=tmpUseTimes;
    idAllocator->size=newSize;
  }
  //���Ҫ��СuseTimes��val�ռ�
  else if(idAllocator->alcSt<ID_ALLOCATOR_SHINK_COE*idAllocator->size&&idAllocator->size>ID_STRING_NUM){
    int newSize=idAllocator->size-ID_STRING_NUM;
    char** tmpVal=malloc(newSize*sizeof(char*));
    int* tmpUseTimes=malloc(newSize*sizeof(int));
    memset(tmpVal,0,newSize*sizeof(char*));
    memset(tmpUseTimes,0,newSize*sizeof(int));
    memcpy(tmpVal,idAllocator->val,newSize*sizeof(char*));
    memcpy(tmpUseTimes,idAllocator->useTimes,newSize*sizeof(int));
    free(idAllocator->val);
    free(idAllocator->useTimes);
    idAllocator->val=tmpVal;
    idAllocator->useTimes=tmpUseTimes;
    idAllocator->size=newSize;
  }
  //���Ҫ����toReuse�ռ�
  if(idAllocator->toReuseNum>ID_ALLOCATOR_EXPAND_COE*idAllocator->toReuseSize){
    int newSize=idAllocator->toReuseSize+ID_STRING_NUM;
    int* tmpU=malloc(sizeof(int)*newSize);
    memset(tmpU,0,sizeof(int)*newSize);
    memcpy(tmpU,idAllocator->toReuse,sizeof(int)*idAllocator->toReuseSize);
    free(idAllocator->toReuse);
    idAllocator->toReuse=tmpU;
    idAllocator->toReuseSize=newSize;
  }
  //���Ҫ��СtoReuse�ռ�
  else if(idAllocator->toReuseNum<ID_ALLOCATOR_SHINK_COE*idAllocator->toReuseSize&&idAllocator->toReuseSize>ID_STRING_NUM){
    int newSize=idAllocator->toReuseSize-ID_STRING_NUM;
    int* tmpU=malloc(sizeof(int)*newSize);
    memset(tmpU,0,sizeof(int)*newSize);
    memcpy(tmpU,idAllocator->toReuse,sizeof(int)*idAllocator->toReuseSize);
    free(idAllocator->toReuse);
    idAllocator->toReuse=tmpU;
    idAllocator->toReuseSize=newSize;
  }
  //
  return 1;
}

//����id������,free��֮��Ͳ��ܹ�ʹ����
int freeIdAllocator(IdAlp idAllocator){
  //���������ַ���
  for(int i=0;i<idAllocator->alcSt;i++){
    if(idAllocator->val[i]==NULL) continue;
    free(idAllocator->val[i]);
  }
  free(idAllocator->val);
  free(idAllocator->toReuse);
  free(idAllocator->useTimes);
  return 1;
}

