#include <elf.h>

typedef struct
{
  long int a_type;              /* Entry type */
  union
    {
      long int a_val;           /* Integer value */
      void *a_ptr;              /* Pointer value */
      void (*a_fcn) (void);     /* Function pointer value */
    } a_un;
} auxv_t;

#define GET_REGISTER(var,reg) __asm__("movq %%" #reg ",%0" :"=m"(var))