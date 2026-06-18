#ifndef DYN_ARRAY_H
#define DYN_ARRAY_H

#include <assert.h>
#include <stdlib.h>

#define DEFINE_DYN_ARRAY(type, name, prefix) \
    typedef struct \
    { \
        size_t size, capacity; \
        type* data; \
    } name; \
    static inline void prefix##_alloc(name* arr, size_t capacity) \
    { \
        assert(arr != NULL); \
        arr->size     = 0; \
        arr->data     = malloc(capacity * sizeof(*arr->data)); \
        assert(arr->data != NULL); \
        arr->capacity = capacity; \
    } \
    static inline void prefix##_free(name* arr) \
    { \
        assert(arr != NULL); \
        assert(arr->data != NULL); \
        free(arr->data); \
    } \
    static inline void prefix##_realloc(name* arr, size_t capacity) \
    { \
        assert(arr != NULL); \
        if (arr->capacity == capacity) \
            return; \
        arr->data     = realloc(arr->data, capacity * sizeof(*arr->data)); \
        assert(arr->data != NULL); \
        arr->capacity = capacity; \
    } \
    static inline type* prefix##_append(name* arr, type val) \
    { \
        assert(arr != NULL); \
        while (arr->size >= arr->capacity) \
            prefix##_realloc(arr, arr->capacity * 2); \
        arr->data[arr->size++] = val; \
        return &arr->data[arr->size-1]; \
    } \
    static inline void prefix##_remove_unordered(name* arr, size_t idx) \
    { \
        assert(arr != NULL); \
        if (idx >= arr->size) \
            return; \
        arr->data[idx] = arr->data[--arr->size]; \
    }

// DEFINE_DYN_ARRAY(char, DynArrayChar, dyn_array_char)

#endif // DYN_ARRAY_H
