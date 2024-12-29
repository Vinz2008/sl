#include "list.h"

enum token_type_t {
    TOK_NB,
};

typedef struct {
    enum token_type_t token_type;
} Token;

typedef list_t Tokens;

Tokens lex(char* code);