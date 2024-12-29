typedef struct {
    char* str;
    int allocated_size;
    int length;
} string_t;

string_t init_string();
void string_append(string_t* s, char element);
void string_destroy(string_t list);