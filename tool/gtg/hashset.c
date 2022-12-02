#include "hashset.h"


//����һ��������С�Ĺ�ϣ����
HSet hashset_cre(int valSize){
    HSet out;
    out.valSize=valSize;
    memset(out.nodes,0,sizeof(out.nodes));
    return out;
}


//����ϣ���м���Ԫ��,����Ѿ����ڵ�Ԫ���򲻼��룻����ɹ����ط�0������ʧ�ܷ���0
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
        //�����λ�ñ���������û��toAddԪ�أ������toAddԪ��
        if(cur->next==NULL){
            cur->next=(HashNode*)malloc(sizeof(HashNode));
            cur->next->next=NULL;
            memcpy(cur->next->val,toAdd,hashset->valSize);
        }
        //���򲻼���,
        else{
            //��ϣ����Ԫ���Ѵ��ڣ�����ʧ�ܣ�����0
            return 0;
        }
    }
    //����ɹ����ط�0ֵ
    return -1;
}


//�ڹ�ϣ���в���һ��Ԫ��,������ڷ��ط�0ֵ����������ڣ�����0
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
        //�����λ�ñ����������Ѿ�����toAddԪ��
        if(cur->next!=NULL){
            return -1;
        }
    }
    //�����indexλ���������ΪNULL,���������������û��toAddԪ�أ���˵����ϣ���в����ڸ�Ԫ��
    return 0;
}

//�ӹ�ϣ����ɾȥһ��Ԫ��,ɾ���ɹ�����-1��ɾ��ʧ�ܷ���0
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
        //�����λ�ñ��������ҵ�toDelԪ�أ������ɾ������
        if(cur->next!=NULL){
            if(cur->next==hashset->nodes[index]){
                hashset->nodes[index]=cur->next->next;
                free(cur->next);
            }else{
                HashNode* t_node=cur->next;
                cur->next=cur->next->next;
                free(t_node);
            }
            return -1;  //ɾ���ɹ����ط�0ֵ
        }
    }
    //ɾ��ʧ�ܷ���0
    return 0;
}

//���ٹ�ϣ��
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

//ʹ�õĹ�ϣ����,����ֵʹ�õĹ�ϣ������ʹ�õ��������ֵ��ǰ��4���ֽڣ�
//�����С��int��С������ʹ�ù�ϣ����Ҫ�޸�hash�������������Խ��
int hash(void* value){
    int* int1=(int*)value;
    return (*int1)%HASHSET_DEFAULT_ARRLEN;
}