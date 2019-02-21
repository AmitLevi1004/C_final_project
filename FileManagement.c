#include <stdlib.h>
#include "FileManagement.h"


fileInfo* OpenOrCreateFile(char* fileName, char* mode) {

    fileInfo* file = (fileInfo*)malloc(sizeof(fileInfo));
    file->fd = fopen(fileName , mode);

    if(file->fd == NULL) {
        perror("Error opening file");
        return NULL;
    }

    return file;

}

void ReadFromFile (fileInfo* fileInfo) {
    if( fgets (fileInfo->currentLineData, LINE_SIZE, fileInfo->fd) == NULL) {
        /*exit(-1);*/
    }
}

void WriteToFile (fileInfo* fileInfo) {
    if (fputs(fileInfo->currentLineData, fileInfo->fd) < 0) {
        exit(-1);
    }
}