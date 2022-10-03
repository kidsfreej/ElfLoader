#include "datastructures.h"
//https://stackoverflow.com/a/7666577/15284796
uint64_t djb2hash(unsigned char *str)
{
    uint64_t hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}
Vector initVector(){
    Vector v;
    v.array = malloc(10*sizeof(void*));
    v.length = 0;
    v._priv_length = 10;
    return v;
}
Vector initVectorSize(size_t size){
    Vector v;
    v.array = malloc(size*sizeof(void*));
    v.length = 0;
    v._priv_length = size;
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
void freeVectorContents(Vector* v,bool freeContents){
    for(int i =0;freeContents&&i<v->length;i++){
        free(v->array[i]);
    }
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
void freeHashtable(Hashtable* h,bool freeValues){
    Vector* arr[] = {h->hash_table,h->key_table,h->value_table};
    for(size_t i =0;i<h->_table_size;i++){
    
        for(size_t k =0;k<sizeof_array(arr);k++){
            if(!arr[k][i].array)
                continue;
            freeVectorContents(&arr[k][i],k==1||(freeValues&&k==2));
        }
    }
}
void setHashtable(Hashtable* h,char* key,void* value){
    if(h->_filled_buckets>=h->_table_size*3/4){
        // resizeHashmap(h);
    }
    Vector* arr[] = {h->hash_table,h->key_table,h->value_table};
    uint64_t hash=  djb2hash(key);
#define mhash hash%h->_table_size

    if(!arr[0][mhash].array){
        for(size_t i = 0;i<sizeof_array(arr);i++)
            arr[i][mhash] = initVectorSize(2);
        h->_filled_buckets++;   
    }
    
    appendVector(&h->hash_table[mhash],hash);
    appendVector(&h->value_table[mhash],hash);
    char* key_copy = malloc(strlen(key));
    strcpy(key_copy,key);
    appendVector(&h->value_table[mhash],key_copy);
    h->size++;
    
#undef mhash
}
// void resizeHashtable(Hashtable* h){

//     Hashtable newh;
//     newh  =initHashtableSize(h->_power+1);
//     newh.size=h->size;
//     for(size_t i =0;i<h->_table_size;i++){
//         newh.
//     }


//     *h =newh;
// }
Hashtable initHashtable(){
    Hashtable h;
    h.size = 0;
    h._power = 3;
    h._table_size=7;
    h._filled_buckets=0;
#define inittable(a)     h.a = malloc(sizeof(*h.a)*h._table_size);memset(h.a,0,sizeof(*h.a)*h._table_size)
    inittable(value_table);
    inittable(key_table);
    inittable(hash_table);
#undef inittable
    return h;
}
Hashtable initHashtableSize(size_t power){
    Hashtable h;
    h.size = 0;
    h._filled_buckets=0;
    h._power = power;
    h._table_size=(2<<power)-1;
#define inittable(a)     h.a = malloc(sizeof(*h.a)*h._table_size);memset(h.a,0,sizeof(*h.a)*h._table_size)
    inittable(value_table);
    inittable(key_table);
    inittable(hash_table);
#undef inittable
    return h;
}