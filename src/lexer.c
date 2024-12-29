#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>

Tokens lex(char* code){
    Tokens tokens = init_list();

    Token* new_token = malloc(sizeof(Token));
    new_token->token_type = 0;
    list_append(&tokens, new_token);
    list_append(&tokens, new_token);
    list_append(&tokens, new_token);
    FOREACH (token, tokens, Token){
        printf("test %d\n", token->token_type);
    }
    // do the lexing
    return tokens;
}