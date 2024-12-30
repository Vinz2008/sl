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

bool has_tokens_left(Parser parser){
    return parser.tok_pos + 1 < parser.tokens.size;
}

Token* advanceToken(Parser* parser){
    parser->tok_pos++;
    Token* new_token = (Token*)parser->tokens.elements[parser->tok_pos];
    parser->cur_tok = new_token;
    return new_token;
}

Token* peek(Parser parser){
    return (Token*)parser.tokens.elements[parser.tok_pos+1];
}


AstNode* ParsePrimary(Parser* parser){
    // TODO : replace with a switch ?
    AstNode* node = malloc(sizeof(AstNode));
    enum token_type_t token_type = parser->cur_tok->token_type;
    if (token_type == NUMBER){
        node->node_type = AST_NUMBER;
        node->content.nb = parser->cur_tok->token_content.nb;
    } else if (token_type == STRING){
        node->node_type = STRING;
        node->content.static_string = parser->cur_tok->token_content.str;
    } else if (token_type == IDENTIFIER){
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

// TODO : parse unary

AstNode* ParseBinary(Parser* parser){
    AstNode* LHS = ParsePrimary(parser);
    if (parser->tok_pos + 1 >= parser->tokens.size){
        return LHS;
    }
    enum token_type_t op_type = peek(*parser)->token_type;
    if (!is_token_operator(op_type)){
        return LHS;
    }
    Token* op_token = advanceToken(parser);
    
    // TODO : implement operator precedence

    AstNode* RHS = ParsePrimary(parser);
    AstNode* binop = malloc(sizeof(AstNode));
    binop->node_type = AST_BINOP;
    binop->content.binop = (struct BinOp){
        .LHS = LHS,
        .op_token = op_token,
        .RHS = RHS,
    };

    return binop;
}

AstNode* ParseExpression(Parser* parser){
    return ParseBinary(parser);   
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
        AstNode* node = ParseExpression(&parser);
        list_append(&fileAST.astNodes, node);
    }
    return fileAST;
}