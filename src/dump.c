#include "dump.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "string.h"
#include "yyjson.h"

static const char* const ast_node_strings[] = {
    [AST_FUNCTION] = "function",
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


static void addNodeType(yyjson_mut_doc* doc, yyjson_mut_val* json_node, AstNode* astNode){
    yyjson_mut_val *key = yyjson_mut_str(doc, "node_type");
    yyjson_mut_val *num = yyjson_mut_str(doc, ast_type_to_string(astNode));
    yyjson_mut_obj_add(json_node, key, num);
}

static yyjson_mut_val* AstNodeToJson(yyjson_mut_doc* doc, AstNode* astNode){
    switch (astNode->node_type){
        case AST_NUMBER:
            yyjson_mut_val* number_node = yyjson_mut_obj(doc);
            addNodeType(doc, number_node, astNode);
            yyjson_mut_val* nb_key = yyjson_mut_str(doc, "nb_val");
            yyjson_mut_val* nb_val = yyjson_mut_int(doc, astNode->content.nb);
            yyjson_mut_obj_add(number_node, nb_key, nb_val);
            return number_node;
        case AST_STRING:
            yyjson_mut_val* str_node = yyjson_mut_obj(doc);
            addNodeType(doc, str_node, astNode);
            yyjson_mut_val* str_key = yyjson_mut_str(doc, "str");
            yyjson_mut_val* str_val = yyjson_mut_str(doc, astNode->content.static_string);
            yyjson_mut_obj_add(str_node, str_key, str_val);
            return str_node;
        case AST_UNARYOP:
            struct UnaryOp unaryop = astNode->content.unop;
            yyjson_mut_val* unary_node = yyjson_mut_obj(doc);
            addNodeType(doc, unary_node, astNode);
            yyjson_mut_val* operand_key = yyjson_mut_str(doc, "LHS");
            yyjson_mut_val* operand_object = AstNodeToJson(doc, unaryop.operand);
            yyjson_mut_obj_add(unary_node, operand_key, operand_object);
            return unary_node;
        case AST_BINOP:
            struct BinOp binop = astNode->content.binop;
            yyjson_mut_val* binop_node = yyjson_mut_obj(doc);
            addNodeType(doc, binop_node, astNode);
            yyjson_mut_val* LHS_key = yyjson_mut_str(doc, "LHS");
            yyjson_mut_val* LHS_object = AstNodeToJson(doc, binop.LHS);
            yyjson_mut_obj_add(binop_node, LHS_key, LHS_object);
            yyjson_mut_val* RHS_key = yyjson_mut_str(doc, "RHS");
            yyjson_mut_val* RHS_object = AstNodeToJson(doc, binop.RHS);
            yyjson_mut_obj_add(binop_node, RHS_key, RHS_object);
            return binop_node;
        default:
            fprintf(stderr, "Unknown Ast Node type");
            exit(1);
    }
}

yyjson_mut_doc* AstToJsonDoc(FileAST ast){
    yyjson_mut_doc *doc = yyjson_mut_doc_new(NULL);
    yyjson_mut_val *root = yyjson_mut_obj(doc);
    yyjson_mut_doc_set_root(doc, root);

    yyjson_mut_val* statements = yyjson_mut_arr(doc);
    FOREACH (ast.astNodes, AstNode, astNode){
        yyjson_mut_val* json_node = AstNodeToJson(doc, astNode);
        yyjson_mut_arr_append(statements, json_node);
    }

    yyjson_mut_obj_add(root, yyjson_mut_str(doc, "statements"), statements);

    return doc;
}

// need to free this
char* AstToJson(FileAST ast){
    const yyjson_mut_doc* doc = AstToJsonDoc(ast);
    char *json = yyjson_mut_write(doc, YYJSON_WRITE_PRETTY, NULL);
    return json;
}

void logToFileAST(const char* filename, FileAST ast){
    const yyjson_mut_doc* doc = AstToJsonDoc(ast);
    yyjson_mut_write_file(filename, doc, YYJSON_WRITE_PRETTY, NULL, NULL);
}

static const char* const instruction_strings[] = {
    [INSTRUCTION_NUMBER] = "number",
    [INSTRUCTION_STRING] = "string",
    [INSTRUCTION_ADD] = "add",
    [INSTRUCTION_MINUS] = "minus",
    [INSTRUCTION_MULT] = "mult",
    [INSTRUCTION_DIV] = "div",
};

static const char* instruction_type_to_string(uint8_t instruction_type){
    return instruction_strings[instruction_type];
}

static void addInstructionType(yyjson_mut_doc* doc, yyjson_mut_val* json_node, uint8_t instruction_type){
    yyjson_mut_val *key = yyjson_mut_str(doc, "instruction_type");
    yyjson_mut_val *num = yyjson_mut_str(doc, instruction_type_to_string(instruction_type));
    yyjson_mut_obj_add(json_node, key, num);
}

static void advanceInstruction(uint8_t** instruction, int* instruction_pos){
    (*instruction)++;
    (*instruction_pos)++;
}

static yyjson_mut_val* BytecodeInstructionToJson(yyjson_mut_doc* doc, uint8_t* instruction, int* instruction_pos){
    uint8_t instruction_type = *instruction;
    printf("Instruction type : %s\n", instruction_type_to_string(instruction_type));
    advanceInstruction(&instruction, instruction_pos);
    switch (instruction_type){
        case INSTRUCTION_NUMBER:
            yyjson_mut_val* number_node = yyjson_mut_obj(doc);
            long nb;
            uint8_t* nb_bytes = (uint8_t*)&nb;
            for (int i = 0; i < sizeof(nb)/sizeof(uint8_t); i++){
                nb_bytes[i] = instruction[i];
            }
            *instruction_pos += sizeof(nb)/sizeof(uint8_t);
            addInstructionType(doc, number_node, instruction_type);
            yyjson_mut_val* nb_key = yyjson_mut_str(doc, "nb_val");
            yyjson_mut_val* nb_val = yyjson_mut_int(doc, nb);
            yyjson_mut_obj_add(number_node, nb_key, nb_val);
            return number_node;
        case INSTRUCTION_STRING:
            yyjson_mut_val* str_node = yyjson_mut_obj(doc);
            string_t str = init_string();
            int i = 0;
            while (instruction[i] != '\0'){
                string_append(&str, instruction[i]);
                i++;
            }
            *instruction_pos += i+1;
            string_append(&str, '\0'); // TODO : is needed ?

            addInstructionType(doc, str_node, instruction_type);
            yyjson_mut_val* str_key = yyjson_mut_str(doc, "str");
            yyjson_mut_val* str_val = yyjson_mut_strcpy(doc, str.str);
            yyjson_mut_obj_add(str_node, str_key, str_val);

            string_destroy(str);
            return str_node;
        case INSTRUCTION_ADD:
        case INSTRUCTION_MINUS:
        case INSTRUCTION_MULT:
        case INSTRUCTION_DIV:
            yyjson_mut_val* binop_node = yyjson_mut_obj(doc);
            addInstructionType(doc, binop_node, instruction_type);
            return binop_node;
        default:
            fprintf(stderr, "Unknown Instruction type %d", instruction_type);
            exit(1);
    }
    return NULL;
}


static yyjson_mut_doc* BytecodeToJsonDoc(Bytecode bytecode){
    yyjson_mut_doc *doc = yyjson_mut_doc_new(NULL);
    yyjson_mut_val *root = yyjson_mut_obj(doc);
    yyjson_mut_doc_set_root(doc, root);

    yyjson_mut_val* instructions = yyjson_mut_arr(doc);
    // TODO : iterate all functions instead
    int instruction_pos = 0;
    while (instruction_pos < bytecode.bytecode_array.length){
        yyjson_mut_val* json_node = BytecodeInstructionToJson(doc, bytecode.bytecode_array.bytecode+instruction_pos, &instruction_pos);
        yyjson_mut_arr_append(instructions, json_node);
    }

    yyjson_mut_obj_add(root, yyjson_mut_str(doc, "instructions"), instructions);

    return doc;
}

void logToFileBytecode(const char* filename, Bytecode bytecode){
    const yyjson_mut_doc* doc = BytecodeToJsonDoc(bytecode);
    yyjson_mut_write_file(filename, doc, YYJSON_WRITE_PRETTY, NULL, NULL);
}

void dumpRawBytecode(const char* filename, Bytecode bytecode){
    FILE* file = fopen(filename, "wb");
    fwrite(bytecode.bytecode_array.bytecode, bytecode.bytecode_array.length, sizeof(uint8_t), file); 
    fclose(file);
}