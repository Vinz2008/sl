#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"

#define NextToken() ((Token*)tokens.elements[tok_pos+1])


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
    int tokens_passed = 0;
    int tok_pos = 0;
    // TODO : move this into a parser.c file
    Token* token = NULL;
    while (tok_pos < tokens.size){
        token = (Token*)tokens.elements[tok_pos];
        if (token->token_type == IDENTIFIER){
            printf("found identifier\n");
            char* identifier = token->token_content.identifier;
            if (tok_pos + 1 < tokens.size && NextToken()->token_type == OPEN_PARENTHESIS){
                printf("found open parenthesis\n");
                printf("function name : %s\n", identifier);
                for (int i = 0 ; i < strlen(identifier); i++){
                    printf("function name %s[%d] : %c %d\n", identifier, i, identifier[i], identifier[i]);
                }
                tok_pos++;
                if (tok_pos + 1 < tokens.size && NextToken()->token_type == STRING){
                    printf("found string\n");
                    char* str = NextToken()->token_content.str;
                    printf("arg first : %s\n", str);
                    list_t args = init_list();
                    list_append(&args, str);
                    call_function(identifier, args);
                }
            }
        } else {
            fprintf(stderr, "BUG : unknown token when parsing\n");
            exit(1);
        }
    }

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