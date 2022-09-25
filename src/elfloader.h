#ifndef ELFLOADER
#define ELFLOADER

#include <stdio.h>
#include "buffer.h"
#include "elf.h"
#include "datastructures.h"
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
    uint64_t value;
    uint64_t type;
    uint64_t binding;
    uint64_t size;
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
    Elf64_Shdr* gnuHashtable;
}LinkInfo;

typedef struct{
    byte* base;
    Elf64_Addr init_array;
    Elf64_Addr init_func;
    size_t init_array_length;
    Elf64_Addr fini_array;
    Elf64_Addr fini_func;
    size_t fini_array_length;
}RuntimeInfo;

typedef struct Elf64{
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

#define BITS 64
#define ERROR(x...) (printf("ERROR LINE ",__LINE__,": ",x))

void elfLoad(char* filename);
unsigned long elf_Hash(const unsigned char *name);
size_t loadFile( char* filename,char* buffer,size_t size);
size_t fileSize(char* filename);
Elf64* parseElf( char* filename);
void loadSection(char* buffer,Elf64_Shdr* hdr,Elf64* e);
Symbol gnuHashtableGet(Elf64* e,char* symbol);
Symbol hashtableGet(Elf64*e, char* symbol);
char* getSectionName(Elf64* e,Elf64_Shdr* hdr);
int indexToIndex(Elf64* e,Elf64_Shdr* hdr,size_t index);
void dumpSymbols(Elf64* e,Symtab stab);
Symtab makeSymtab(Elf64* e,Elf64_Shdr* symtab);
Strtab makeStrtab(Elf64* e,Elf64_Shdr* strtab);
void retrieveRelSym(Elf64* e,uint64_t sh_link,Symtab* symtab,Strtab* strtab);
Symbol symLookup(Elf64* e,char* symbol,Vector* libs,int type_class);
Symbol makeSymbol(Elf64_Sym sym,char* str,Elf64* e);
void performRelocation(Elf64* e,Elf64_Addr r_offset,uint64_t r_info, int64_t r_addend,uint64_t sh_link,Vector* loadedLibs);
void performRelocations();
void gatherLinkInfo(Elf64* e);
void linkElf(Elf64* e,Vector* loadedLibs);
Elf64_Shdr* sectionByAddr(Elf64* e,Elf64_Addr addr);
void processDynamic(Elf64* e,Elf64_Shdr* dyn);
int inSegments(Elf64* e,Elf64_Shdr* hdr);
size_t addLibrariesToGlobal(Elf64* e,Vector* global_needed_libraries);
bool libraryEq(void* elf,void* str);
void loadElf(Elf64* e,Vector* loadedLibs,Vector* global_needed_libraries);
void freeElf(Elf64* elf);

#endif