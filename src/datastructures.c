#include "datastructures.h"

Vector initVector(){
    Vector v;
    v.array = malloc(10*sizeof(void*));
    v.length = 0;
    v._priv_length = 10;
    return v;
}
void appendVector(Vector* v, void* value){
    v->length++;
    if(v->length>v->_priv_length){
        v->array = realloc(v->array,v->_priv_length*1.5*sizeof(void*));
        v->_priv_length = v->_priv_length*1.5;
    }
    v->array[v->length-1]=value;

}
void freeVector(Vector* v){
    free(v->array);
    v->array=0;
    v->length=0;
    v->_priv_length=0;
}
void stripVector(Vector* v){
    v->_priv_length = v->length;
    if(v->array){
        v->array = realloc(v->array,v->_priv_length*sizeof(*v->array));
    }else{
        v->array = NULL;
    }
}
bool inVector(Vector* v,void* p,compare_t func){
    for(int i =0;i<v->length;i++){
        if(func(v->array[i],p)){
            return true;
        }
    }
    return false;
}