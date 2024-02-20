#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "tagreader.h"

void _append_tag(Id3v2Tag* Tag, char* tagname, char* value) {
    if (!strcmp(tagname, "TIT2")) Tag->TIT2=value;
    else if (!strcmp(tagname, "TPE1")) Tag->TPE1=value;
    else if (!strcmp(tagname, "TALB")) Tag->TALB=value;
    else if (!strcmp(tagname, "TYER")) Tag->TYER=value;
    else if (!strcmp(tagname, "TCON")) Tag->TCON=value;
}

Id3v2Tag tagreader_tagstruct_init() {
    Id3v2Tag Tags = {NULL, NULL, NULL, NULL, NULL, 0};
    return Tags;
}

Id3v2Tag tagreader_read_tags(char* path) {
    Id3v2Tag Tags = tagreader_tagstruct_init();
    char tagname[5] = {0};
    char tagsize[1] = {0};
    FILE* F = fopen(path, "rb");
    //char *buff = malloc(st.st_size);
    fread(tagname, 1, 3, F);
    if (strcmp(tagname, "ID3")) return Tags;
    fseek(F, 10, SEEK_SET);
    int P = 10;
    for (;;) {
        memset(tagname, 0, 5); tagsize[0]=0;
        fread(tagname, 1, 4, F);
        fseek(F, 3, SEEK_CUR);fread(tagsize, 1, 1, F);
        char size = tagsize[0];
        if (!size) break;
        fseek(F, 3, SEEK_CUR);
        char* value = calloc(size, 1);
        fread(value, 1, size-1, F);
        _append_tag(&Tags, tagname, value);
    }

    return Tags;
}