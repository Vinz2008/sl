#include "interpreter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"
#include "bytecode.h"
#include "vm.h"
#include "ast_json.h"
#include "string.h"

// for now, the args can only be char*, so it will contain that
// TODO : create a Value struct
 
void call_function(char* function_name, list_t args){
    printf("calling function\n");
    printf("is print : %d\n", strcmp(function_name, "print"));
    if (strcmp(function_name, "print") == 0){
        fprintf(stderr, "%s\n", (char*)args.elements[0]);
        printf("after printing");
    } else {
        fprintf(stderr, "Unknown function\n");
        exit(1);
    }
}

static void remove_file_extension(string_t* filename){
    size_t len = strlen(filename->str);
    size_t i = len-1;
    while (i >= 0 && filename->str[i] != '.'){
        i--;
    }
    if (i == 0){
        return;
    }

    memset(filename+i, 0, len-i);
    filename->length -= len-i;
}

int interpret_code(char* code, struct CliArgs cliArgs){
    Tokens tokens = lex(code);
    FileAST fileAst = parse(tokens);

    if (cliArgs.should_dump_json){
        string_t json_filename = init_string_from_str(cliArgs.filename);
        remove_file_extension(&json_filename);
        string_append_str(&json_filename, ".json");
        logToFileAST(json_filename.str, fileAst);
        string_destroy(json_filename);
    }

    Bytecode bytecode = bytecode_gen(fileAst);
    run_vm(bytecode);

    destroyTokens(tokens);
    return 0;
}

char* read_file(FILE* file){
    fseek(file, 0L, SEEK_END);
    int file_size = ftell(file);
    rewind(file);
    char* buf = malloc(sizeof(char) * file_size);
    fread(buf, file_size, file_size, file);
    return buf;
}

int interpret_file(struct CliArgs cliArgs){
    FILE* file = fopen(cliArgs.filename, "r");
    char* content = read_file(file);
    printf("%s\n", content);
    fclose(file);
    int ret = interpret_code(content, cliArgs);
    free(content);
    if (ret == -1){
        return -1;
    }
    return 0;
}


int interpret_repl(){
    printf("not implemented\n");
    exit(1);
    // TODO (use interpret_code)
}