#include "parser.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "lexer.h"


// TODO : flatten the AST ?

typedef struct {
    int tok_pos;
    Tokens tokens;
    Token* cur_tok;
} Parser;

static AstNode* ParseExpression(Parser* parser);


static const char* const ast_node_strings[] = {
    [AST_FUNCTION] = "function",
    [AST_NUMBER] = "number",
    [AST_STRING] = "string",
    [AST_BINOP] = "",
    [AST_UNARYOP] = "",
    [AST_FUNCTION_CALL] = "function call",
    [AST_RETURN] = "return",
    [AST_FUNCTION_DECL] = "function decl",
};

const char* ast_type_to_string(AstNode* astNode){
    if (astNode->node_type == AST_BINOP){
        char* buf = malloc(sizeof(char) * (1 + 1 + strlen("binop ")));
        const char* op_str = token_type_to_string(astNode->content.binop.op_token->token_type);
        sprintf(buf, "binop %c", *op_str); // op_str is one character
        return buf;
    }
    if (astNode->node_type == AST_UNARYOP){
        char* buf = malloc(sizeof(char) * (1 + 1 + strlen("unaryop ")));
        const char* op_str = token_type_to_string(astNode->content.unop.op->token_type);
        sprintf(buf, "unaryop %c", *op_str); // op_str is one character
        return buf;
    }

    return ast_node_strings[astNode->node_type];
}


static bool has_tokens_left(Parser parser){
    return parser.tok_pos + 1 < parser.tokens.size;
}


// TODO : remove ?
static Token* advanceToken(Parser* parser){
    parser->tok_pos++;
    if (parser->tok_pos > parser->tokens.size){
        parser->cur_tok = NULL;
        return NULL;
    }
    Token* new_token = (Token*)parser->tokens.elements[parser->tok_pos];
    parser->cur_tok = new_token;
    return new_token;
}

// returns the last token
static Token* eatToken(Parser* parser, enum token_type_t token_type){
    Token* cur_tok = parser->cur_tok;
    if (cur_tok->token_type != token_type){
        // call error function (with line number, etc)
        fprintf(stderr, "expected %s token, got %s instead\n", token_type_to_string(token_type), token_type_to_string(cur_tok->token_type));
        exit(1);
    }
    parser->tok_pos++;
    if (parser->tok_pos > parser->tokens.size){
        parser->cur_tok = NULL;
        return NULL;
    }
    Token* new_token = (Token*)parser->tokens.elements[parser->tok_pos];
    parser->cur_tok = new_token;
    return cur_tok;
}

/*static Token* peek(Parser parser){
    return (Token*)parser.tokens.elements[parser.tok_pos+1];
}*/


static AstNode* ParsePrimary(Parser* parser){
    enum token_type_t token_type = parser->cur_tok->token_type;
    if (token_type == OPEN_PARENTHESIS){
        eatToken(parser, OPEN_PARENTHESIS);
        AstNode* node = ParseExpression(parser);
        eatToken(parser, CLOSE_PARENTHESIS);
        return node;
    }
    // TODO : replace with a switch ?
    AstNode* node = malloc(sizeof(AstNode));
    printf("parsing token type : %s\n", token_type_to_string(token_type));
    if (token_type == NUMBER){
        node->node_type = AST_NUMBER;
        node->content.nb = parser->cur_tok->token_content.nb;
        eatToken(parser, NUMBER);
    } else if (token_type == STRING){
        node->node_type = AST_STRING;
        node->content.static_string = parser->cur_tok->token_content.str;
        eatToken(parser, STRING);
    } else if (token_type == RETURN){
        eatToken(parser, RETURN);
        AstNode* expr = ParseExpression(parser);
        node->node_type = AST_RETURN;
        node->content.ret = (struct Return){
            .expr = expr,
        };
    } else if (token_type == IDENTIFIER){
        // TODO
        printf("found identifier\n");
        char* identifier = parser->cur_tok->token_content.identifier;
        eatToken(parser, IDENTIFIER);
        if (has_tokens_left(*parser) && parser->cur_tok->token_type == OPEN_PARENTHESIS){
            printf("found open parenthesis\n");
            printf("function call : %s\n", identifier);
            for (int i = 0 ; i < strlen(identifier); i++){
                printf("function name %s[%d] : %c %d\n", identifier, i, identifier[i], identifier[i]);
            }
            eatToken(parser, OPEN_PARENTHESIS);
            list_t args = init_list();
            while (has_tokens_left(*parser) && parser->cur_tok->token_type != CLOSE_PARENTHESIS){
                AstNode* arg = ParseExpression(parser);
                if (parser->cur_tok->token_type != CLOSE_PARENTHESIS){
                    eatToken(parser, COMMA);
                }
                list_append(&args, arg);
            }

            eatToken(parser, CLOSE_PARENTHESIS);
            node->node_type = AST_FUNCTION_CALL;
            node->content.call = (struct FunctionCall){
                .name = identifier,
                .args = args,
            };
            
        }
    } else {
        free(node);
        fprintf(stderr, "BUG : unknown token when parsing %s\n", token_type_to_string(token_type));
        exit(1);
    }
    return node;
}

struct BinOpPrecedence {
    enum token_type_t op;
    int precedence;
};

struct BinOpPrecedence binOpPrecedences[] = {
    { PLUS_OP, 20},
    { MINUS_OP, 20},
    { MULT_OP, 30},
    { DIV_OP, 30},
};

