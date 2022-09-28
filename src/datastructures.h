
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
void appendVector(Vector* v, void* value);
Vector initVector();
void freeVector(Vector* v);
void stripVector(Vector* v);
bool inVector(Vector* v,void* p,compare_t func);
Queue initQueue();
void enQueue(Queue* q,void* value);
void* deQueue(Queue* q);
void* getQueue(Queue* q,size_t index);
void freeQueue(Queue* q);
bool inQueue(Queue* q,void* value, compare_t f);
#endif
