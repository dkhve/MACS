#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <search.h>


static const int defaultCapacity = 4;
void VectorNew(vector *v, int elemSize, VectorFreeFunction freeFn, int initialAllocation){
    assert(elemSize > 0 && initialAllocation >= 0);
    if(initialAllocation == 0) initialAllocation = defaultCapacity;
    v->vec = malloc(initialAllocation * elemSize);
    assert(v->vec != NULL);
    v->capacity = initialAllocation;
    v->elemSize = elemSize;
    v->filledNum = 0;
    v->unload = freeFn;
}

void VectorDispose(vector *v){
    if(v->unload != NULL){
        for(int i = 0 ; i < v->filledNum; i++){
            v->unload((char*)(v->vec) + i * v->elemSize);
        }
    }
    free(v->vec);
}

int VectorLength(const vector *v){
    return v->filledNum;     
}

void *VectorNth(const vector *v, int position){ 
    assert(position >= 0 && position < v->filledNum);    
    void* Nth = (char*)(v->vec) + v->elemSize * position;
    return Nth; 
}

void VectorReplace(vector *v, const void *elemAddr, int position){
    void* Nth = VectorNth(v , position);
    if(v->unload != NULL) v->unload(Nth);
    memcpy(Nth, elemAddr, v->elemSize);
}

static void VectorGrow(vector *v){
    v->capacity *= 2;
    v->vec = realloc(v->vec, v->capacity * v->elemSize);
    assert(v->vec != NULL);
}

void VectorInsert(vector *v, const void *elemAddr, int position){
    assert(position >= 0 && position <= v->filledNum);
    if(v->filledNum == v->capacity) VectorGrow(v);
    //move every element from given position by one to the right, so that there is space for new one
    for(int i = v->filledNum; i > position; i--){
        void* dest= (char*)(v->vec) + v->elemSize * i;
        void* source = (char*)dest - v->elemSize;
        memmove(dest, source, v->elemSize);
    }
    void* ptr = (char*)(v->vec) + position * v->elemSize;
    memcpy(ptr, elemAddr, v->elemSize);
    v->filledNum++;
}

void VectorAppend(vector *v, const void *elemAddr){
    VectorInsert(v, elemAddr, v->filledNum);
}

void VectorDelete(vector *v, int position){
    void* Nth = VectorNth(v, position);
    if(v->unload != NULL) v->unload(Nth);
    //move every element after given position by one to the left so that no gaps are left
    for(int i = position; i < v->filledNum - 1; i++){
        void* dest = (char*)(v->vec) + i * v->elemSize;
        void* source = (char*) dest + v->elemSize;
        memmove(dest, source, v->elemSize);
    }
    v->filledNum--;
}

void VectorSort(vector *v, VectorCompareFunction compare){
    assert(compare != NULL);
    qsort(v->vec, v->filledNum, v->elemSize, compare);
}

void VectorMap(vector *v, VectorMapFunction mapFn, void *auxData){
    assert(mapFn != NULL);
    for(int i = 0 ; i < v->filledNum; i++){
        mapFn((char*)(v->vec) + i * v->elemSize, auxData);
    }
}

static const int kNotFound = -1;
int VectorSearch(const vector *v, const void *key, VectorCompareFunction searchFn, int startIndex, bool isSorted){ 
    assert(key != NULL && searchFn != NULL && startIndex >= 0 && startIndex <= v->filledNum);
    void* elem = NULL;
    if(isSorted){
        elem = bsearch(key, (char*)(v->vec) + startIndex * v->elemSize, v->filledNum - startIndex, v->elemSize, searchFn);
    }else{
        size_t elemNum = v->filledNum - startIndex;
        elem = lfind(key, (char*)(v->vec) + startIndex * v->elemSize, &elemNum , v->elemSize, searchFn);
    }
    int index;
    //if elem!= NULL means that element was found
    if(elem != NULL) index = ((char*)elem - (char*)(v->vec))/v->elemSize;
    else index  = kNotFound;
    return index;
} 