#include "hashset.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void HashSetNew(hashset *h, int elemSize, int numBuckets,
		HashSetHashFunction hashfn, HashSetCompareFunction comparefn, HashSetFreeFunction freefn)
{
	assert(numBuckets > 0 && elemSize > 0 && hashfn != NULL && comparefn != NULL);
	h->cache = (vector*)malloc(numBuckets * sizeof(vector));
	assert(h->cache != NULL);
	h->bucketNum = numBuckets;
	h->compare = comparefn;
	h->hash = hashfn;
	h->size = 0;
	//initialize vectors in each bucket
	for(int i = 0; i < numBuckets; i++){
		vector v;
		VectorNew(&v, elemSize, freefn, 0);
		memcpy(h->cache + i, &v, sizeof(vector));
	}
}

void HashSetDispose(hashset *h){
	for(int i = 0; i < h->bucketNum; i ++){
		VectorDispose(h->cache + i);
	}
	free(h->cache);
}

int HashSetCount(const hashset *h){ 
	return h->size; 
}

void HashSetMap(hashset *h, HashSetMapFunction mapfn, void *auxData){
	for(int i = 0; i < h->bucketNum; i++){
		VectorMap(h->cache + i, mapfn, auxData);
	}
}

void HashSetEnter(hashset *h, const void *elemAddr){
	assert(elemAddr != NULL);
	int hashCode = h->hash(elemAddr, h->bucketNum);
	assert(hashCode >= 0 && hashCode < h->bucketNum);
	vector* vp = h->cache + hashCode;
	int pos = VectorSearch(vp,elemAddr, h->compare, 0, true);
	if(pos != -1) VectorReplace(vp, elemAddr, pos); //element has been found and should be replaced
	else{
		h->size++;
		int length = VectorLength(vp);
		//if its biggest element and/or first element in the vector
		if(length == 0 || h->compare(elemAddr, VectorNth(vp, length - 1)) > 0) VectorAppend(vp, elemAddr); 
		else {
			//find the index where this element belongs by the order
			for(int i = 0 ; i < VectorLength(vp); i++){
				if(h->compare(elemAddr, VectorNth(vp, i)) < 0) {
					VectorInsert(vp, elemAddr, i);
					return;
				}
			}
		}
	}
}

void *HashSetLookup(const hashset *h, const void *elemAddr){
	assert(elemAddr != NULL);
	int hashCode = h->hash(elemAddr, h->bucketNum);
	assert(hashCode >= 0 && hashCode < h->bucketNum);
	int pos = VectorSearch(h->cache + hashCode, elemAddr, h->compare, 0, true);
	if(pos != -1) return VectorNth(h->cache + hashCode, pos);
	else return NULL;
}