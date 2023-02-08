#include "vector.h"

//���һ��Ԫ�ش�СΪvalSize��vector
vector getVector(int valSize){
  vector out;
  out.capacity=0;
  out.vals=NULL;
  out.size=0;
  out.valSize=valSize;
  return out;
}

//��vectorĩβѹ��Ԫ��
void vector_push_back(vector* vec,void* val){
  //�Ƚ��з������,��ѹ��Ԫ��
  vector_resize(vec);
  vec->vals[vec->size].val=malloc(vec->valSize);
  memcpy(vec->vals[vec->size].val,val,vec->valSize);
  vec->size++;
}


void vector_get(vector* vec,int index,void* dir){
  //�±���,�����ʾ
  if(index<0){
    printf("index less than 0 in vector_get\n");
    return;
  }
  if(dir!=NULL&&index<(vec->size)&&index>=0){
    memcpy(dir,vec->vals[index].val,vec->valSize);
  }
  else if(dir!=NULL) memset(dir,0,vec->valSize);
}

//����vector��Ӧλ�õ�����,�����Ӧλ��ԭ��������,��ͨ��oldָ�뷵��,ɾ��ʧ�ܷ���NULL
void vector_set(vector* vec,int index,void* val,void* old){
  if(index>=vec->size) return;
  vector_node* tvn=&vec->vals[index];
  if(old!=NULL) memcpy(old,tvn->val,vec->valSize);
  if(val!=NULL)
    memcpy(tvn->val,val,vec->valSize);
}


//ɾ��Ԫ��,ͨ��delָ�뷵��ɾȥ������,���ɾ��ʧ��,����NULL
void vector_del(vector* vec,int index,void* del){
  if(index>=vec->size) return;
  vector_resize(vec); //�Ƚ��з������,�ٽ���ɾ��
  if(del!=NULL) memcpy(del,vec->vals[index].val,vec->valSize);

  //���������Ԫ��˳����ǰ��,Ȼ��ɾ�����һ��
  for(int i=index;i<vec->size-1;i++){
    memcpy(vec->vals[i].val,vec->vals[i+1].val,vec->size);
  }
  free(vec->vals[vec->size-1].val);
  vec->vals[vec->size-1].val=NULL;
  vec->size--;
}

//��vector���з������
void vector_resize(vector* vec){
  if(vec->vals==NULL){
    vec->vals=malloc(sizeof(vector_node)*VECTOR_ARR_BASE_SIZE);
    vec->capacity=VECTOR_ARR_BASE_SIZE;
    return;
  }
  //���Ҫ��������
  else if(vec->size>=VECTOR_EXPAND_LIMIT*vec->capacity){
    int newCap=vec->capacity*VECTOR_EXPAND_COE;
    vector_node* newVals=malloc(sizeof(vector_node)*newCap);
    memcpy(newVals,vec->vals,vec->size*sizeof(vector_node));
    free(vec->vals);
    vec->vals=newVals;
    vec->capacity=newCap;
  }
  //���Ҫ������С
  else if(vec->size<=VECTOR_SHRINK_LIMIT*vec->capacity&&vec->capacity>VECTOR_ARR_BASE_SIZE){
    int newCap=vec->capacity*VECTOR_SHRINK_COE;
    vector_node* newVals=malloc(sizeof(vector_node)*newCap);
    memcpy(newVals,vec->vals,vec->size*sizeof(vector_node));
    free(vec->vals);
    vec->vals=newVals;
    vec->capacity=newCap;
  }
}


//���Ԫ��
void vector_clear(vector* vec){
  if(vec->vals==NULL) return;
  for(int i=0;i<vec->size;i++) free(vec->vals[i].val);
  free(vec->vals);
  vec->vals=NULL;
  vec->size=0;
  vec->capacity=0;
}


//��ֵתΪ��ά���鷵��,���ص��������ֵ��ʵ��ֵ��ǳ����
void* vector_toArr(vector* vec){
  void* out=NULL;
  out=malloc(vec->valSize*vec->size);
  for(int i=0;i<vec->size;i++){
    memcpy(out+vec->valSize*i,vec->vals[i].val,vec->valSize);
  }
  return out;
}
