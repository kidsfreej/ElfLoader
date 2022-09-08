#include "buffer.h"
LPVOID largeAlloc(size_t size){
    SYSTEM_INFO s;

    GetSystemInfo(&s);
    DWORD gran = s.dwAllocationGranularity;
    size+=gran-size%gran;
   

    return VirtualAlloc(NULL,size,MEM_COMMIT |MEM_RESERVE,PAGE_EXECUTE_READWRITE );
}
void largeFree(LPVOID p,size_t size){
    if(!VirtualFree(p,size,MEM_RELEASE)){
        printf("ERROR FREEING!");
    }
}