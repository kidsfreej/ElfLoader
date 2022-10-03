#include "libcsim.h"
//TODO: HANDLE THREAD LOCAL STORAGE
//INITLIIE PTHREADS HEARD AND LOCAL STORAGE
//PT_TLS 
//import undocumented relocation: TPOFFSET64 
//read auxvec and init library based auxvecs (RANDOM)
//my own version of https://codebrowser.dev/glibc/glibc/csu/libc-start.c.html#234
int libc_start_main(int(*main)(int, char** ,char** ,void*),int argc,char** argv,auxv_t* auxvecs, int(*init)(int, char** ,char** ,void*),void(* fini)(void), void (*rtld_fini) (void),
			    void *stack_end)
{
    auxv_t* auxvec;
    char** evp = &argv[argc+1];
    while(*evp++ != NULL);
    auxvec = evp;
    exit(main(arc,argv,,auxvec));
}