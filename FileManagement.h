#ifndef OPENUTASK14_READFROMFILE_H
#define OPENUTASK14_READFROMFILE_H

#include <stdio.h>

#define LINE_SIZE 80

typedef struct fileInformation {
    FILE *fd;
    char currentLineData[LINE_SIZE];
} fileInfo;

fileInfo* OpenOrCreateFile(char* fileName, char* mode);
void ReadFromFile (fileInfo* fileInfo);
void WriteToFile (fileInfo* fileInfo);

#endif
