#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "string.h"

Token* new_token(enum token_type_t token_type){
    Token* new_token = malloc(sizeof(Token));
    new_token->token_type = token_type;
    return new_token;
}

// TODO : create functions to create specific tokens (ex : new_token_nb(enum token_type_t token_type, double nb) )

static const char* const token_strings[] = {
    [IDENTIFIER] = "identifier",
    [PLUS_OP] = "+",
    [MINUS_OP] = "-",
    [MULT_OP] = "*",
    [DIV_OP] = "/",
    [OPEN_PARENTHESIS] = "(",
    [CLOSE_PARENTHESIS] = ")",
    [STRING] = "string",
    [NUMBER] = "number",
};


const char* const token_type_to_string(enum token_type_t token_type){
    return token_strings[token_type];
}

Tokens lex(char* code){
    Tokens tokens = init_list();
    
    int i = 0;
    int code_len = strlen(code);
    while (i < code_len){
        Token* new_tok = NULL;
        while (i < code_len && isspace(code[i])){
            i++;
        }
        if (isdigit(code[i])){
            new_tok = new_token(NUMBER);
            string_t nb_str = init_string();
            string_append(&nb_str, code[i]);
            i++;
            while (i < code_len && isdigit(code[i])){
                string_append(&nb_str, code[i]);
                i++;
            }
            long nb = strtol(nb_str.str, NULL, 10);
            string_destroy(nb_str);
            new_tok->token_content.nb = nb;
        } else if (isalnum(code[i])){
            new_tok = new_token(IDENTIFIER);
            string_t identifier_str = init_string();
            string_append(&identifier_str, code[i]);
            i++;
            while (i < code_len && isalnum(code[i])){
                string_append(&identifier_str, code[i]);
                i++;
            }
            new_tok->token_content.identifier = identifier_str.str;
        } else if (code[i] == '\"'){
            new_tok = new_token(STRING);
            string_t str = init_string();
            i++;
            while (i < code_len && code[i] != '\"'){
                string_append(&str, code[i]);
                i++;
            }
            i++;
            new_tok->token_content.str = str.str;
            
        } else if (code[i] == '('){
            new_tok = new_token(OPEN_PARENTHESIS);
            i++;
        } else if (code[i] == ')'){
            new_tok = new_token(CLOSE_PARENTHESIS);
            i++;
        } else {
            fprintf(stderr, "unexpected char '%c' while lexing\n", code[i]);
            exit(1);
        }

        if (new_tok){
            list_append(&tokens, new_tok);
        }
    }    
    
    FOREACH (tokens, Token, token){
        printf("token type %s\n", token_type_to_string(token->token_type));
    }

    return tokens;
}