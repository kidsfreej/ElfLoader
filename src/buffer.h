


#include <stdio.h>
#include <stdint.h>
#include <memory.h>
#include <string.h>
#ifdef _WIN64
#include <windows.h>
#endif
#ifdef __unix__
#include <sys/mman.h>
#endif
typedef unsigned char byte;
void* largeAlloc(size_t size);
void largeFree(void* p,size_t size);
