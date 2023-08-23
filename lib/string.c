#include "string.h"

size_t strlen(const char *str)
{
    char *ptr = (char *)str;
    while (*ptr != EOS)
    {
        ptr++;
    }
    return ptr - str;
}

void *memcpy(void *dest, void *src, size_t count)
{
    char *ptr = dest;
    while(count--)
    {
        *ptr++ = *((char *)(src++));
    }
    return dest;
}

void* memset(void* dest, char ch, size_t count)
{
    char* ptr = dest;
    while(count--)
    {
        *ptr++ = ch;
    }
    return dest;
}