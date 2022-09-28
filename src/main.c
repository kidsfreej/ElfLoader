#include "elfloader.h"
#include <stdio.h>
#include "datastructures.h"
int main(){
    Queue q = initQueue();
    for(int i =0;i<20;i++){
        enQueue(&q,i);

    }
	for(int i =0;i<15;i++){
		printf("%d\n",deQueue(&q));
	}
	enQueue(&q,5);
	for(int i =0;i<6;i++){
		printf("%d\n",deQueue(&q));
	}
   elfLoad("helloworld");
//	printf("%p",largeAlloc(1000));
    return 0;
}
