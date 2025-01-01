#include <stdarg.h>

typedef struct {
    char* str;
    int allocated_size;
    int length;
} string_t;

string_t init_string();
string_t init_string_from_str(const char* string);
string_t init_string_from_allocated_str(char* string);
void string_append(string_t* s, char element);
void string_append_str(string_t* s, const char* str);
void string_writef(string_t* s, char* format, ...) __attribute__ ((format (printf, 2, 3)));
void vstring_writef(string_t* s, const char* format, va_list vlist);
void string_overwritef(string_t* s, char* format, ...) __attribute__ ((format (printf, 2, 3)));
void string_destroy(string_t list);