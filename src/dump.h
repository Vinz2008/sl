#include "parser.h"
#include "bytecode.h"

void logToFileAST(const char* filename, FileAST ast);
void logToFileBytecode(const char* filename, Bytecode bytecode);
void dumpRawBytecode(const char* filename, Bytecode bytecode);