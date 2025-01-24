#include "bytecode.h"

#define STACK_MAX 256

typedef struct {
    Bytecode bytecode;
    uint8_t* instruction_pointer;
    Value stack[STACK_MAX]; // TODO : make it dynamic
    Value* stack_top;
} VM;

void run_vm(Bytecode bytecode);