#include "vm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "string.h"

extern bool debug;

void push_stack(VM* vm, Value val){
    *vm->stack_top = val;
    vm->stack_top++;
}

Value pop_stack(VM* vm){
    vm->stack_top--;
    return *vm->stack_top;
}

void dump_stack(VM* vm){
    printf("\n\nSTART STACK DUMP\n");
    Value* temp_stack = vm->stack;
    while (temp_stack < vm->stack_top){
        int print_count = 0;
        switch (temp_stack->type.vm_type){
            case INT64_TYPE:
                print_count = printf("|  %ld  |\n", temp_stack->val.nb);
                break;
            default:
                fprintf(stderr, "Unsupported type in dumping stack");

        }

        for (int i = 0; i < print_count-1; i++){
            printf("-");
        }
        printf("\n");

        temp_stack++;
    }

    printf("END STACK DUMP\n\n");
}

void call_function(VM* vm, char* function_name){
    if (strcmp(function_name, "print") == 0){
        Value string_arg = pop_stack(vm);
        printf("%s\n", string_arg.val.s);
        return;
    }

    fprintf(stderr, "Function call not found");
    exit(1);
}

void run_vm(Bytecode bytecode){
    VM vm = (VM){
        .bytecode = bytecode,
        .instruction_pointer = bytecode.bytecode_array.bytecode,
    };

    vm.stack_top = vm.stack;
    while (vm.instruction_pointer < vm.bytecode.bytecode_array.bytecode + vm.bytecode.bytecode_array.length){
        if (debug){
            printf("%p\n", vm.instruction_pointer);
        }
        switch (*vm.instruction_pointer)
        {
            case INSTRUCTION_NUMBER:
                vm.instruction_pointer += 1;
                long nb;
                uint8_t* nb_bytes = (uint8_t*)&nb;
                for (int i = 0; i < sizeof(nb)/sizeof(uint8_t); i++){
                    nb_bytes[i] = vm.instruction_pointer[i];
                }
                vm.instruction_pointer += sizeof(nb);
                push_stack(&vm, (Value){
                    .type = (VM_Type){
                        .vm_type = INT64_TYPE,
                    },
                    .val.nb = nb,
                });
                break;
            case INSTRUCTION_STRING: {
                vm.instruction_pointer += 1;

                uint8_t* str_start = vm.instruction_pointer;

                while (*vm.instruction_pointer != '\0'){
                    vm.instruction_pointer++;
                }

                size_t str_length = vm.instruction_pointer - str_start;
                char* str = malloc(sizeof(char) * str_length);
                memcpy(str, str_start, str_length);

                vm.instruction_pointer++; // pass '\0'
                push_stack(&vm, (Value){
                    .type = (VM_Type){
                        .vm_type = STRING_TYPE,
                    },
                    .val.s = str,
                    //.val.s = str.str,  
                });
                break;
            }
            case INSTRUCTION_ADD: {
                vm.instruction_pointer += 1;
                Value rhs = pop_stack(&vm);
                Value lhs = pop_stack(&vm);
                long res = lhs.val.nb + rhs.val.nb;
                push_stack(&vm, (Value){
                    .type = (VM_Type){
                        .vm_type = INT64_TYPE,
                    },
                    .val.nb = res,
                });
                break;
            }
            case INSTRUCTION_MINUS: {
                vm.instruction_pointer += 1;
                Value rhs = pop_stack(&vm);
                Value lhs = pop_stack(&vm);
                long res = lhs.val.nb - rhs.val.nb;
                push_stack(&vm, (Value){
                    .type = (VM_Type){
                        .vm_type = INT64_TYPE,
                    },
                    .val.nb = res,
                });
                break;
            }
            case INSTRUCTION_MULT: {
                vm.instruction_pointer += 1;
                Value rhs = pop_stack(&vm);
                Value lhs = pop_stack(&vm);
                long res = lhs.val.nb * rhs.val.nb;
                push_stack(&vm, (Value){
                    .type = (VM_Type){
                        .vm_type = INT64_TYPE,
                    },
                    .val.nb = res,
                });
                break;
            }

            case INSTRUCTION_DIV: {
                vm.instruction_pointer += 1;
                Value rhs = pop_stack(&vm);
                Value lhs = pop_stack(&vm);
                long res = lhs.val.nb / rhs.val.nb;
                push_stack(&vm, (Value){
                    .type = (VM_Type){
                        .vm_type = INT64_TYPE,
                    },
                    .val.nb = res,
                });
                break;
            }

            case INSTRUCTION_CALL: {
                vm.instruction_pointer += 1;
                string_t function_name = init_string();
                // TODO : replace this with just a memcpy
                while (*vm.instruction_pointer != '\0'){
                    string_append(&function_name, *vm.instruction_pointer);
                    vm.instruction_pointer++;
                }
                vm.instruction_pointer++; // pass '\0'
                // TODO : implement call
                call_function(&vm, function_name.str);

                string_destroy(function_name);
                break;
            }

            // TODO
        
            default:
                fprintf(stderr, "Unknown instruction in vm : %d\n", *vm.instruction_pointer);
                exit(1);
        }
        if (debug){
            dump_stack(&vm);
        }
    }

    if (vm.stack_top != vm.stack){
        printf("vm result : %ld\n", vm.stack_top[-1].val.nb);
    }
    // TODO
}