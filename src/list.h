#ifndef LIST_HEADER
#define LIST_HEADER

typedef struct {
    void** elements;
    int allocated_size;
    int size;
} list_t;

list_t init_list();
void list_append(list_t* list, void* element);
void list_destroy(list_t list);

#define FOREACH(_list_, _type_, _item_) \
    for (int _list_pos_ = 0; _list_pos_ < _list_.size; _list_pos_++) \
        for (_type_* _item_ = (_type_*)_list_.elements[_list_pos_], * _once_ = (_type_*)0; _once_ == (_type_*)0; _once_=_once_+1)

#define LIST_ELEMENT(_list_, _pos_) \
    _list_.elements[_pos_]

#endif