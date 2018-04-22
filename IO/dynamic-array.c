#include <dynamic-array.h>
#include <stdlib.h>
#include <string.h>

pDynamic_Arr_t new_dynamic_array(size_t el_size) {
    if (el_size == 0) {return NULL;}
    pDynamic_Arr_t arr = malloc(sizeof(Dynamic_Arr_t));

    if (!arr) {return NULL;}

    arr->el_size = el_size;
    arr->index = 0;
    arr->ptr = NULL;

    return arr;
}

void free_dynamic_array(pDynamic_Arr_t arr, Free_Func_t func) {
    if (!arr) {return;}

    if (func != NULL) {
        size_t i;
        for (i = 0; i < get_array_count(arr); ++i) {
            func(*(void**) get_array_element(arr,i));
        }
    }

    if (arr->ptr != NULL) {free(arr->ptr);}
    free(arr);
}

void add_array_element(pDynamic_Arr_t arr,const void* new) {
    if (!arr) {return;}
    if (!new) {return;}

    if (arr->ptr == NULL) {
        arr->index = 0;
        arr->max = 0;
        arr->len = 16;  //Initial length
        arr->ptr = malloc(arr->len * arr->el_size);
    }

    if (!arr->ptr) {return;}

    //You can have a NULL buffer going into this function
    if (arr->index >= arr->len) {
        arr->len+=16;
        arr->ptr = realloc(arr->ptr,arr->len * arr->el_size);
        if (!arr->ptr) {return;}
    }


    //Cast to a char* to do pointer arithmetic
    void* index = (void*) (((char*) arr->ptr)+ (arr->index * arr->el_size));

    memcpy(index,new,arr->el_size);

    arr->index+=1;
    if (arr->index > arr->max) {arr->max = arr->index;}
}


void add_array_elements(pDynamic_Arr_t arr, const void* new_arr, size_t count) {
    if (!arr) {return;}
    if (!new_arr) {return;}

    size_t i;
    for (i = 0; i < count; ++i) {
        void* next = (void*) (((char*) new_arr) + (i * arr->el_size));
        add_array_element(arr,next);
    }
}

void add_array_elements_p(pDynamic_Arr_t arr, const void** new_ptrs, size_t count) {
    if (!arr) {return;}
    if (!new_ptrs) {return;}

    size_t i;
    for (i = 0; i < count; ++i) {
        add_array_element(arr,new_ptrs[i]);
    }


}


void set_array_index(pDynamic_Arr_t arr, size_t index) {
    if (!arr) {return;}
    if (index >= arr->len) {return;}
    arr->index = index;
}

void* get_array_element(pDynamic_Arr_t arr, size_t index) {
    if (!arr) {return NULL;}
    if (index >= arr->len) {return NULL;}

    //Cast to a char* to do pointer arithmetic
    return (void*) (((char*) arr->ptr)+ (index * arr->el_size));
}

//Return NULL on error or an empty array
void* flush_dynamic_array(pDynamic_Arr_t arr) {
    if (!arr) {return NULL;}

    //Resize array to match the max size
    void* ret = NULL;
    if (arr->max == 0) {if (arr->ptr) {free(arr->ptr);} ret = NULL;}
    else {ret = realloc(arr->ptr,arr->max * arr->el_size);}

    arr->index = 0;
    arr->max = 0;
    arr->len = 0;
    arr->ptr = NULL;

    return ret;
}

//How many items are in this array???
size_t get_array_count(pDynamic_Arr_t arr) {
    if (!arr) {return 0;}
    if (arr->ptr == NULL) {return 0;}
    return (arr->max);
}
