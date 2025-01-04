#include "bytecode.h"
#include <stdlib.h>
#include <stdio.h>
#include "parser.h"

static void AstNodeToInstruction(AstNode* astNode, list_t* instructions);

static void createOpInstruction(struct BinOp binOp, Instruction* binop_instruction, list_t* instructions){
    switch (binOp.op_token->token_type)
    {
        case PLUS_OP:
            binop_instruction->instruction_type = INSTRUCTION_ADD;
            break;
        case MINUS_OP:
            binop_instruction->instruction_type = INSTRUCTION_MINUS;
            break;
        case MULT_OP:
            binop_instruction->instruction_type = INSTRUCTION_MULT;
            break;
        case DIV_OP:
            binop_instruction->instruction_type = INSTRUCTION_DIV;
            break;
        default:
            break;
    }
    AstNodeToInstruction(binOp.LHS, instructions);
    AstNodeToInstruction(binOp.RHS, instructions);
}

static Function* FunctionAstNodeToFunction(AstNode* function){
    // TODO
    fprintf(stderr, "Not implemented\n");
    exit(1);
}

static void AstNodeToInstruction(AstNode* astNode, list_t* instructions){
    // TODO : can return multiple instructions so instead, pass the list where the instructions need to be added
    switch (astNode->node_type)
    {
    case AST_NUMBER:
        Instruction* nb_instruction = malloc(sizeof(Instruction));
        nb_instruction->instruction_type = INSTRUCTION_NUMBER;
        nb_instruction->content.nb = astNode->content.nb;
        list_append(instructions, nb_instruction);
        break;
    case AST_STRING:
        Instruction* str_instruction = malloc(sizeof(Instruction));
        str_instruction->instruction_type = INSTRUCTION_STRING;
        str_instruction->content.str = astNode->content.static_string;
        list_append(instructions, str_instruction);
        break;
    case AST_BINOP:
        struct BinOp binOp = astNode->content.binop;
        Instruction* binop_instruction = malloc(sizeof(Instruction));
        createOpInstruction(binOp, binop_instruction, instructions);
        list_append(instructions, binop_instruction);
        break;
    case AST_UNARYOP:
        break;
    case AST_FUNCTION:
    default:
        fprintf(stderr, "Unknown AstNode in bytecode codegen");
        exit(1);
    }
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
            list_append(&functions, FunctionAstNodeToFunction(astNode));
        } else {
            AstNodeToInstruction(astNode, &entry_function->instructions);
        }
    }
    return (Bytecode) {
        .entry_function = entry_function,
        .functions = functions,
    };
}