#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "interpreter.h"
#include "config.h"

int main(int argc, char** argv){
    // TODO : proper args handling
    if (argc == 1){
        return interpret_repl(); 
    }
    struct Config cliArgs;
    cliArgs.should_dump_json = false;
    cliArgs.should_dump_raw = false;
    cliArgs.filename = NULL;
    for (int i = 1; i < argc; i++){
        if (strcmp(argv[i], "--help") == 0){
            printf("Usage: sl [script]\n");
            exit(0);
        } else if (strcmp(argv[i], "--dump-json") == 0){
            cliArgs.should_dump_json = true;
        } else if (strcmp(argv[i], "--dump-raw") == 0){
            cliArgs.should_dump_raw = true;
        } else {
            cliArgs.filename = argv[i];
        }
    }
    if (!cliArgs.filename){
        fprintf(stderr, "Filename not found");
        exit(1);
    }

    return interpret_file(cliArgs);
}