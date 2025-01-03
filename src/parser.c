#include "parser.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "lexer.h"


// TODO : flatten the AST ?

void call_function(char* function_name, list_t args);

typedef struct {
    int tok_pos;
    Tokens tokens;
    Token* cur_tok;
} Parser;


static bool has_tokens_left(Parser parser){
    return parser.tok_pos + 1 < parser.tokens.size;
}

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

/*static Token* peek(Parser parser){
    return (Token*)parser.tokens.elements[parser.tok_pos+1];
}*/


static AstNode* ParsePrimary(Parser* parser){
    // TODO : replace with a switch ?
    AstNode* node = malloc(sizeof(AstNode));
    enum token_type_t token_type = parser->cur_tok->token_type;
    printf("parsing token type : %s\n", token_type_to_string(token_type));
    if (token_type == NUMBER){
        node->node_type = AST_NUMBER;
        node->content.nb = parser->cur_tok->token_content.nb;
        advanceToken(parser);
    } else if (token_type == STRING){
        node->node_type = STRING;
        node->content.static_string = parser->cur_tok->token_content.str;
        advanceToken(parser);
    } else if (token_type == IDENTIFIER){
        // TODO
        printf("found identifier\n");
        char* identifier = parser->cur_tok->token_content.identifier;
        advanceToken(parser);
        if (has_tokens_left(*parser) && parser->cur_tok->token_type == OPEN_PARENTHESIS){
            printf("found open parenthesis\n");
            printf("function call : %s\n", identifier);
            for (int i = 0 ; i < strlen(identifier); i++){
                printf("function name %s[%d] : %c %d\n", identifier, i, identifier[i], identifier[i]);
            }
            advanceToken(parser); // pass open parenthesis
            if (has_tokens_left(*parser) && parser->cur_tok->token_type == STRING){
                printf("found string\n");
                char* str = parser->cur_tok->token_content.str;
                printf("arg first : %s\n", str);
                list_t args = init_list();
                list_append(&args, str);
                advanceToken(parser); // pass string
                call_function(identifier, args);
            }
        }
    } else {
        free(node);
        fprintf(stderr, "BUG : unknown token when parsing\n");
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
    advanceToken(parser);
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
        advanceToken(parser); // pass operator

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
        AstNode* node = ParseExpression(&parser);
        list_append(&fileAST.astNodes, node);
    }
    return fileAST;
}