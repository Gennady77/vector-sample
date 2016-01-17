#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

void VectorNew(vector *v, int elemSize, VectorFreeFunction freeFn, int initialAllocation)
{
    assert(elemSize > 0);
    v->elemSize = elemSize;
    v->logLength = 0;
    v->allocLength = initialAllocation;
    v->elems = malloc(initialAllocation * elemSize);
    v->freefn = freeFn;
    assert(v->elems != NULL);
}

void VectorDispose(vector *v)
{
    int i;
    v->logLength = 0;

    if(v->freefn !=NULL) {
        for(i = 0; i < VectorLength(v); i++) {
            v->freefn(VectorNth(v, i));
        }
    } else {
        free(v->elems);
    }
}

int VectorLength(const vector *v)
{
    return v->logLength;
}

void *VectorNth(const vector *v, int position)
{
    assert(position >= 0 && position < VectorLength(v));
    return (char *)v->elems + position * v->elemSize;
}

void VectorReplace(vector *v, const void *elemAddr, int position)
{
    assert(position >= 0 && position < VectorLength(v));
    if(v->freefn != NULL) {
        v->freefn(VectorNth(v, position));
    }
    memcpy(VectorNth(v, position), elemAddr, v->elemSize);
}

void VectorInsert(vector *v, const void *elemAddr, int position)
{
    assert(position >= 0 && position <= VectorLength(v));
    if(VectorLength(v) == v->allocLength) {
        v->allocLength *= 2;
        v->elems = realloc(v->elems, v->allocLength);
    }
    void *dest = (char *)v->elems + (position + 1) * v->elemSize;
    void *src = (char *)v->elems + position * v->elemSize;
    memmove(dest, src, (VectorLength(v) - position) * v->elemSize);
    memcpy(src, elemAddr, v->elemSize);
    v->logLength++;
}

void VectorAppend(vector *v, const void *elemAddr)
{
    void *destAddr;
    if(VectorLength(v) == v->allocLength) {
        v->allocLength *= 2;
        v->elems = realloc(v->elems, v->allocLength * v->elemSize);
        assert(v->elems != NULL);
    }
    destAddr = (char *)v->elems + VectorLength(v) * v->elemSize;
    memcpy(destAddr, elemAddr, v->elemSize);
    v->logLength++;
}

void VectorDelete(vector *v, int position)
{
    assert(position >= 0 && position < VectorLength(v));
    void *dest = (char *)v->elems + position * v->elemSize;
    void *src = (char *)v->elems + (position + 1) * v->elemSize;
    memmove(dest, src, (VectorLength(v) - position - 1) * v->elemSize);
    v->logLength--;
}

void VectorSort(vector *v, VectorCompareFunction compare)
{
    assert(compare != NULL);
    qsort(v->elems, VectorLength(v), v->elemSize, compare);
}

void VectorMap(vector *v, VectorMapFunction mapFn, void *auxData)
{
    int i;
    assert(mapFn != NULL);
    for(i=0; i < VectorLength(v); i++) {
        mapFn(VectorNth(v, i), auxData);
    }
}

static const int kNotFound = -1;
int VectorSearch(const vector *v, const void *key, VectorCompareFunction searchFn, int startIndex, bool isSorted)
{
    int i;
    assert(startIndex >= 0 && startIndex < VectorLength(v));
    assert(searchFn != NULL);
    if(isSorted) {
        void *bres = bsearch(key, v->elems, VectorLength(v), v->elemSize, searchFn);
        if(bres == NULL) return kNotFound;
        return ((char *) bres - (char *) v->elems) / v->elemSize;
    } else {
        for(i = startIndex; i < VectorLength(v); i++) {
            if(searchFn(key, VectorNth(v, i)) == 0) {
                return i;
            }
        }
        return kNotFound;
    }
} 
