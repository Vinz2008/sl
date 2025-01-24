#include "vm.h"

void run_vm(Bytecode bytecode){

    int instruction_pos = 0;
    while (instruction_pos < bytecode.bytecode_array.length){
        switch (bytecode.bytecode_array.bytecode[instruction_pos])
        {
            // TODO
        
        default:
            break;
        }
        instruction_pos++;
    }
    // TODO
}