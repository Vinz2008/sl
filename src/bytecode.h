#ifndef BYTECODE_HEADER
#define BYTECODE_HEADER

#include "parser.h"

typedef struct {

} Bytecode;

Bytecode bytecode_gen(FileAST fileAST);

#endif