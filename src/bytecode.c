#include "bytecode.h"
#include <stdlib.h>
#include <stdio.h>
#include "parser.h"

static Instruction* AstNodeToInstruction(AstNode* astNode);

static void createOpInstruction(struct BinOp binOp, Instruction* instruction){
    switch (binOp.op_token->token_type)
    {
        case PLUS_OP:
            instruction->instruction_type = INSTRUCTION_ADD;
            break;
        case MINUS_OP:
            instruction->instruction_type = INSTRUCTION_MINUS;
            break;
        case MULT_OP:
            instruction->instruction_type = INSTRUCTION_MULT;
            break;
        case DIV_OP:
            instruction->instruction_type = INSTRUCTION_DIV;
            break;
        default:
            break;
    }
}

static Instruction* AstNodeToInstruction(AstNode* astNode){
    // TODO : can return multiple instructions so instead, pass the list where the instructions need to be added
    Instruction* instruction = malloc(sizeof(Instruction));
    switch (astNode->node_type)
    {
    case AST_FUNCTION:
        // TODO
        break;
    case AST_NUMBER:
        instruction->instruction_type = INSTRUCTION_NUMBER;
        instruction->content.nb = astNode->content.nb;
        break;
    case AST_STRING:
        instruction->instruction_type = INSTRUCTION_STRING;
        instruction->content.str = astNode->content.static_string;
        break;
    case AST_BINOP:
        struct BinOp binOp = astNode->content.binop;
        createOpInstruction(binOp, instruction);
        break;
    case AST_UNARYOP:
        break;
    default:
        fprintf(stderr, "Unknown AstNode in bytecode codegen");
        exit(1);
    }
    return instruction;
}

Bytecode bytecode_gen(FileAST fileAST){
    list_t functions = init_list();
    Function* entry_function = malloc(sizeof(Function));
    entry_function->name = "__entry";
    entry_function->args = init_list();
    entry_function->instructions = init_list();
    list_append(&functions, entry_function);
    FOREACH (fileAST.astNodes, AstNode, astNode){
        if (IS_FUNCTION(astNode)){
            // TODO : functions
            // need to transform the astNode to a function
            list_append(&functions, AstNodeToInstruction(astNode));
        } else {
            list_append(&entry_function->instructions, AstNodeToInstruction(astNode));
        }
    }
    return (Bytecode) {
        .entry_function = entry_function,
        .functions = functions,
    };
}