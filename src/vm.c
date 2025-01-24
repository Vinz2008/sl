#include "vm.h"

void run_vm(Bytecode bytecode){
    VM vm = (VM){
        .bytecode = bytecode,
        .instruction_pointer = bytecode.bytecode_array.bytecode,
    };

    vm.stack_top = vm.stack;
    while (vm.instruction_pointer < vm.bytecode.bytecode_array.bytecode + vm.bytecode.bytecode_array.length){
        switch (*vm.instruction_pointer)
        {
            // TODO
        
        default:
            break;
        }
        vm.instruction_pointer += 1; // TODO : increase of the size of an instruction (or do it in the switch if the instructions are of variable length)
    }
    // TODO
}