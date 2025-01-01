#ifndef PARSER_HEADER
#define PARSER_HEADER

#include "list.h"
#include "lexer.h"

enum ast_node_type_t {
    AST_NUMBER,
    AST_STRING,
    AST_BINOP,
    AST_UNARYOP
};

typedef struct AstNode AstNode;

struct BinOp {
    AstNode* LHS;
    Token* op_token;
    AstNode* RHS;
};

struct UnaryOp {
    Token* op;
    AstNode* operand;
};
typedef struct AstNode {
    enum ast_node_type_t node_type;
    union {
        long nb;
        char* static_string;
        struct BinOp binop;
        struct UnaryOp unop;
    } content;
} AstNode;

typedef struct {
    // list of AstNode
    list_t astNodes;
} FileAST;


FileAST parse(Tokens tokens);

#endif