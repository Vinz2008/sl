#include "bytecode.h"

#define STACK_MAX 256

typedef struct {
    Bytecode bytecode;
    uint8_t* instruction_pointer;
    Value stack[STACK_MAX]; // TODO : make it dynamic
} VM;

void run_vm(Bytecode bytecode);