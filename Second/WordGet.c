#include "FileIndex.h"


int dictionary_setup = 0;

char * CutWords(const char *page,int start,int end){
    char *word = malloc(sizeof(char)*(end-start));
    int pos = 0;
    if (start >= end)
        return NULL;

    for (int i = start; i <= end; i++) {
        word[pos++]=page[i];
    }
    word[pos] = '\0';
    return word;
}

void GetWord(const char word[], const char *page) {
    int start=0,end = 0,count = 0,flag=0;
    char *comword;
    int line = 1;
    for (int i = 0; i < strlen(page); i++) {
        if (flag == 0) {
            if (((page[i]==' ' ) && (page[i+1]!=' '))) {
                start = i+1;
                flag = 1;
            }
            if(page[i]=='\n' && page[i+1]!='\n') {
                start = i+1;
                flag = 1;
            }
            else if (i==0 && (page[0]!=' ')) {
                start = i;
                flag = 1;
            }
        }
        else if (flag == 1) {
            if ((page[i]==' '||page[i]=='\n') && (page[i+1]!=' ' || page[i+1]!='\n')) {
                i--;                
                end = i;
                flag = 2;
            }
            else if (i==strlen(page)-1) {
                end = i;
                flag = 2;
            }
        }
        if (flag==2) {
            for (int j = 0; j < end; j++)
                if (page[j] == '\n')
                    line++;

            comword = CutWords(page,start,end);

            //RedisSetup(comword);
    
        //if ((comword != NULL) && (strcmp(word,comword)==0))
            printf("%sin Line %d\n",comword, line);

            if (comword != NULL) {
                free(comword);
                comword = NULL;
            }

        flag = 0;
        line = 1;
/*
            if ((comword != NULL) && strcmp(word,comword)==0) 
                count++;
 */ 

        }
    }
    //printf("the frequency is %d\n",count);
}

int OpenFile(const char *filename) {

    int fd;
    char *buffer = NULL;
    unsigned long filesize = -1;
    struct stat statbuff;

    fd = open(filename, O_RDONLY);

    if (fd == -1)
        return (-1);
    
    if(stat(filename, &statbuff) >= 0){    
        filesize = statbuff.st_size;  
    }   
    else
        return 0;
        
    if (buffer == NULL)
        buffer = (char *)malloc((sizeof(char))*filesize+1);
    else
        exit(1);
    
    for (int i = 0; i <= filesize; i++)
        buffer[i] = 0;

    read(fd, buffer, filesize);
        //printf("%s", buffer);
    if (IsPlainText(buffer, filesize))
        GetWord("abc", buffer);

    if (buffer != NULL) {
        free(buffer);
        buffer = NULL;
    }
    else
        exit(1);

    close(fd);

    return 0;
}

/*int main(void) {//pass test
    OpenFile("/home/jyxk/Code/FreshMan/Second/testemp");
    return 0;
}
*/
