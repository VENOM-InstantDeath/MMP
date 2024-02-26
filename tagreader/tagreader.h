#ifndef TAGREADER_H
#define TAGREADER_H
#include <stddef.h>

typedef struct {
    char* TIT2; // title
    char* TPE1; // artist
    char* TALB; // album
    char* TYER; // year
    char* TCON; // genre
} Id3v2Tag;

Id3v2Tag tagreader_read_tags(char* path);
#endif
