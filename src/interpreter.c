#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"

// for now, the args can only be char*, so it will contain that
// TODO : create a Value struct
 
void call_function(char* function_name, list_t args){
    printf("calling function\n");
    printf("is print : %d\n", strcmp(function_name, "print"));
    if (strcmp(function_name, "print") == 0){
        fprintf(stderr, "%s\n", (char*)args.elements[0]);
        printf("after printing");
    } else {
        fprintf(stderr, "Unknown function\n");
        exit(1);
    }
}

int interpret_code(char* code){
    Tokens tokens = lex(code);
    FileAST fileAst = parse(tokens);
    
    return 0;
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
    printf("not implemented\n");
    exit(1);
    // TODO (use interpret_code)
}