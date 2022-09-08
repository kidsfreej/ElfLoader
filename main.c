#include <stdio.h>
#include <stdint.h>
#include <memory.h>
#include <string.h>
#include "buffer.h"
#include "elfloader.h"
// spports only .hash not .gnu.hash
// compile with  gcc -Xlinker --hash-style -Xlinker sysv
int main(){

	// LPVOID a = largeAlloc(main,1000);
	// if(!a){
	// 	printf("BRUH!!!");
	// }
	// printf("bruuuhh");
	// Vector v=initVector();
	// for(int i =0;i<100;i++){
	// 	appendVector(&v,100);
	// }
	// freeVector(&v);
	elfLoad("test");
	// for(int i =0;i<e.section_headers_length;i++){


	return 0;
}