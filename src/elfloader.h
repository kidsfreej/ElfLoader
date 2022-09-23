#include <stdio.h>
#include "buffer.h"
#include "elf.h"
#include "datastructures.h"
typedef void(*void_func_t)(void);
struct Elf64;
typedef struct{
    Elf64_Shdr** sections; 
    size_t length;
}Segment;
typedef struct{
    Elf64_Sym* symtab;
    size_t size;
    Elf64_Shdr* hdr;
}Symtab;
typedef struct{
    char* strtab;
    size_t size;
    Elf64_Shdr* hdr;
}Strtab;
typedef struct{
    char* str;
    Elf64_Sym sym;
    bool undef;
    struct Elf64* object;
}Symbol;
typedef struct{

    Elf64_Shdr* got;
    char** needed_libraries;
    size_t needed_libraries_length;
    Symtab dynamic_symtab;
    Strtab dynamic_strtab;
    Elf64_Shdr** rels;
    size_t rels_length;
    Elf64_Shdr* hashtable;

}LinkInfo;

typedef struct{
    Elf64_Addr init_array;
    Elf64_Addr init_func;
    size_t init_array_length;
    Elf64_Addr fini_array;
    Elf64_Addr fini_func;
    size_t fini_array_length;
}RuntimeInfo;

typedef struct{
    Elf64_Ehdr header;
    Elf64_Phdr* program_headers;
    size_t program_headers_length;
    Elf64_Shdr* section_headers;
    size_t section_headers_length;
    char* buffer;
    size_t buffer_size;
    Segment* segments;
    size_t segments_length;
    LinkInfo linkInfo;
    RuntimeInfo runtimeInfo;
    char* filename;
}Elf64;
void runElf(Elf64 e);

Elf64 loadElf(char* filename);
void freeElf(Elf64* elf);
void elfLoad(char* filename);

void gatherLinkInfo(Elf64* e);

void processDynamic(Elf64* e,Elf64_Shdr* dyn);
Symbol hashtableGet(Elf64*e, char* symbol);
#define ERROR(x) (printf("ERROR LINE ",__LINE__,": ",x))