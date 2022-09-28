#include "elfloader.h"
//load libs will always be in reverse topological order of dependencies
void elfLoad(char* filename){
    Queue global_needed_libraries;
    Vector loadedLibs;

    global_needed_libraries=initQueue();
    loadedLibs = initVector();

    enQueue(&global_needed_libraries,filename);
    while(global_needed_libraries.length){
	    Elf64* cure = parseElf((char*)deQueue(&global_needed_libraries));
	    appendVector(&loadedLibs,cure);
	    loadElf(cure,&loadedLibs,&global_needed_libraries);
    }
    for(int i =0;i<loadedLibs.length;i++){
        linkElf(loadedLibs.array[loadedLibs.length-i-1],&loadedLibs);
    }
    Elf64* e = (Elf64*)loadedLibs.array[0];
    dumpSymbols(e,e->linkInfo.dynamic_symtab);
    runElf((Elf64*)loadedLibs.array[0]);
    for(int i =0;i<loadedLibs.length;i++){
        freeElf(loadedLibs.array[i]);
    }
    freeVector(&loadedLibs);
    freeQueue(&global_needed_libraries);
    
}
void endFunc(){
	printf("PROGRAM TERMINATED SUCCESFULLY?");
}
void runElf(Elf64* e){
    void* entry_point = e->runtimeInfo.base+e->header.e_entry;
	asm("\n\
	movq %0,%%rax\n\
	pushq $0\n\
	pushq $0\n\
	pushq $0\n\
	pushq $0\n\
	pushq $0\n\
	pushq $0\n\
	movq %1, %%rdx\n\
	jmp %0"::"m"(entry_point),"m"(endFunc));
	__asm__("pushq $0");
	__asm__("pushq $0");
	__asm__("pushq $0");
	__asm__("pushq $0");
	__asm__("pushq $0");
	__asm__("pushq $0");
	__asm__("pushq $0");
	__asm__("movq %0,%%rdx"::"r"(endFunc));
    __asm__("jmp %0"::"r"(entry_point));
}
//DEUB NOTES
//0x7ffff77fec85 
//0x7ffff7841ee1
//0x7ffff78383fd
//
//0x7ffff77fec85
//0x7ffff7fc515d
//0x7ffff7841ee1
//0x7ffff78383fd
//error at 0x7ffff77fe0d0
//relocation(possibly) at (0x7ffff7bc8048)
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
gnu_Hash (const char *s)
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
Elf64* parseElf( char* filename){
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
        return NULL;
    }
    if(header.e_ident[4]!=ELFCLASS64){
        printf("Expected 64 bit only ELF");
        return NULL;
    }if(header.e_ident[5]!=ELFDATA2LSB){
        printf("Expected little endian");
        return NULL;
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

    Elf64* ret = malloc(sizeof(*ret));
    *ret= elf;
    return ret;
    
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
/* this is my code but how this works is explained here: https://flapenguin.me/elf-dt-gnu-hash */
Symbol gnuHashtableGet(Elf64* e,char* symbol){
    typedef struct gnu_hash_table {
        uint32_t nbuckets;
        uint32_t symoffset;
        uint32_t bloom_size;
        uint32_t bloom_shift;
        uint32_t  arrs[];
    
    }gnu_hash_table;
    gnu_hash_table* hashtable =(gnu_hash_table*) &e->buffer[e->linkInfo.gnuHashtable->sh_offset];
    uint32_t  bloom_size = hashtable->bloom_size;
    uint32_t  symoffset = hashtable->symoffset;
    uint32_t  bloom_shift = hashtable->bloom_shift;
    uint32_t  nbuckets = hashtable->nbuckets;
    Elf64_Xword* bloom =(Elf64_Xword*) hashtable->arrs;
    uint32_t* buckets = (uint32_t*)(bloom+bloom_size);
    uint32_t* chain = (uint32_t*)(buckets+nbuckets);
    uint32_t hash = gnu_Hash(symbol);

    /* check in bloom if absent */
    int mask =bloom[(hash/BITS)%bloom_size];
    int bit1=hash%BITS;
    int bit2=(hash>>bloom_shift)%BITS;
    if(1&(mask>>bit1)&(mask>>bit2)){
        Symbol s={0};
        s.undef = true;
        return s;
    }
    Symtab symtab = e->linkInfo.dynamic_symtab;
    Strtab strtab = e->linkInfo.dynamic_strtab;

    for(int idx=buckets[hash%nbuckets];idx>=symoffset;idx++){
        if(idx>=symtab.size){
            ERROR("index greater than symtab size");
        }
        if(symtab.symtab[idx].st_name>=strtab.size){
            ERROR("Could not find associated symbol");
        }
        if((hash|1)==(chain[idx-symoffset]|1)&&strcmp(symbol,&strtab.strtab[symtab.symtab[idx].st_name])==0){
            
            return makeSymbol(symtab.symtab[idx],&strtab.strtab[symtab.symtab[idx].st_name],e);
        }
		if(chain[idx-symoffset]&1){
			break;
		}

    }

    Symbol s={0};
    s.undef = true;
    return s;

}
Symbol hashtableGet(Elf64*e, char* symbol){
    typedef struct ElfHashtable{
       Elf64_Word nbucket; 
       Elf64_Word nchain; 
       Elf64_Word table[]; 
    }ElfHashtable;
    ElfHashtable* h = (ElfHashtable*)&e->buffer[e->linkInfo.hashtable->sh_offset];
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
    s.value  = s.sym.st_value;
    s.type = ELF64_ST_TYPE(s.sym.st_info);
    s.binding = ELF64_ST_BIND(s.sym.st_info);
    s.size = s.sym.st_size;
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
    for(Elf64_Sym* i =symtab;(char*)i<&e->buffer[stab.hdr->sh_offset+stab.hdr->sh_size];i++){
        printf("%d %d %s :%p\n",indexToIndex(e,strtab,i->st_name),i->st_name,&e->buffer[strtab->sh_offset+i->st_name],i->st_value);
    }
    // for(char* i =&e->buffer[strtab->sh_offset];i<&e->buffer[strtab->sh_offset+hdr->sh_size];i+=strlen(i)+1){
    //     printf("d: %s\n",i);
    // }
}
Symtab makeSymtab(Elf64* e,Elf64_Shdr* symtab){
    Symtab s;
    s.hdr  = symtab;
    s.size = symtab->sh_size/sizeof(Elf64_Sym);
    s.symtab = (Elf64_Sym*)&e->buffer[symtab->sh_offset];
    return s; 
}
Strtab makeStrtab(Elf64* e,Elf64_Shdr* strtab){
    Strtab s;
    s.hdr  = strtab;
    s.size = strtab->sh_size;
    s.strtab = &e->buffer[strtab->sh_offset];
    return s; 
}
void retrieveRelSym(Elf64* e,uint64_t sh_link,Symtab* symtab,Strtab* strtab){
    symtab->symtab=NULL;
    strtab->strtab=NULL;
    if(sh_link<0 || sh_link>=e->section_headers_length){
        return;
    }
    *symtab= makeSymtab(e, &e->section_headers[sh_link]);

    uint64_t sym_link =symtab->hdr->sh_link; 
    if(sym_link<0 || sym_link>=e->section_headers_length){
        return;
    }
    *strtab = makeStrtab(e,&e->section_headers[sym_link]);
}


Symbol symLookup(Elf64* e,char* symbol,Vector* libs,int type_class){
    Symbol sym={0};
    sym.undef=true;
    bool exitloop = false;
    for(int i =0;i<libs->length&&!exitloop;i++){
        Elf64* lib  = libs->array[i];
        /* ignore copy relocations */
        if ((type_class & ELF_RTYPE_CLASS_COPY) && lib==e)
	        continue;
        Symbol tsym;

        if(lib->linkInfo.gnuHashtable){
            tsym = gnuHashtableGet(lib,symbol);
        }else if(lib->linkInfo.hashtable){
            tsym = hashtableGet(lib,symbol);
        }else{
            ERROR("NO GNU HASHTABLE FOR REGULAR HASHTABLE FOR LIBRARY: %s",lib->filename);
        }
        
        if(tsym.undef)
            continue;
        /* this line has been copy and pasted + modifed from glibc dl-lookup.c. DEAL WITH IT!!*/
        if ((tsym.sym.st_value == 0 /* No value.  */
            && tsym.sym.st_shndx != SHN_ABS
            && tsym.sym.st_info != STT_TLS)
            || (type_class & (tsym.sym.st_shndx == SHN_UNDEF)))
                continue;

        /* ignore local and other symbols */
        if(tsym.sym.st_other==STV_HIDDEN||tsym.sym.st_other==STV_INTERNAL)
            continue;
        
        switch(tsym.binding){
            case STB_WEAK:
                if(!sym.undef){
                    sym=tsym;
                }
                break;
            case STB_GLOBAL:
                sym=tsym;
                exitloop = true;
                break;
            case STB_GNU_UNIQUE:
                ERROR("NOT IMPLEMETED THIS YET STB_GNU_UNIQUE NVM I WILL NEVER IMPLEMENT THIS THIS IS USELESS");
            default:
                break;


        }

        
    }
    return sym;

}
Symbol makeSymbol(Elf64_Sym sym,char* str,Elf64* e){
    Symbol s;
    s.str =str;
    s.undef=false;
    s.sym = sym;
    s.object = e;
    s.value  = sym.st_value;
    s.type = ELF64_ST_TYPE(sym.st_info);
    s.binding = ELF64_ST_BIND(sym.st_info);
    s.size = sym.st_size;
    return s;
}
//returns size of relocation in bytes.
void performRelocation(Elf64* e,Elf64_Addr r_offset,uint64_t r_info, int64_t r_addend,uint64_t sh_link,Vector* loadedLibs){
    //s = symbol value
    //a = addend
    //p = r_offset + section address (sh_link)
    //b = shared object load location (during runtime)
    //GOT = .got address
    //L = DONT USE, R_X86_64_PLT32 IS R_X86_64_PC32 (S+A-P)
    //z = size of symbol
    //G = swear at user

    Symtab symtab;
    Strtab strtab;

    byte* reloc_addr = r_offset+e->runtimeInfo.base;
    retrieveRelSym(e,sh_link,&symtab,&strtab);
    
    char* symstr = &strtab.strtab[symtab.symtab[ELF64_R_SYM(r_info)].st_name];
    Elf64_Sym rsym = symtab.symtab[ELF64_R_SYM(r_info)];
    Symbol sym = makeSymbol(rsym,symstr,e);
    uint32_t type_class =elf_machine_type_class(ELF64_R_TYPE(r_info));
    if(ELF64_R_SYM(r_info)>strtab.size){
        ERROR("Symbol index too large!");
    }
    /* this line has been copy and pasted + modifed from glibc dl-lookup.c. DEAL WITH IT!!*/
    if ((rsym.st_value == 0 /* No value.  */
        && rsym.st_shndx != SHN_ABS
        && rsym.st_info != STT_TLS && ELF64_R_SYM(r_info)!=0)
        || type_class & (rsym.st_shndx == SHN_UNDEF) ||type_class&ELF_RTYPE_CLASS_COPY)
        sym = symLookup(e,symstr,loadedLibs,elf_machine_type_class(ELF64_R_TYPE(r_info)));

    size_t s = &sym.object->runtimeInfo.base[sym.value];
    if(sym.undef)
		printf("undefined symbol, relocation failed: %s\n",symstr);
        s=0x6969;
    
    size_t a = r_addend;
    if(sh_link>=e->section_headers_length)
        ERROR("ok who allowed this error to happen. this will literally never happen");
    size_t p =r_offset+e->section_headers[sh_link].sh_addr;
    byte* b = sym.object->runtimeInfo.base;
    size_t got=0;
    if(e->linkInfo.got)
        got = e->linkInfo.got->sh_addr;
    size_t z = sym.size;
    #define reloc(value,size) (*(uint##size##_t*) reloc_addr= (uint##size##_t)value)

    //https://www.intezer.com/blog/malware-analysis/executable-and-linkable-format-101-part-3-relocations/
    switch(ELF64_R_TYPE(r_info)){
        case R_X86_64_64:
            reloc(s+a,64);
            break;
        case R_X86_64_PLT32:
        case R_X86_64_PC32:
            reloc(s+a,32);
            break;
        case R_X86_64_COPY:
            memcpy(reloc_addr,s,z);
            break;
        case R_X86_64_JUMP_SLOT:
        case R_X86_64_GLOB_DAT:
            reloc(s,64);
            break;
        case R_X86_64_RELATIVE:
            reloc(b+a,64);
            break;  
        case R_X86_64_32S:
        case R_X86_64_32:
            reloc(s+a,32);
            break;
        case R_X86_64_16:
            reloc(s+a,16);
            break;
        case R_X86_64_PC16:
            reloc(s+a-p,16);
            break;
        case R_X86_64_8:
            reloc(s+a,8);
            break;
        case R_X86_64_PC8:
            reloc(s+a-p,8);
            break;
        case R_X86_64_PC64:
            reloc(s+a-p,64);
            break;
        case R_X86_64_GOTOFF64:
            reloc(s+a-got,64);
            break;
        case R_X86_64_GOTPC32:
            reloc(got+a-p,32);
            break;
        case R_X86_64_SIZE32:
            reloc(z+a,32);
            break;
        case R_X86_64_SIZE64:
            reloc(z+a,64);
            break;
		case R_X86_64_IRELATIVE:;
			uint64_t v = s+a;
			v = ((Elf64_Addr(*) (void))v)();
			reloc(v,64);
			break;
		default:
			printf("UNKNOWNN RELOCATION");
    }
    return;
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
        }if(e->section_headers[i].sh_type==SHT_GNU_HASH){
            linkInfo->gnuHashtable = &e->section_headers[i];
        }
        
    }
    stripVector(&rels);
    e->linkInfo.rels = (Elf64_Shdr**)rels.array;
    e->linkInfo.rels_length = rels.length;

}

