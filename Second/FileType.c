#include <stdio.h>
#include <stdlib.h>
#include <magic.h>
#include <string.h>

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

    printf("%s %s\n", file_name, mime);
    //if (magic != NULL)
        magic_close(magic);
    magic = NULL;
    
    return is_plain_text;
}

int IsPlainText(const char * buffer, const int size) {
    for (int i = 0; i < size; i++) {
        if (buffer[i]<=0 && buffer[i]>=127)
            return 0;
    }
    return 1;
}