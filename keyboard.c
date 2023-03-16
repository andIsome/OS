#include "memory.h"


void* memcpy(void *dest, const void *src, size_t len){
    char *d = (char*)dest;
    const char *s = (const char*)src;

    while (len--)
        *d++ = *s++;
    
    return dest;
}


void* memset(void* dest, int val, size_t len){
    unsigned char *ptr = (unsigned char*)dest;

    while (len-- > 0)
        *ptr++ = val;

    return dest;
}

//void* malloc(size_t size){}
//void* realloc(void* ptr, size_t new_size){}