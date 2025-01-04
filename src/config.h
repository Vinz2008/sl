#ifndef CLIARGS_HEADER
#define CLIARGS_HEADER

#include <stdbool.h>

struct Config {
    char* filename;
    bool should_dump_json;
    bool should_dump_raw;
};

#endif