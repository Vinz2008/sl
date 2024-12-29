#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>

Token* new_token(enum token_type_t token_type){
    Token* new_token = malloc(sizeof(Token));
    new_token->token_type = token_type;
}

Tokens lex(char* code){
    Tokens tokens = init_list();

    Token* tok1 = new_token(IDENTIFIER);
    Token* tok2 = new_token(MULT_OP);
    Token* tok3 = new_token(DIV_OP);
    list_append(&tokens, tok1);
    list_append(&tokens, tok2);
    list_append(&tokens, tok3);
    FOREACH (tokens, Token, token){
        printf("test %d\n", token->token_type);
    }
    // do the lexing
    return tokens;
}