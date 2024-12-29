#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "interpreter.h"

int main(int argc, char** argv){
    // TODO : proper args handling
    if (argc == 1){
        return interpret_repl(); 
    }
    if (strcmp(argv[1], "--help") == 0){
        printf("Usage: sl [script]\n");
        exit(0);
    }

    return interpret_file(argv[1]);
}