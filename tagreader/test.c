#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include "tagreader.h"
#include "../libvector/vector.h"

int endswith(char* string, char* p) {
	char* tmp = strrchr(string, '.');
	if (tmp && !strcmp(tmp, p)) return 1;
	return 0;
}

int main() {
    char* path = "/home/xp/music/";
    DIR* dir = opendir(path);
    struct dirent* dt;
    while ((dt=readdir(dir))) {
        if (!endswith(dt->d_name, ".mp3")) continue;
        string S = string_init(path);
        string_append(&S, dt->d_name);
        Id3v2Tag Tags = tagreader_read_tags(string_get_c_str(&S));
        printf("%s\n", dt->d_name);
        printf("\tTIT2: %s\n", Tags.TIT2);
        printf("\tTPE1: %s\n", Tags.TPE1);
        printf("\tTALB: %s\n", Tags.TALB);
        printf("\tTYER: %s\n", Tags.TYER);
        printf("\tTCON: %s\n", Tags.TCON);
        free(S.c_str);
    }
    return 0;
}