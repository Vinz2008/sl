#include "list.h"


// after changing this, update the token_strings list in lexer.c
enum token_type_t {
    IDENTIFIER,
    PLUS_OP,
    MINUS_OP,
    MULT_OP,
    DIV_OP,
    OPEN_PARENTHESIS,
    CLOSE_PARENTHESIS,
    STRING,
    NUMBER,
    TOK_NB,
};

typedef struct {
    enum token_type_t token_type;
    union {
        char*  str;
        // TODO : support doubles
        // TODO : create a struct for number that will be either an int or a float (or a bigint ?)
        long nb; 
        char* identifier;
    } token_content;
} Token;

typedef list_t Tokens;

Tokens lex(char* code);