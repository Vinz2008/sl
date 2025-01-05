#include "bytecode.h"
#include <stdlib.h>
#include <stdio.h>
#include "parser.h"

static void bytecode_write(BytecodeByteArray* bytecode_array, uint8_t byte){
    if (bytecode_array->length+1 == bytecode_array->allocated_size){
        bytecode_array->allocated_size *= 2;
        bytecode_array->bytecode = realloc(bytecode_array->bytecode, sizeof(uint8_t) * bytecode_array->allocated_size);
    }

    bytecode_array->bytecode[bytecode_array->length] = byte;
    bytecode_array->length++;
}

static void AstNodeToInstruction(AstNode* astNode, BytecodeByteArray* bytecode_array);

static void createOpInstruction(struct BinOp binOp, BytecodeByteArray* bytecode_array){
    uint8_t instruction_type;
    switch (binOp.op_token->token_type)
    {
        case PLUS_OP:
            instruction_type = INSTRUCTION_ADD;
            break;
        case MINUS_OP:
            instruction_type = INSTRUCTION_MINUS;
            break;
        case MULT_OP:
            instruction_type = INSTRUCTION_MULT;
            break;
        case DIV_OP:
            instruction_type = INSTRUCTION_DIV;
            break;
        default:
            fprintf(stderr, "Unknown instruction type\n");
            exit(1);
    }
    AstNodeToInstruction(binOp.LHS, bytecode_array);
    AstNodeToInstruction(binOp.RHS, bytecode_array);
    bytecode_write(bytecode_array, instruction_type);
}

static void FunctionAstNodeToFunction(AstNode* function, BytecodeByteArray* bytecode_array){
    // TODO
    fprintf(stderr, "Not implemented\n");
    exit(1);
}

static void AstNodeToInstruction(AstNode* astNode, BytecodeByteArray* bytecode_array){
    // TODO : can return multiple instructions so instead, pass the list where the instructions need to be added
    switch (astNode->node_type)
    {
    case AST_NUMBER:
        bytecode_write(bytecode_array, INSTRUCTION_NUMBER);
        uint8_t* nb_bytes = (uint8_t*)&astNode->content.nb;
        for (int i = 0; i < sizeof(astNode->content.nb)/sizeof(uint8_t); i++){
            bytecode_write(bytecode_array, nb_bytes[i]);
        }
        /*Instruction* nb_instruction = malloc(sizeof(Instruction));
        nb_instruction->instruction_type = INSTRUCTION_NUMBER;
        nb_instruction->content.nb = astNode->content.nb;
        list_append(instructions, nb_instruction);*/
        break;
    case AST_STRING:
        /*Instruction* str_instruction = malloc(sizeof(Instruction));
        str_instruction->instruction_type = INSTRUCTION_STRING;
        str_instruction->content.str = astNode->content.static_string;
        list_append(instructions, str_instruction);*/
        break;
    case AST_BINOP:
        struct BinOp binOp = astNode->content.binop;
        createOpInstruction(binOp, bytecode_array);
        /*Instruction* binop_instruction = malloc(sizeof(Instruction));
        createOpInstruction(binOp, binop_instruction, instructions);
        list_append(instructions, binop_instruction);*/
        break;
    case AST_UNARYOP:
        break;
    case AST_FUNCTION:
        FunctionAstNodeToFunction(astNode, bytecode_array);
        break;
    default:
        fprintf(stderr, "Unknown AstNode in bytecode codegen");
        exit(1);
    }
}

Bytecode bytecode_gen(FileAST fileAST){
    const int initial_size = 10;
    BytecodeByteArray bytecode_array;
    bytecode_array.bytecode = malloc(initial_size * sizeof(uint8_t));
    bytecode_array.allocated_size = initial_size;
    bytecode_array.length = 0;
    hashtable_t functions = hashtable_create();

    FOREACH (fileAST.astNodes, AstNode, astNode){
        AstNodeToInstruction(astNode, &bytecode_array);
    }

    // bytecode will not be resized (optimizations that would be added must be before that)
    // so we resize the array to the exact size of the bytecode
    bytecode_array.bytecode = realloc(bytecode_array.bytecode, bytecode_array.length * sizeof(uint8_t));
    bytecode_array.allocated_size = bytecode_array.length;

    return (Bytecode) {
        .bytecode_array = bytecode_array,
        .functions = functions,
    };
}