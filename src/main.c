#include "elfloader.h"
#include <stdio.h>
#include "datastructures.h"
int main(){
    Queue q = initQueue();
    for(int i =0;i<20;i++){
        enQueue(&q,i);

    }
    elfLoad("a.out");

    return 0;
}