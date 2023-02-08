#include "vector.h"

//获得一个元素大小为valSize的vector
vector getVector(int valSize){
  vector out;
  out.capacity=0;
  out.vals=NULL;
  out.size=0;
  out.valSize=valSize;
  return out;
}

//往vector末尾压入元素
void vector_push_back(vector* vec,void* val){
  //先进行放缩检查,再压入元素
  vector_resize(vec);
  vec->vals[vec->size].val=malloc(vec->valSize);
  memcpy(vec->vals[vec->size].val,val,vec->valSize);
  vec->size++;
}


void vector_get(vector* vec,int index,void* dir){
  //下标检查,输出提示
  if(index<0){
    printf("index less than 0 in vector_get\n");
    return;
  }
  if(dir!=NULL&&index<(vec->size)&&index>=0){
    memcpy(dir,vec->vals[index].val,vec->valSize);
  }
  else if(dir!=NULL) memset(dir,0,vec->valSize);
}

//设置vector对应位置的内容,如果对应位置原本有内容,会通过old指针返回,删除失败返回NULL
void vector_set(vector* vec,int index,void* val,void* old){
  if(index>=vec->size) return;
  vector_node* tvn=&vec->vals[index];
  if(old!=NULL) memcpy(old,tvn->val,vec->valSize);
  if(val!=NULL)
    memcpy(tvn->val,val,vec->valSize);
}


//删除元素,通过del指针返回删去的内容,如果删除失败,返回NULL
void vector_del(vector* vec,int index,void* del){
  if(index>=vec->size) return;
  vector_resize(vec); //先进行放缩检查,再进行删除
  if(del!=NULL) memcpy(del,vec->vals[index].val,vec->valSize);

  //把它后面的元素顺序往前推,然后删掉最后一个
  for(int i=index;i<vec->size-1;i++){
    memcpy(vec->vals[i].val,vec->vals[i+1].val,vec->size);
  }
  free(vec->vals[vec->size-1].val);
  vec->vals[vec->size-1].val=NULL;
  vec->size--;
}

//对vector进行放缩检查
void vector_resize(vector* vec){
  if(vec->vals==NULL){
    vec->vals=malloc(sizeof(vector_node)*VECTOR_ARR_BASE_SIZE);
    vec->capacity=VECTOR_ARR_BASE_SIZE;
    return;
  }
  //如果要进行扩大
  else if(vec->size>=VECTOR_EXPAND_LIMIT*vec->capacity){
    int newCap=vec->capacity*VECTOR_EXPAND_COE;
    vector_node* newVals=malloc(sizeof(vector_node)*newCap);
    memcpy(newVals,vec->vals,vec->size*sizeof(vector_node));
    free(vec->vals);
    vec->vals=newVals;
    vec->capacity=newCap;
  }
  //如果要进行缩小
  else if(vec->size<=VECTOR_SHRINK_LIMIT*vec->capacity&&vec->capacity>VECTOR_ARR_BASE_SIZE){
    int newCap=vec->capacity*VECTOR_SHRINK_COE;
    vector_node* newVals=malloc(sizeof(vector_node)*newCap);
    memcpy(newVals,vec->vals,vec->size*sizeof(vector_node));
    free(vec->vals);
    vec->vals=newVals;
    vec->capacity=newCap;
  }
}


//清空元素
void vector_clear(vector* vec){
  if(vec->vals==NULL) return;
  for(int i=0;i<vec->size;i++) free(vec->vals[i].val);
  free(vec->vals);
  vec->vals=NULL;
  vec->size=0;
  vec->capacity=0;
}


//把值转为二维数组返回,返回的数组里的值是实际值的浅拷贝
void* vector_toArr(vector* vec){
  void* out=NULL;
  out=malloc(vec->valSize*vec->size);
  for(int i=0;i<vec->size;i++){
    memcpy(out+vec->valSize*i,vec->vals[i].val,vec->valSize);
  }
  return out;
}
