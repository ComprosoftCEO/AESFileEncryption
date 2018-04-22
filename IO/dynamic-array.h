#ifndef DYNAMIC_ARRAY_HEADER
#define DYNAMIC_ARRAY_HEADER

//TODO: Move some of the exposed variables into an internal object

#include <stddef.h>

typedef struct {
    void* ptr;      // Internal string buffer
    size_t el_size; // How big is each element
    size_t index;   // Where to insert the next character
    size_t len;     // Absolute string length (with null-terminator)
    size_t max;     // Biggest index used (DO NOT MESS WITH THIS!!!)
} Dynamic_Arr_t, *pDynamic_Arr_t;

typedef void (*Free_Func_t)(void*);


pDynamic_Arr_t new_dynamic_array(size_t el_size);
void free_dynamic_array(pDynamic_Arr_t, Free_Func_t func);


void add_array_element(pDynamic_Arr_t arr, const void* new);

//new_arr should be a pointer to an array of element (such as a char*)
//  If new_arr is a pointer to an array of pointers, use add_p (such as char**)
void add_array_elements(pDynamic_Arr_t arr, const void* new_arr, size_t count);
void add_array_elements_p(pDynamic_Arr_t arr, const void** new_ptrs, size_t count);

void set_array_index(pDynamic_Arr_t arr, size_t index);
void* get_array_element(pDynamic_Arr_t arr, size_t index);

//Resize the pointer
void* flush_dynamic_array(pDynamic_Arr_t arr);

size_t get_array_count(pDynamic_Arr_t arr);

#endif // DYNAMIC_ARRAY_HEADER Included
