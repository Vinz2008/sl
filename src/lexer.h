#include "list.h"

enum token_type_t {
    IDENTIFIER,
    PLUS_OP,
    MINUS_OP,
    MULT_OP,
    DIV_OP,
    STRING,
    NUMBER,
    TOK_NB,
};

typedef struct {
    enum token_type_t token_type;
} Token;

typedef list_t Tokens;

Tokens lex(char* code);