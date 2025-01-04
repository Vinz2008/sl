#include "bytecode.h"

typedef struct {

} Value;

#define STACK_MAX 256

typedef struct {
    Bytecode bytecode;
    Instruction* instruction_pointer; // TODO : make it an uint8_t and make the Instruction lists in Bytecode uint8_t lists ? (like in https://craftinginterpreters.com/a-virtual-machine.html#the-vms-stack) 
    Value stack[STACK_MAX]; // TODO : make it dynamic
} VM;

void run_vm(Bytecode bytecode);