const int binOpPrecedencesLength = sizeof(binOpPrecedences)/sizeof(struct BinOpPrecedence);

static int getOpPrecedence(enum token_type_t op_type){
    for (int i = 0; i < binOpPrecedencesLength; i++){
        if (binOpPrecedences[i].op == op_type){
            return binOpPrecedences[i].precedence;
        }
    }
    return -1;
}

static AstNode* ParseUnary(Parser* parser){
    if (parser->cur_tok->token_type != MINUS_OP){
        // TODO : add support for postfix unary operators
        return ParsePrimary(parser); // ParsePrimary or ParseBinary ?
    }
    Token* op = parser->cur_tok;
    eatToken(parser, op->token_type);
    //advanceToken(parser);
    AstNode* expr = ParsePrimary(parser);
    AstNode* unop = malloc(sizeof(AstNode));
    unop->node_type = AST_UNARYOP;
    unop->content.unop = (struct UnaryOp){
        .op = op,
        .operand = expr,
    };
    return unop;
}

static AstNode* ParseBinaryRec(Parser* parser, AstNode* LHS, int last_expr_precedence){
    while (true){
        if (parser->tok_pos >= parser->tokens.size){
            return LHS;
        }
        enum token_type_t op_type = parser->cur_tok->token_type;
        if (!is_token_binary_operator(op_type)){
            printf("is not token operator\n");
            return LHS;
        }
        int tok_precedence = getOpPrecedence(op_type);
        if (tok_precedence < last_expr_precedence){
            return LHS;
        }
        Token* op_token = parser->cur_tok;
        //advanceToken(parser); // pass operator
        eatToken(parser, op_token->token_type);

        AstNode* RHS = ParseUnary(parser);

        if (parser->tok_pos < parser->tokens.size){
            int next_precedence = getOpPrecedence(parser->cur_tok->token_type);
            if (tok_precedence < next_precedence){
                RHS = ParseBinaryRec(parser, RHS, tok_precedence + 1);
            }

        }

        AstNode* binop = malloc(sizeof(AstNode));
        binop->node_type = AST_BINOP;
        binop->content.binop = (struct BinOp){
            .LHS = LHS,
            .op_token = op_token,
            .RHS = RHS,
        };

        LHS = binop;
    }

}

static AstNode* ParseBinary(Parser* parser, int last_expr_precedence){
    AstNode* LHS = ParseUnary(parser);
    return ParseBinaryRec(parser, LHS, 0);
}


static AstNode* ParseExpression(Parser* parser){
    return ParseBinary(parser, 0);   
}

static AstNode* ParseFunctionDecl(Parser* parser){
    eatToken(parser, FUNCTION);
    char* function_name = eatToken(parser, IDENTIFIER)->token_content.identifier;
    eatToken(parser, OPEN_PARENTHESIS);

    list_t args = init_list();

    while (has_tokens_left(*parser) && parser->cur_tok->token_type != CLOSE_PARENTHESIS){
        char* arg_name = eatToken(parser, IDENTIFIER)->token_content.identifier;
        char* arg_type = NULL;
        
        if (has_tokens_left(*parser) && parser->cur_tok->token_type == COLON){
            eatToken(parser, COLON);
            arg_type = eatToken(parser, IDENTIFIER)->token_content.identifier;
        }
        
        if (has_tokens_left(*parser) && parser->cur_tok->token_type != CLOSE_PARENTHESIS){
            eatToken(parser, COMMA);
        }
        
        struct FunctionArgs* arg = malloc(sizeof(struct FunctionArgs));
        arg->name = arg_name;
        arg->type = arg_type; 
        list_append(&args, arg);
    }

    eatToken(parser, CLOSE_PARENTHESIS);

    list_t exprs = init_list();

    eatToken(parser, OPEN_BRACKETS);

    while (has_tokens_left(*parser) && parser->cur_tok->token_type != CLOSE_BRACKETS){
        printf("parser->cur_tok->token_type : %d\n", parser->cur_tok->token_type);
        AstNode* expr = ParseExpression(parser);
        list_append(&exprs, expr);
    }

    eatToken(parser, CLOSE_BRACKETS);

    AstNode* node = malloc(sizeof(AstNode));
    node->node_type = AST_FUNCTION_DECL;
    node->content.function_decl = (struct FunctionDecl){
        .function_name = function_name,
        .args = args,
        .return_type = NULL, // TODO
        .exprs = exprs,
    };

    return node;
}

static AstNode* ParseAstNode(Parser* parser){
    switch (parser->cur_tok->token_type){
        case FUNCTION:
            return ParseFunctionDecl(parser);
        default:
            return ParseExpression(parser);
    }
}


FileAST parse(Tokens tokens){
    FileAST fileAST = (FileAST){
        .astNodes = init_list(),
    };

    Parser parser = (Parser){
        .tok_pos = 0,
        .tokens = tokens,
        .cur_tok = (Token*)tokens.elements[0],
    };
    

    while (parser.tok_pos < tokens.size){
        printf("parser.tok_pos : %d, tokens.size : %d\n", parser.tok_pos, tokens.size);
        AstNode* node = ParseAstNode(&parser);
        list_append(&fileAST.astNodes, node);
    }
    return fileAST;
}

void destroyFileAST(FileAST fileAST){
    for (int i = 0; i < fileAST.astNodes.size; i++){
        free(fileAST.astNodes.elements[i]);
        // TODO : destroy astNodes children
    }
    list_destroy(fileAST.astNodes);
}