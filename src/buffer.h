#include <stdio.h>
#include <stdint.h>
#include <memory.h>
#include <string.h>
#include <windows.h>

typedef unsigned char byte;
LPVOID largeAlloc(size_t size);
void largeFree(LPVOID p,size_t size);