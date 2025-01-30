#ifndef PARSER_HEADER
#define PARSER_HEADER

#include "list.h"
#include "lexer.h"

enum ast_node_type_t {
    AST_FUNCTION,
    AST_NUMBER,
    AST_STRING,
    AST_BINOP,
    AST_UNARYOP,
    AST_FUNCTION_CALL,
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

struct FunctionCall {
    char* name;
    // contains AstNode pointers
    list_t args;
};

typedef struct AstNode {
    enum ast_node_type_t node_type;
    union {
        long nb;
        char* static_string;
        struct BinOp binop;
        struct UnaryOp unop;
        struct FunctionCall call;
    } content;
} AstNode;

typedef struct {
    // list of AstNode
    list_t astNodes;
} FileAST;

//#define IS_FUNCTION(astNode) astNode->node_type == AST_FUNCTION


FileAST parse(Tokens tokens);
void destroyFileAST(FileAST fileAST);

#endif