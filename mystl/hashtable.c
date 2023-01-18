#include "hashtable.h"


hashtbl getHashTbl(int cap,int keySize,int valSize,int (*hash)(const void*),int (*keyEq)(const void*,const void*)){
  hashtbl out;
  out.cap=cap;
  out.keySize=keySize;
  out.valSize=valSize;
  out.size=0;
  out.hash=hash;
  out.keyEq=keyEq;
  out.nodes=malloc(sizeof(hashtable_node*)*out.cap);
  memset(out.nodes,0,sizeof(hashtable_node*)*out.cap);
  return out;
}





//����ԭ�������ڶ�Ӧ��key,val�Բ��ܹ�put�ɹ�
//�ɹ����ط�0ֵ��ʧ�ܷ���0
int hashtbl_put(hashtbl* htbl,void* key,void* val){
  int (*keyEq)(const void*,const void*)=htbl->keyEq;
  int (*hash)(const void*)=htbl->hash;

  //���Ȼ�ȡkey��Ӧ���±�
  int index=(hash(key)%htbl->cap+htbl->cap )%htbl->cap;
  //Ȼ������ж϶�Ӧkey�Ƿ����
  //�������нڵ�
  hashtable_node* tn=htbl->nodes[index];
  while (tn!=NULL)
  {
    if(keyEq(key,tn->keyVal.key)) return 0;
    tn=tn->next;
  }
  hashtable_node* add=malloc(sizeof(hashtable_node));
  add->next=htbl->nodes[index];
  if(val!=NULL) add->keyVal.val=malloc(htbl->valSize);
  else add->keyVal.val=NULL;
  add->keyVal.key=malloc(htbl->keySize);
  if(val!=NULL) memcpy(add->keyVal.val,val,htbl->valSize);
  memcpy(add->keyVal.key,key,htbl->keySize);
  htbl->nodes[index]=add;
  htbl->size++;
  return 1;
}


//�滻
int hashtbl_replace(hashtbl* htbl,void* key,void* newVal){
  int (*keyEq)(const void*,const void*)=htbl->keyEq;
  int (*hash)(const void*)=htbl->hash;
  //���Ȼ�ȡkey��Ӧ���±�
  int index=(hash(key)%htbl->cap+htbl->cap )%htbl->cap;
  //Ȼ������ж϶�Ӧkey�Ƿ����
  //�������нڵ�
  hashtable_node* tn=(htbl->nodes[index]);
  while (tn!=NULL)
  {
    if(keyEq(key,tn->keyVal.key)){
      if(tn->keyVal.val==NULL){
        tn->keyVal.val=malloc(htbl->valSize);
      }
      if(newVal!=NULL) memcpy(tn->keyVal.val,newVal,htbl->valSize);
      else return 0;
      return 1;
    }
    tn=tn->next;
  }
  return 0;
}

//��ȡkey��Ӧ��ֵ,д��valRetָ��ָ���ַ
//��ȡ�ɹ����ط�0ֵ��ʧ�ܷ���0
int hashtbl_get(hashtbl* htbl,void* key,void* valRet){
  int (*keyEq)(const void*,const void*)=htbl->keyEq;
  int (*hash)(const void*)=htbl->hash;
  //���Ȼ�ȡkey��Ӧ���±�
  int index=(hash(key)%htbl->cap+htbl->cap )%htbl->cap;
  //Ȼ������ж϶�Ӧkey�Ƿ����
  //�������нڵ�
  hashtable_node* tn=htbl->nodes[index];
  while (tn!=NULL)
  {
    if(keyEq(key,tn->keyVal.key)){
      if(valRet!=NULL) memcpy(valRet,tn->keyVal.val,htbl->valSize);
      return 1;
    }
    tn=tn->next;
  }
  return 0;
}

//ɾ����Ӧ�ļ�,ɾ���ɹ����ط�0ֵ,ɾ��ʧ�ܷ���0
int hashtbl_del(hashtbl* htbl,void* key){
  int (*keyEq)(const void*,const void*)=htbl->keyEq;
  int (*hash)(const void*)=htbl->hash;
  //���Ȼ�ȡkey��Ӧ���±�
  int index=(hash(key)%htbl->cap+htbl->cap )%htbl->cap;
  //Ȼ������ж϶�Ӧkey�Ƿ����
  //�������нڵ�
  hashtable_node* tn=htbl->nodes[index];
  hashtable_node* pre=NULL;
  while (tn!=NULL)
  {
    if(keyEq(key,tn->keyVal.key)) break;
    pre=tn;
    tn=tn->next;
  }
  if(tn==NULL) return 0;  //˵��û���ҵ�Ŀ��ڵ�
  if(pre==NULL) htbl->nodes[index]=tn->next;
  else pre->next=tn->next;
  if(tn->keyVal.val!=NULL) free(tn->keyVal.val);
  free(tn->keyVal.key);
  free(tn);
  htbl->size--;
  return 1;
}


//key,val������
void* hashtbl_toArr(hashtbl* htbl,void* keys,void* vals){
  int j=0;
  int i=0;
  while (i < htbl->size && j < htbl->cap)
  {
    if (htbl->nodes[j] == NULL)
    {
      j++;
      continue;
    }
    hashtable_node *tmp = htbl->nodes[j];
    do
    {
      if (keys != NULL)
        memcpy(keys + i * htbl->keySize, tmp->keyVal.key, htbl->keySize);
      if (vals != NULL)
        if(tmp->keyVal.val!=NULL)
          memcpy(vals + i * htbl->valSize, tmp->keyVal.val, htbl->valSize);
        else
          memset(vals+i*htbl->valSize,0,htbl->valSize);
      i++;
      tmp = tmp->next;
    } while (tmp != NULL);
    j++;
  }
}

//��չ�ϣ��������Ԫ��
void hashtbl_clear(hashtbl* htbl){
  int j=0;
  int i=0;
  while (i < htbl->size && j < htbl->cap)
  {
    if (htbl->nodes[j] == NULL)
    {
      j++;
      continue;
    }
    hashtable_node *tmp = htbl->nodes[j];
    do
    {
      htbl->nodes[j]=tmp->next;
      if(tmp->keyVal.val!=NULL) free(tmp->keyVal.val);
      free(tmp->keyVal.key);
      free(tmp);
      i++;
      tmp = htbl->nodes[j];
    } while (tmp != NULL);
    j++;
  }
  htbl->size=0;
}

//�ͷŹ�ϣ��ռ䣬��������������ϣ��Ӧ����ʹ��
void hashtbl_release(hashtbl* htbl){
  int j=0;
  int i=0;
  while (i < htbl->size && j < htbl->cap)
  {
    if (htbl->nodes[j] == NULL)
    {
      j++;
      continue;
    }
    hashtable_node *tmp = htbl->nodes[j];
    do
    {
      htbl->nodes[j]=tmp->next;
      free(tmp->keyVal.val);
      free(tmp->keyVal.key);
      free(tmp);
      i++;
      tmp = htbl->nodes[j];
    } while (tmp != NULL);
    j++;
  }
  free(htbl->nodes);
  htbl->size=0;
}
