#include "hashset.h"


//创建一个基础大小的哈希数组
HSet hashset_cre(int valSize){
    HSet out;
    out.valSize=valSize;
    memset(out.nodes,0,sizeof(out.nodes));
    return out;
}


//往哈希表中加入元素,如果已经存在的元素则不加入；加入成功返回非0，加入失败返回0
int hashset_add(HSetp hashset,void* toAdd){
    int index=hash(toAdd);
    if(hashset->nodes[index]==NULL){
        hashset->nodes[index]=(HashNode*)malloc(sizeof(HashNode));
        hashset->nodes[index]->next=NULL;
        memcpy(hashset->nodes[index]->val,toAdd,hashset->valSize);
    }else{
        HashNode* cur;
        HashNode f;
        f.next=hashset->nodes[index];
        cur=&f;
        while(cur->next!=NULL){
            if(memcmp(cur->next->val,toAdd,hashset->valSize)==0) break;
            cur=cur->next;
        }
        //如果该位置遍历检查后发现没有toAdd元素，则加入toAdd元素
        if(cur->next==NULL){
            cur->next=(HashNode*)malloc(sizeof(HashNode));
            cur->next->next=NULL;
            memcpy(cur->next->val,toAdd,hashset->valSize);
        }
        //否则不加入,
        else{
            //哈希表中元素已存在，插入失败，返回0
            return 0;
        }
    }
    //插入成功返回非0值
    return -1;
}


//在哈希表中查找一个元素,如果存在返回非0值，如果不存在，返回0
int hashset_contains(HSetp hashset,void* toFind){
    int index=hash(toFind);
    if(hashset->nodes[index]!=NULL){
        HashNode* cur;
        HashNode f;
        f.next=hashset->nodes[index];
        cur=&f;
        while(cur->next!=NULL){
            if(memcmp(cur->next->val,toFind,hashset->valSize)==0) break;
            cur=cur->next;
        }
        //如果该位置遍历检查后发现已经存在toAdd元素
        if(cur->next!=NULL){
            return -1;
        }
    }
    //如果该index位置链表起点为NULL,或者链表遍历后发现没有toAdd元素，则说明哈希表中不存在该元素
    return 0;
}

//从哈希表中删去一个元素,删除成功返回-1，删除失败返回0
int hashset_remove(HSetp hashset,void* toDel){
    int index=hash(toDel);
    if(hashset->nodes[index]!=NULL){
        HashNode* cur;
        HashNode f;
        f.next=hashset->nodes[index];
        cur=&f;
        while(cur->next!=NULL){
            if(memcmp(cur->next->val,toDel,hashset->valSize)==0) break;
            cur=cur->next;
        }
        //如果该位置遍历检查后找到toDel元素，则进行删除操作
        if(cur->next!=NULL){
            if(cur->next==hashset->nodes[index]){
                hashset->nodes[index]=cur->next->next;
                free(cur->next);
            }else{
                HashNode* t_node=cur->next;
                cur->next=cur->next->next;
                free(t_node);
            }
            return -1;  //删除成功返回非0值
        }
    }
    //删除失败返回0
    return 0;
}

//销毁哈希表
void hashset_del(HSetp hashset){
    int size=sizeof(hashset->nodes)/sizeof(hashset->nodes[0]);
    for(int i=0;i<size;i++){
        if(hashset->nodes[i]==NULL) continue;
        HashNode* cur;
        while(hashset->nodes[i]!=NULL){
            cur=hashset->nodes[i];
            hashset->nodes[i]=cur->next;
            free(cur);
        }
    }
}

//使用的哈希函数,根据值使用的哈希函数，使用的是数组的值的前面4个字节，
//如果对小于int大小的数据使用哈希，需要修改hash函数，避免访问越界
int hash(void* value){
    int* int1=(int*)value;
    return (*int1)%HASHSET_DEFAULT_ARRLEN;
}