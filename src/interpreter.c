#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"

int interpret_code(char* code){
    Tokens tokens = lex(code);
}

char* read_file(FILE* file){
    fseek(file, 0L, SEEK_END);
    int file_size = ftell(file);
    rewind(file);
    char* buf = malloc(sizeof(char) * file_size);
    fread(buf, file_size, file_size, file);
    return buf;
}

int interpret_file(char* filename){
    FILE* file = fopen(filename, "r");
    char* content = read_file(file);
    printf("%s\n", content);
    fclose(file);
    int ret = interpret_code(content);
    free(content);
    if (ret == -1){
        return -1;
    }
    return 0;
}


int interpret_repl(){
    printf("not implemented");
    exit(1);
    // TODO (use interpret_code)
}