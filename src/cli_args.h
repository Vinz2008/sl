#ifndef CLIARGS_HEADER
#define CLIARGS_HEADER

#include <stdbool.h>

struct CliArgs {
    char* filename;
    bool should_dump_json;
};

#endif