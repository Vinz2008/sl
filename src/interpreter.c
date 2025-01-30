#include "interpreter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"
#include "bytecode.h"
#include "vm.h"
#include "dump.h"
#include "string.h"

static void remove_file_extension(string_t* filename){
    int len = strlen(filename->str);
    int i = len-1;
    while (i >= 0 && filename->str[i] != '.'){
        i--;
    }
    if (i == 0){
        return;
    }

    memset(filename->str+i, 0, len-i);
    filename->length -= len-i;
}

static int interpret_code(char* code, struct Config config){
    Tokens tokens = lex(code);
    FileAST fileAst = parse(tokens);

    if (config.should_dump_json){
        string_t json_ast_filename = init_string_from_str(config.filename);
        remove_file_extension(&json_ast_filename);
        string_append_str(&json_ast_filename, ".ast.json");
        logToFileAST(json_ast_filename.str, fileAst);
        string_destroy(json_ast_filename);
    }

    Bytecode bytecode = bytecode_gen(fileAst);
    
    if (config.should_dump_json){
        string_t json_bytecode_filename = init_string_from_str(config.filename);
        remove_file_extension(&json_bytecode_filename);
        string_append_str(&json_bytecode_filename, ".bc.json");
        logToFileBytecode(json_bytecode_filename.str, bytecode);
        string_destroy(json_bytecode_filename);
    }

    if (config.should_dump_raw){
        string_t raw_bytecode_filename = init_string_from_str(config.filename);
        remove_file_extension(&raw_bytecode_filename);
        string_append_str(&raw_bytecode_filename, ".bc");
        dumpRawBytecode(raw_bytecode_filename.str, bytecode);
        string_destroy(raw_bytecode_filename);
    }

    run_vm(bytecode);

    destroyTokens(tokens);
    destroyFileAST(fileAst);
    destroyBytecode(bytecode);
    return 0;
}

static char* read_file(FILE* file){
    fseek(file, 0L, SEEK_END);
    int file_size = ftell(file);
    rewind(file);
    char* buf = malloc(sizeof(char) * (file_size + 1));
    fread(buf, file_size, file_size, file);
    buf[file_size] = '\0';
    return buf;
}

int interpret_file(struct Config config){
    FILE* file = fopen(config.filename, "r");
    char* content = read_file(file);
    printf("%s\n", content);
    fclose(file);
    int ret = interpret_code(content, config);
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