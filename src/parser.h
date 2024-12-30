#include "list.h"
#include "lexer.h"

typedef struct {

} AstNode;

typedef struct {
    // list of AstNode
    list_t astNodes;
} FileAST;


FileAST parse(Tokens tokens);

