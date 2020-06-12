#include<stdio.h>
#include<fs.h>

long getFileSize(const char * fileName) {
    FILE *f = fopen(fileName, "rb");
    if (!f) return -1;
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fclose(f);
    return fsize;
}

int readFileToBuffer(const char * fileName, char * buffer, int bufferLength) {
    FILE *f = fopen(fileName, "rb");
    if (!f) return 0;
    long readBytes = fread(buffer, 1, bufferLength, f);
    fclose(f);
    return readBytes;
}