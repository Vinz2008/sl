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
    AST_RETURN,
    AST_FUNCTION_DECL,
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

struct Return {
    AstNode* expr;
};

struct FunctionArgs {
    char* type;
    char* name;
};

struct FunctionDecl {
    char* function_name;
    char* return_type; // is null if no type is supplied
    // contains FunctionArgs
    list_t args;

    list_t exprs;

};

typedef struct AstNode {
    enum ast_node_type_t node_type;
    union {
        long nb;
        char* static_string;
        struct BinOp binop;
        struct UnaryOp unop;
        struct FunctionCall call;
        struct Return ret;
        struct FunctionDecl function_decl;
    } content;
} AstNode;


typedef struct {
    list_t astNodes;
} FileAST;

//#define IS_FUNCTION(astNode) astNode->node_type == AST_FUNCTION


const char* ast_type_to_string(AstNode* astNode);

FileAST parse(Tokens tokens);
void destroyFileAST(FileAST fileAST);

#endif