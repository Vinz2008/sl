#include "ast_json.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "string.h"

static const char* const ast_node_strings[] = {
    [AST_NUMBER] = "number",
    [AST_STRING] = "string",
    [AST_BINOP] = "",
    [AST_UNARYOP] = "",
};

static const char* ast_type_to_string(AstNode* astNode){
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

static int nb_opened_object = 0;

static void start_json_object(string_t* json, bool isObjectVal){
    if (!isObjectVal){
        for (int i = 0; i < nb_opened_object; i++){
            string_append(json, '\t');
        }
    }
    string_append_str(json, "{\n");
    nb_opened_object++;
}

static void write_json(string_t* json, char* format, ...){
    va_list vlist;
    va_start(vlist, format);
    for (int i = 0; i< nb_opened_object; i++){
        string_append(json, '\t');
    }
    vstring_writef(json, format, vlist);
    va_end(vlist);
}

static void end_json_object(string_t* json, bool isObjectVal){
    nb_opened_object--;
    for (int i = 0; i < nb_opened_object; i++){
        string_append(json, '\t');
    }
    string_append_str(json, "}\n");
}


static void AstNodeToJson(string_t* json, AstNode* astNode, bool isObjectVal){
    switch (astNode->node_type){
        case AST_NUMBER:
            start_json_object(json, isObjectVal);
            write_json(json, "\"type\": \"%s\",\n", ast_type_to_string(astNode));
            write_json(json, "\"nb_val\": %ld,\n", astNode->content.nb);
            end_json_object(json, isObjectVal);
            break;
        case AST_STRING:
            start_json_object(json, isObjectVal);
            write_json(json, "\"type\": \"%s\",\n", ast_type_to_string(astNode));
            end_json_object(json, isObjectVal);
            break;
        case AST_UNARYOP:
            start_json_object(json, isObjectVal);
            write_json(json, "\"type\": \"%s\",\n", ast_type_to_string(astNode));
            end_json_object(json, isObjectVal);
            break;
        case AST_BINOP:
            struct BinOp binop = astNode->content.binop;
            start_json_object(json, isObjectVal);
            write_json(json, "\"type\": \"%s\",\n", ast_type_to_string(astNode));
            write_json(json, "\"LHS\": ");
            AstNodeToJson(json, binop.LHS, true);
            string_append_str(json, "\n");
            write_json(json, "\"RHS\": ");
            AstNodeToJson(json, binop.RHS, true);
            string_append_str(json, "\n");
            end_json_object(json, isObjectVal);
            break;
        default:
            fprintf(stderr, "Unknown Ast Node type");
            exit(1);
    }
}

const char* AstToJson(FileAST ast){
    string_t json = init_string();
    nb_opened_object = 0;
    // TODO

    FOREACH (ast.astNodes, AstNode, astNode){
        AstNodeToJson(&json, astNode, false);
    }

    return (const char*)json.str;
}

void logToFileAST(const char* filename, FileAST ast){
    const char* content = AstToJson(ast);
    size_t content_length = strlen(content);
    FILE* file = fopen(filename, "w");
    fwrite(content, sizeof(char), content_length, file);
    fclose(file);
    free((void*)content);
}