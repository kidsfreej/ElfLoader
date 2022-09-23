#include "elfloader.h"
#include <memory.h>
#include <stdbool.h>

Vector global_needed_libraries;
Vector loadedLibs;
void elfLoad(char* filename){
    global_needed_libraries= initVector();
    loadedLibs = initVector();
    Elf64 e=loadElf(filename);
    dumpSymbols(&e,e.linkInfo.dynamic_symtab);
    printf("%d",hashtableGet(&e,"printf").undef);
    runElf(e);
    freeElf(&e);
}
unsigned long elf_Hash(const unsigned char *name)
{
    unsigned long h = 0, g;
 
        while (*name)
        {
             h = (h << 4) + *name++;
             if (g = h & 0xf0000000)
                  h ^= g >> 24;
                   h &= ~g;
        }
        return h;
}
uint32_t
dl_new_hash (const char *s)
{
	        uint32_t h = 5381;

			        for (unsigned char c = *s; c != '\0'; c = *++s)
						                h = h * 33 + c;

					        return h;
							
}
size_t loadFile( char* filename,char* buffer,size_t size){
	FILE* f =fopen(filename,"rb");
    if(!f){
        printf("ERROR: Could not find/read file '%s'",filename);
        exit(0);
    }
	size_t v= fread(buffer,1,size,f);
    while(v<size){
        v+= fread(buffer,1,size-v,f);
    }
	fclose(f);
	return v;
}
size_t fileSize(char* filename){
	FILE* f=fopen(filename,"r");
	fseek(f,0,SEEK_END);
	size_t val = ftell(f);
	fclose(f);
	return val;
}
Elf64 loadElf( char* filename){
    size_t size = fileSize(filename);
    char* buffer=  malloc(size);
    loadFile(filename,buffer,size);
    Elf64 elf;
    memset(&elf,0,sizeof(elf));
    elf.filename = filename;
    //unsafe lmao
    elf.header = *(Elf64_Ehdr*)buffer;
    Elf64_Ehdr header = elf.header;
    if(header.e_ident[0]!=ELFMAG0||header.e_ident[1]!=ELFMAG1||header.e_ident[2]!=ELFMAG2||header.e_ident[3]!=ELFMAG3){
        printf("Error reading ELF. Unknown magic !!!");
        return elf;
    }
    if(header.e_ident[4]!=ELFCLASS64){
        printf("Expected 64 bit only ELF");
        return elf;
    }if(header.e_ident[5]!=ELFDATA2LSB){
        printf("Expected little endian");
        return elf;
    }



    elf.program_headers_length = header.e_phnum;
    elf.program_headers = malloc(sizeof(*elf.program_headers)*header.e_phnum);
    for(int i =0;i<header.e_phnum;i++){
        elf.program_headers[i]=*(Elf64_Phdr*)&buffer[i*header.e_phentsize+header.e_phoff];
    }

    elf.segments_length = elf.program_headers_length;
    elf.segments = malloc(elf.program_headers_length*sizeof(*elf.segments));
    memset(elf.segments,0,elf.segments_length*sizeof(*elf.segments));


    elf.section_headers_length = header.e_shnum;
    elf.section_headers = malloc(sizeof(*elf.section_headers)*header.e_shnum);
    for(int k =0;k<header.e_phnum;k++){
        elf.segments[k].length = 0;
    }
    for(int i =0;i<header.e_shnum;i++){
        elf.section_headers[i]=*(Elf64_Shdr*)&buffer[i*header.e_shentsize+header.e_shoff];


        for(int k =0;k<header.e_phnum;k++){

            if(ELF_SECTION_IN_SEGMENT_STRICT(&elf.section_headers[i],&elf.program_headers[k]){
                if(!elf.segments[k].sections){
                    elf.segments[k].sections=  malloc(elf.section_headers_length*sizeof(*elf.segments[k].sections));
                }
                elf.segments[k].sections[elf.segments[k].length] = &elf.section_headers[i];
                elf.segments[k].length++;
            }
        }
    }
    elf.buffer = buffer;
    elf.buffer_size = size;
    gatherLinkInfo(&elf);
    return elf;
    
}
//TODO FIX LOAD SECTION BECAUSE IM LOADING IT LATER THAN ITS BEING ACCESSED WHATEVER OK?!!?
void loadSection(char* buffer,Elf64_Shdr* hdr,Elf64* e){
    switch(hdr->sh_type){

        case SHT_NULL:
            return;
        case SHT_NOBITS:
            memset(&buffer[hdr->sh_addr],0,hdr->sh_size);
            return;
        default:
            memcpy(&buffer[hdr->sh_addr],&e->buffer[hdr->sh_offset],hdr->sh_size);
    }
}

Symbol hashtableGet(Elf64*e, char* symbol){
    typedef struct ElfHashtable{
       Elf64_Word nbucket; 
       Elf64_Word nchain; 
       Elf64_Word table[]; 
    }ElfHashtable;
    ElfHashtable* h = &e->buffer[e->linkInfo.hashtable->sh_offset];
    Elf64_Word* buckets=  &h->table[0];
    Elf64_Word* chain=  &h->table[h->nbucket];
    Symtab symtab = e->linkInfo.dynamic_symtab;
    Strtab strtab = e->linkInfo.dynamic_strtab;
    unsigned long hash = elf_Hash(symbol);
    if(h->nbucket+h->nchain+2>e->linkInfo.hashtable->sh_size/sizeof(Elf64_Word)){
        printf("ERROR LINE: ",__LINE__,"INVALID HASH TABLE SIZE INCORRECT");
    }

    Elf64_Word i = buckets[hash%h->nbucket];
    while(strcmp(&strtab.strtab[symtab.symtab[i].st_name],symbol)!=0){
        i = chain[i];
        if(i==0){
            Symbol s={0};
            
            s.undef = true;
            s.object=e;
            return s;
        }
    }
    Symbol s={0};
    s.str =&strtab.strtab[symtab.symtab[i].st_name];
    s.undef=false;
    s.sym = symtab.symtab[i]; 
    s.object = e;
    return s;
 }

char* getSectionName(Elf64* e,Elf64_Shdr* hdr){
    Elf64_Shdr* symstrtab = &e->section_headers[e->header.e_shstrndx];
    if(symstrtab->sh_offset+hdr->sh_name>e->buffer_size){
        printf("ERROR" ,__LINE__, ": FAILED TO GET SECTION NAME");
    }
    return &e->buffer[symstrtab->sh_offset+hdr->sh_name];
}
int indexToIndex(Elf64* e,Elf64_Shdr* hdr,size_t index){
    int k =0;
    for(char* i =&e->buffer[hdr->sh_offset];i<&e->buffer[hdr->sh_offset+hdr->sh_size];i+=strlen(i)+1){
        if(i==&e->buffer[hdr->sh_offset+index]){
            return k;
        }

        k++;
    }

    return -1;
}
void dumpSymbols(Elf64* e,Symtab stab){
    Elf64_Sym* symtab = stab.symtab;
    Elf64_Shdr* strtab = &e->section_headers[stab.hdr->sh_link];
    
    printf("\n\nSection name: %s\n\n",getSectionName(e,stab.hdr));
    for(Elf64_Sym* i =symtab;i<&e->buffer[stab.hdr->sh_offset+stab.hdr->sh_size];i++){
        printf("%d %d %s :%p\n",indexToIndex(e,strtab,i->st_name),i->st_name,&e->buffer[strtab->sh_offset+i->st_name],i->st_value);
    }
    // for(char* i =&e->buffer[strtab->sh_offset];i<&e->buffer[strtab->sh_offset+hdr->sh_size];i+=strlen(i)+1){
    //     printf("d: %s\n",i);
    // }
}

void retrieveRelSym(Elf64* e,uint64_t sh_link,Elf64_Shdr** symtab,Elf64_Shdr** strtab){
    *symtab= NULL;
    *strtab = NULL;
    if(sh_link<0 || sh_link>=e->section_headers_length){
        return;
    }
    *symtab = &e->section_headers[sh_link];
    uint64_t sym_link =symtab[0]->sh_link; 
    if(sym_link<0 || sym_link>=e->section_headers_length){
        return;
    }
    *strtab = &e->section_headers[sym_link];
}

char* symbolToLibraryBase(char* strsym){
    printf("NOT IMPLEMENTED ",__LINE__,"\n");
    return NULL;
}
uint64_t calculateRelocation(Elf64* e,char* runtime,size_t runtime_length,Elf64_Addr r_offset,uint64_t r_info, int64_t r_addend,uint64_t sh_link){
    //s = symbol value
    //a = addend
    //p = r_offset + section address (sh_link)
    //b = shared object load location (during runtime)
    //GOT = .got address
    //L = DONT USE, R_X86_64_PLT32 IS R_X86_64_PC32 (S+A-P)
    //z = size of symbol
    //G = swear at user
    Elf64_Shdr* symtab;
    Elf64_Shdr* strtab;
    retrieveRelSym(e,sh_link,&symtab,&strtab);
    int a = r_addend;
    // int b= symbolToLibraryBase();
    int p =r_offset;
    int got = e->linkInfo.got->sh_addr;
    return 0;
}
void performRelocations(){
    
}
void gatherLinkInfo(Elf64* e){
    //gather basic link info
    LinkInfo* linkInfo = &e->linkInfo;
    Vector rels=initVector();
    for(int i =0;i<e->section_headers_length;i++){
        if(e->section_headers[i].sh_type==SHT_DYNAMIC){
            processDynamic(e,&e->section_headers[i]);
        } if(strcmp(getSectionName(e,&e->section_headers[i]),".got")==0){
            linkInfo->got = &e->section_headers[i];
        }if(e->section_headers[i].sh_type==SHT_REL || e->section_headers[i].sh_type==SHT_RELA){
            appendVector(&rels,&e->section_headers[i]);
        }if(e->section_headers[i].sh_type==SHT_HASH){
            linkInfo->hashtable = &e->section_headers[i];
        }
        
    }
    stripVector(&rels);
    e->linkInfo.rels = (Elf64_Shdr**)rels.array;
    e->linkInfo.rels_length = rels.length;

}

void linkElf(Elf64* e,char* runtime){
    Elf64_Shdr** rels = e->linkInfo.rels;
    for(int i =0;i<e->linkInfo.rels_length;i++){
        if(rels[i]->sh_type==SHT_REL){
            
        }else if(rels[i]->sh_type==SHT_RELA){


        }else{
            printf("Unexpected section at line ",__LINE__);
        }
    }
}
Elf64_Shdr* sectionByAddr(Elf64* e,Elf64_Addr addr){
    for(int i =0;i<e->section_headers_length;i++){
        if(e->section_headers[i].sh_addr==addr){
            return &e->section_headers[i];
        }
    }
    return NULL;
}
void processDynamic(Elf64* e,Elf64_Shdr* dyn){
    Elf64_Dyn* dyn_array =(Elf64_Dyn*) &e->buffer[dyn->sh_offset];
    Vector needed = initVector();

    for(int i=0; dyn_array[i].d_tag!=DT_NULL&&i*sizeof(*dyn_array)<dyn->sh_size; i++){
        switch(dyn_array[i].d_tag){
            case DT_NEEDED:
                appendVector(&needed,(void*)dyn_array[i].d_un.d_val);
                break;
            case DT_SYMTAB:
                Elf64_Shdr* sect = sectionByAddr(e,dyn_array[i].d_un.d_ptr);
                Symtab symtab;
                symtab.size = sect->sh_size/sizeof(Elf64_Sym);
                symtab.symtab = &e->buffer[sect->sh_offset];
                symtab.hdr = sect;
                e->linkInfo.dynamic_symtab =symtab;
                break;
            case DT_STRTAB:

                sect = sectionByAddr(e,dyn_array[i].d_un.d_ptr);
                Strtab strtab;
                strtab.size = sect->sh_size;
                strtab.strtab = &e->buffer[sect->sh_offset];
                strtab.hdr = sect;
                e->linkInfo.dynamic_strtab = strtab;
                break;
            case DT_PLTGOT:
                break;
            case DT_INIT:
                e->runtimeInfo.init_func = dyn_array[i].d_un.d_val;
                break;
            case DT_FINI:
                e->runtimeInfo.fini_func = dyn_array[i].d_un.d_val;
                break;
            case DT_INIT_ARRAY:
                e->runtimeInfo.init_array =dyn_array[i].d_un.d_ptr;
                break;
            case DT_FINI_ARRAY:
                e->runtimeInfo.fini_array =dyn_array[i].d_un.d_ptr;
                break;
            case DT_INIT_ARRAYSZ:
                e->runtimeInfo.init_array_length = dyn_array[i].d_un.d_ptr;
                break;
            case DT_FINI_ARRAYSZ:
                e->runtimeInfo.fini_array_length = dyn_array[i].d_un.d_ptr;
                break;

        }
    }
    stripVector(&needed);
    
    e->linkInfo.needed_libraries = (char**) needed.array;
    e->linkInfo.needed_libraries_length = needed.length;
    for(int i =0;i<e->linkInfo.needed_libraries_length;i++){
        e->linkInfo.needed_libraries[i] =&e->buffer[e->linkInfo.dynamic_strtab.hdr->sh_offset+(uint32_t) e->linkInfo.needed_libraries[i] ];
    }
}
int inSegments(Elf64* e,Elf64_Shdr* hdr){
    for(int i =0;i<e->segments_length;i++){
        for(int k =0 ;k<e->segments[i].length;k++){
            if(e->segments[i].sections[k]==hdr){
                return 1;
            }
        }
    }
    return 0;
}
//returns number of added
size_t addLibrariesToGlobal(Elf64* e){
    size_t added = 0;
    char** needed = e->linkInfo.needed_libraries;
    for(int i =0;i<e->linkInfo.needed_libraries_length;i++){
        bool broken = false;
        for(int k =0;k<global_needed_libraries.length;k++){
            if(strcmp(needed[i],global_needed_libraries.array[k])==0){
                broken=true;
                break;
            }
        }
        if(!broken){
            appendVector(&global_needed_libraries,needed[i]);
            added++;
        }
    }
    return added;
}

bool libraryEq(Elf64* a,Elf64* b){
    return strcmp(a->filename,b->filename)==0;
}
void runElf(Elf64 e){
   
    size_t allocSize=0;
    for(int i =0;i<e.section_headers_length;i++){
        if(e.section_headers[i].sh_addr&& inSegments(&e,&e.section_headers[i])){
            allocSize= max(allocSize,e.section_headers[i].sh_addr+e.section_headers[i].sh_size);
        }
    }


    char* runtime = largeAlloc(allocSize);
    printf("\nbuffer%p\n",runtime);
    for(int i =0;i<e.segments_length;i++){
        Segment seg= e.segments[i];
        if(e.program_headers[i].p_type==PT_LOAD){
            for(int k=0;k<seg.length;k++){
                loadSection(runtime,seg.sections[k],&e);
            }   
        }
    }
    size_t addedLibsCount = addLibrariesToGlobal(&e);
    size_t upto = global_needed_libraries.length;
    for(int i =global_needed_libraries.length-addedLibsCount;i<upto;i++){
        if(inVector(&loadedLibs,global_needed_libraries.array[i],libraryEq)){
            continue;
        }
        Elf64* e = malloc(sizeof(Elf64));
        
        *e = loadElf( global_needed_libraries.array[i]);
        runElf(*e);
        appendVector(&loadedLibs,e);
        
    }
}

void freeElf(Elf64* elf){
    free(elf->program_headers);
    free(elf->section_headers);
    free(elf->buffer);
    for(int i =0;i<elf->segments_length;i++){
        if(elf->segments[i].sections){
            free(elf->segments[i].sections);
        }
    }
    free(elf->segments);
}
