#ifndef BYTECODE_HEADER
#define BYTECODE_HEADER

#include "parser.h"
#include "list.h"

enum vm_type_t {
    INT64_TYPE,
    DOUBLE_TYPE,
    STRING_TYPE,
};

typedef struct {
    enum vm_type_t vm_type;
} VM_Type;

struct Args {
    VM_Type type;
    char* name;
};

// TODO : add stack instructions
enum instruction_type_t {
    INSTRUCTION_RETURN,
    INSTRUCTION_PUSH,
    INSTRUCTION_NUMBER,
    INSTRUCTION_STRING,
    INSTRUCTION_ADD,
    INSTRUCTION_MINUS,
    INSTRUCTION_MULT,
    INSTRUCTION_DIV,
};

typedef struct Instruction Instruction;


typedef struct Instruction {
    enum instruction_type_t instruction_type;
    union {
        long nb;
        char* str;
    } content;
} Instruction;


typedef struct {
    char* name;
    // contains struct Args
    list_t args;
    // contains Instruction
    list_t instructions;
} Function;

typedef struct {
    Function* entry_function;
    // list of Function
    list_t functions;
} Bytecode;

Bytecode bytecode_gen(FileAST fileAST);

#endif