void linkElf(Elf64* e,Vector* loadedLibs){
    Elf64_Shdr** rels = e->linkInfo.rels;
    for(int i =0;i<e->linkInfo.rels_length;i++){
        if(rels[i]->sh_type==SHT_REL){
            for(Elf64_Rel* p =(Elf64_Rel*) &e->buffer[rels[i]->sh_offset];(char*)p<&e->buffer[rels[i]->sh_offset+rels[i]->sh_size];p++){
                performRelocation(e,p->r_offset,p->r_info,0,rels[i]->sh_link,loadedLibs);
            }
        }else if(rels[i]->sh_type==SHT_RELA){
            for(Elf64_Rela* p =(Elf64_Rela*) &e->buffer[rels[i]->sh_offset];(char*)p<&e->buffer[rels[i]->sh_offset+rels[i]->sh_size];p++){
                performRelocation(e,p->r_offset,p->r_info,p->r_addend,rels[i]->sh_link,loadedLibs);
            }

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
	Elf64_Shdr* sect;
    for(int i=0; dyn_array[i].d_tag!=DT_NULL&&i*sizeof(*dyn_array)<dyn->sh_size; i++){
        switch(dyn_array[i].d_tag){
            case DT_NEEDED:
                appendVector(&needed,(void*)dyn_array[i].d_un.d_val);
                break;
            case DT_SYMTAB:
                sect = sectionByAddr(e,dyn_array[i].d_un.d_ptr);
                Symtab symtab;
                symtab.size = sect->sh_size/sizeof(Elf64_Sym);
                symtab.symtab =(Elf64_Sym*) &e->buffer[sect->sh_offset];
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
        e->linkInfo.needed_libraries[i] =&e->buffer[e->linkInfo.dynamic_strtab.hdr->sh_offset+(intptr_t) e->linkInfo.needed_libraries[i] ];
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
bool checkAdded(void* elf,void* name){
	return strcmp(((Elf64*)elf)->filename,(char*)name)==0;
}
bool checkLibraries(void* a,void* b){
	return strcmp((char*)a,(char*)b)==0;
}

//returns number of added
void addLibrariesToQueue(Elf64* e,Vector* addedLibs, Queue* global_needed_libraries){
    size_t added = 0;
    char** needed = e->linkInfo.needed_libraries;
    for(int i =0;i<e->linkInfo.needed_libraries_length;i++){
	    char* libname = needed[i];
	    if(inVector(addedLibs,libname,checkAdded)||inQueue(global_needed_libraries,libname,checkLibraries))
		continue;
	    enQueue(global_needed_libraries,libname);

    }
}

bool libraryEq(void* elf,void* str){
    return strcmp(((Elf64*)elf)->filename,(char*)str)==0;
}
void loadElf(Elf64* e,Vector* loadedLibs,Queue* global_needed_libraries){
   size_t allocSize=0;
    for(int i =0;i<e->section_headers_length;i++){
        if(e->section_headers[i].sh_addr&& inSegments(e,&e->section_headers[i])){
            allocSize= max(allocSize,e->section_headers[i].sh_addr+e->section_headers[i].sh_size);
        }
    }


    char* runtime = largeAlloc(allocSize);
    e->runtimeInfo.base = runtime;
    printf("%s: %p\n",e->filename,runtime);
    for(int i =0;i<e->segments_length;i++){
        Segment seg= e->segments[i];
        if(e->program_headers[i].p_type==PT_LOAD){
            for(int k=0;k<seg.length;k++){
                loadSection(runtime,seg.sections[k],e);
            }   
        }
    }
    addLibrariesToQueue(e,loadedLibs,global_needed_libraries);
}

void freeElf(Elf64* elf){
    free(elf->buffer);
    for(int i =0;i<elf->segments_length;i++){
        if(elf->segments[i].sections){
            free(elf->segments[i].sections);
        }
    }
    free(elf->segments);
    free(elf->program_headers);
    free(elf->section_headers);

}
