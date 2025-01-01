#include "ast_json.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "string.h"

const char* AstToJson(FileAST ast){
    string_t json = init_string();
    // TODO

    return (const char*)json.str;
}

void logToFileAST(const char* filename, FileAST ast){
    const char* content = AstToJson(ast);
    size_t content_length = strlen(content);
    FILE* file = fopen(filename, "w");
    fwrite(content, sizeof(char), content_length, file);
    fclose(file);
}