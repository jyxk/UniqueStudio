#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <hiredis/hiredis.h>

char * CutWords(const char *page,int start,int end);
void GetWord(const char word[],const char *page);
int OpenFile(const char *filename);
int FileType(const char *file_name);
void ListFile(const char *name);
void RedisSetup(char * comword);
int IsPlainText(const char * buffer, const int size);
extern redisContext * Index;