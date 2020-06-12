#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<list.h>
#include<lexer.h>
#include<fs.h>


int main(void) {
    const char * fileName = "../main.dpp";
    int len = getFileSize(fileName);
    char input[len + 1];
    readFileToBuffer(fileName, input, len + 1);
    input[len] = '\0';

    List *list = List_new();

    if (0 != Lexer_splitTokens(input, list)) { 
        Lexer_clearTokens(list);
        List_free(list);
        return EXIT_FAILURE;
    }

    FILE * filestreamForWrite = fopen("../main_tokens.txt", "w");
    Lexer_printTokens(list,filestreamForWrite);
    fclose(filestreamForWrite);


    Lexer_clearTokens(list);
    List_free(list);

    return 0;
}


