#pragma once
#include <stdint.h>

typedef uint32_t size_t;


//void* memcpy(void* dst, void* src, size_t size);

void* memcpy(void *dest, const void *src, size_t len);


void* memset(void *dest, int val, size_t len);


//void* malloc(size_t size);
//void* realloc(void* ptr, size_t new_size);