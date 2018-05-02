#include "FileIndex.h"
#define MAX_LENGTH 1024
#define IS_PLAIN_TEXT 1

void ListFile(const char *name) {
    DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir(name)))
        exit(-1);

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            char path[MAX_LENGTH];
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);
            //printf("%*s[%s]\n", indent, "", entry->d_name);
            ListFile(path);
        } 
        else {
            //printf(" %s\n", entry->d_name);
            char absolute_path[MAX_LENGTH];
            sprintf(absolute_path, "%s/%s", name, entry->d_name);
            //if (FileType(absolute_path) == IS_PLAIN_TEXT) {
                //printf("%s\n", entry->d_name);
            OpenFile(absolute_path);
            //}
        }
    }
    closedir(dir);
}
/*
redisContext* Index = NULL;

void RedisSetup(char * comword) {
    char *command1 = malloc(sizeof(char) * 512);
    sprintf(command1, "sadd dictionary %s", comword);
    redisReply* r = (redisReply*)redisCommand(Index,command1);
    free(command1);
    freeReplyObject(r);
}
*/
int main(int argc, char * argv[]) {
/*
     Index = redisConnect("127.0.0.1", 6379);

	if ( Index->err) {
		redisFree(Index);
		printf("Connect to redisServer fail\n");
		return 0;
	}
	printf("Connect to redisServer Success\n");
*/
    ListFile("/home/jyxk/Code");
    //redisReply * reply = redisCommand(Index, "smembers dictionary");
    //printf("%d", reply->len);
   // freeReplyObject(reply);
    //redisFree(Index);
    
    
    return 0;
}