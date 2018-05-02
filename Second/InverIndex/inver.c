#include "invert.h"  

if_t* InvertfileCreate(int length) {  
    if_t *h;  

    h = (if_t *)calloc(1,sizeof(if_t));  
    if (NULL == h) 
        return NULL;       

    h->table =(void **)calloc(length,sizeof(void *));  
    h->tablelen=length;  
    h->nodecount=0;  
    word_t *w;  
    for (int i=0;i<length;i++) {  
        h->table[i]=malloc(sizeof(word_t));  
        w=(word_t*)h->table[i];  
        w->id=i;  
        w->refered=0;  
        w->link=NULL;  
    }  
    return h;  
}  
//check if document d_id have word w_id  
wf_t*InvertfileSearch (if_t *h,int w_id,int d_id) {  
    word_t *w;  
    wf_t    *wf;  
    w=(word_t*)h->table[w_id];  
    if (w->refered>0) {  
        wf=(wf_t*)w->link;  
        while (wf) {  
            if(wf->d_id==d_id)return wf;  
            wf=(wf_t*)wf->next;  
        }  
    }  
    return NULL;  
}  
void InvertfileInsert(if_t *h,int w_id,int d_id){  
    word_t * w=(word_t*)h->table[w_id];  
    wf_t * wf;  
    if ((wf=InvertfileSearch(h,w_id,d_id))!=NULL) {  
        wf->refered++;  
    }  
    else {  
        wf=(wf_t *)malloc(sizeof(wf_t));  
        wf->next=w->link;  
        w->link=wf;  
        w->refered++;  
        wf->refered++;  
        wf->d_id=d_id;  
        h->nodecount++;  
    }  
}  
void InvertfileFree(if_t *h){  
    word_t *w;  
    wf_t* wf,*cur;  
    for(int i=0;i<h->tablelen;i++){  
        w=(word_t*)h->table[i];  
        wf=(wf_t*)w->link;  
        while(wf!=NULL){  
            cur=wf;  
            wf=(wf_t*)wf->next;  
            free(cur);  
        }  
        free(w);  
    }  
    free(h->table);  
}  
void InvertfileTraverse(if_t *h){  
    word_t *w;  
    wf_t* wf,*cur;  
    for(int i=0;i<h->tablelen;i++){  
        w=(word_t*)h->table[i];  
        wf=(wf_t*)w->link;  
        printf("word_id:%d;",w->id);  
        while(wf!=NULL){  
            cur=wf;  
            wf=(wf_t*)wf->next;  
            printf("d_id:%d,freq:%d;",cur->d_id,cur->refered);  
        }  
        printf("\n");  
    }  
}  