#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sstream>
#include <unordered_map>
#include <magic.h>
#include <string.h>
#include "inverted_index.h"

#define IS_PLAIN_TEXT 1
#define MAX_LENGTH 1024
#define NONE                 "\e[0m"
#define L_RED                "\e[1;31m"
#define L_GREEN              "\e[1;32m"

using namespace std;

InvertedIndex* inverted_index = new InvertedIndex();

int main(int argc, char const *argv[]) {

    const char *file_path = argv[1];
    const char *target_file[argc];

    for (int i = 2; i < argc; i++) 
        target_file[i-2] = argv[i];
    
    if ((file_path == NULL) || (target_file == NULL)) {
        printf(L_RED"Wrong Input!\n"NONE);
        exit(-1);
    }

    ListFile(file_path);

    unordered_map<string,unordered_map<string,int>> result = inverted_index->result_index();
    unordered_map<string,unordered_map<string,int>>::iterator it;

    for (it = result.begin(); it != result.end(); ++it) {
        for (int i = 0; i < argc-2; i++)
            if ( strcmp((it->first).c_str(), target_file[i]) == 0 ) {
                printf(L_GREEN"%s:"NONE, (it->first).c_str());
                unordered_map<string,int> file_count = it->second;
                unordered_map<string,int>::iterator file_it;
                for (file_it = file_count.begin(); file_it != file_count.end(); ++file_it) {
                    printf(L_GREEN"["NONE"%s,%d"L_GREEN"]\n\t"NONE, (file_it->first).c_str(), file_it->second);
                }
                printf("\n");
            }
    }
	return 0;
}


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
            ListFile(path);
        } 
        else {
            char absolute_path[MAX_LENGTH];
            sprintf(absolute_path, "%s/%s", name, entry->d_name);
            if (FileType(absolute_path) == IS_PLAIN_TEXT) {
                //printf("%s\n", entry->d_name);
                //OpenFile(absolute_path);
                inverted_index->StatWords(absolute_path);
                printf("file stat success: %s\n", absolute_path);
            }
        }
    }
    closedir(dir);
}

int FileType(const char *file_name) {
    const char *mime = NULL;
    struct magic_set *magic = NULL;
    int is_plain_text = 0;
    //printf("Getting magic from %s\n", file_name);

    magic = magic_open(MAGIC_MIME_TYPE); 
    magic_load(magic, NULL);
    //magic_compile(magic, NULL);
    mime = magic_file(magic, file_name);

    if ( (mime != NULL) && strcmp(mime, "text/x-c") == 0 ) 
        is_plain_text = 1;

    //printf("%s %s\n", file_name, mime);
    //if (magic != NULL)
    magic_close(magic);
    magic = NULL;
    
    return is_plain_text;
}