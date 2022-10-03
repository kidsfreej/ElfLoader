
#ifndef DATASTRUCTURES_H
#define DATASTRUCTURES_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
typedef bool (*compare_t)(void*,void*);
typedef struct Vector{
    size_t length;
    void** array;
    size_t _priv_length;
}Vector;
typedef struct Queue{
    size_t length;
    void** _array;
    size_t _priv_length;
    size_t _start;
}Queue;

//HASHTABLES TAKE OWNERSHIP OF 
typedef struct Hashtable{
    Vector* value_table;
    Vector* key_table;
    Vector* hash_table;
    size_t _table_size;
    size_t size;
    size_t _power;
    int _filled_buckets;
}Hashtable;

#define sizeof_array(a) sizeof(a)/sizeof(a[0])


uint64_t djb2hash(unsigned char*  str);
void appendVector(Vector* v, void* value);
Vector initVector();
Vector initVectorSize(size_t size);
void freeVector(Vector* v);
void stripVector(Vector* v);
bool inVector(Vector* v,void* p,compare_t func);
Queue initQueue();
void enQueue(Queue* q,void* value);
void* deQueue(Queue* q);
void* getQueue(Queue* q,size_t index);
void freeQueue(Queue* q);
bool inQueue(Queue* q,void* value, compare_t f);
void freeVectorContents(Vector* v,bool freeContents);
#endif
