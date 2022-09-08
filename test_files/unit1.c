#include <stdio.h>
int thesymbol = 100;
extern void print2();
int main(){
    printf("%d\n",thesymbol);
    print2();
    return 0;
}