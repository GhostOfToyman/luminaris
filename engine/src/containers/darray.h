#pragma once 

#include "defines.h"
/*
* Memory Layout
* u64 capacity - number of elements that can be held
* u64 length - number of elements contained
* u64 stride - size of each element in bytes
* void* elements
*/

enum {
  DARRAY_CAPACITY,
  DARRAY_LENGTH,
  DARRAY_STRIDE,
  DARRAY_FIELD_LENGTH
};

LAPI void* _darray_create(u64 length, u64 stride);
LAPI void _darray_destroy(void* array);

LAPI u64 _darray_field_get(void* array, u64 field);
LAPI void _darray_field_set(void* array, u64 field, u64 value);

LAPI void* _darray_resize(void* array);

LAPI void* _darray_push(void* array, const void* value_ptr);
LAPI void _darray_pop(void* array, void* dest);

LAPI void* _darray_push_at(void* array, u64 index, void* value_ptr);
LAPI void* _darray_pop_at(void* array, u64 index, void* dest);

