#ifndef INVERT_FILE_H  
#define INVERT_FILE_H  
#include<stdio.h>  
#include<stdlib.h>  

typedef struct _invertfile_ {  
    unsigned int tablelen;  
    void **table;  
    //unsigned int offset;    
    unsigned int nodecount;  
} if_t;  

typedef struct _word_ {  
    unsigned int id;  
    unsigned int refered;//  
    void *link;  
} word_t;  

typedef struct _word_frequency_ {  
    unsigned int d_id;  
    unsigned int refered;//the num of referenced in the document  
    void *next;  
} wf_t;  

if_t* InvertfileCreate(int length);  
void InvertfileInsert(if_t *h,int w_id,int d_id);  
wf_t* InvertfileSearch(if_t *h,int w_id,int d_id);  
void InvertfileTraverse(if_t *h);  
void InvertfileFree(if_t *h);  
#endif  
