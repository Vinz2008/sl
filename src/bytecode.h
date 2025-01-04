#ifndef BYTECODE_HEADER
#define BYTECODE_HEADER

#include "parser.h"
#include "list.h"
#include "hashtable.h"
#include <stdint.h>

enum vm_type_t {
    INT64_TYPE,
    DOUBLE_TYPE,
    STRING_TYPE,
};

typedef struct {
    enum vm_type_t vm_type;
} VM_Type;


typedef struct {
    VM_Type type;
} Value;

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

//typedef struct Instruction Instruction;


/*typedef struct Instruction {
    enum instruction_type_t instruction_type;
    union {
        long nb;
        char* str;
    } content;
} Instruction;*/


typedef struct {
    char* name;
    VM_Type return_type;
    // contains struct Args
    list_t args;
    size_t bytecode_pos;
} Function;

struct FunctionHeader {
    char* name;
    // TODO : add a pointer to the Function that is in the hashmap (for now just use the name to find it)
    size_t size;
};

typedef struct {
    uint8_t* bytecode;
    int allocated_size;
    int length;
} BytecodeByteArray;

typedef struct {
    BytecodeByteArray bytecode_array;
    hashtable_t functions;
} Bytecode;

Bytecode bytecode_gen(FileAST fileAST);

#endif