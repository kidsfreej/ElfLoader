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
size_t mod(size_t a,size_t b){
    int m = a%b;
    return m<0?a+b:m;
}
Queue initQueue(){
    Queue q;
    q.length=0;
    q._priv_length=10;
    q._start=0;
    q._array=malloc(sizeof(*q._array)*q._priv_length);
    return q;
}
void enQueue(Queue* q,void* value){
    if(q->length==q->_priv_length){
        void** new_arr =malloc(q->_priv_length*sizeof(*q->_array)*1.5);
        size_t end = mod(q->_start+q->length,q->_priv_length);
        int k =0;
        for(int i = q->_start;i!=end||k==0;i++,k++,i=mod(i,q->_priv_length)){
            new_arr[k]=q->_array[i];
        }
        free(q->_array);
        q->_array = new_arr;
        q->_priv_length*=1.5;
        q->_start=0;
    }
    q->_array[(q->_start+q->length)%q->_priv_length]=value;
    q->length++;
}
void* deQueue(Queue* q){
    void* r =q->_array[mod(q->_start,q->_priv_length)];
    q->_start+=1;
    q->length-=1;
    q->_start%=q->_priv_length;
    return r;
}
void* getQueue(Queue* q,size_t index){
    return q->_array[mod(q->_start+index,q->_priv_length)];
}
void freeQueue(Queue* q){
    free(q->_array);
}

bool inQueue(Queue* q,void* value,compare_t f){
	for(int i =0;i<q->length;i++){
		void* v = getQueue(q,i);
		if(f(v,value)){
			return true;
		}
	}
	return false;
}
