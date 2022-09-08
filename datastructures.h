#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
typedef bool (*compare_t)(void*,void*);
typedef struct {
    size_t length;
    void** array;
    size_t _priv_length;
}Vector;

void appendVector(Vector* v, void* value);
Vector initVector();
void freeVector(Vector* v);
void stripVector(Vector* v);
bool inVector(Vector* v,void* p,compare_t func);