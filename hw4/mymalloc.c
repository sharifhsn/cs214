#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "mymalloc.h"

#define HEAPSIZE (1 << 20)
#define FOOTSIZE (sizeof(unsigned long))
#define PADDING (sizeof(Head) + sizeof(unsigned long))
#define VIT(p) ((void *) p)
#define FOOT(h) (*(unsigned long *) (VIT(h) + h->size - FOOTSIZE))


static void *heap;
static struct Head *root;

static int alloc_alg;

void myinit(int allocAlg) {
    alloc_alg = allocAlg;
    heap = malloc(HEAPSIZE);
    root = malloc(sizeof(Head));
    root->size = 0;
    root->valid = 875;

    struct Head *h = heap;
    h->size = HEAPSIZE;
    h->valid = 13;
    h->next = NULL;
    h->prev = root;
    FOOT(h) = HEAPSIZE;
    root->next = h;
    root->prev = NULL;

    printf("front is: %p\n", h);
    //printf("size is %lu\n", *((int *) VIT(h) + h->size - FOOTSIZE));
    printf("end of heap: %p\n", VIT(h) + h->size);
    printf("%d\n", HEAPSIZE);
}

void *mymalloc(size_t size) {
    if (size == 0) {
        return NULL;
    }
    // align size
    if (size % 8 != 0) {
        size += (8 - size % 8);
    }
    struct Head *blk = find_fit(size + PADDING);
    struct Head *h = VIT(blk) + size + PADDING;
    h->valid = 13;
    h->size = blk->size - size - PADDING;
    h->next = blk->next;
    h->prev = blk->prev;
    if (blk->next != NULL) {
        blk->next->prev = h;
    }
    if (blk->prev != NULL) {
        blk->prev->next = h;
    }
    // printf("pointer of h is %p\n", h);
    // printf("size of size: %d\n", h->size);
    // printf("some pointer %p\n", VIT(h) + h->size - FOOTSIZE);
    FOOT(h) = h->size;

    blk->size = size + PADDING;
    blk->valid = 1989;
    FOOT(blk) = blk->size;

    return VIT(blk) + sizeof(Head);
}

Head *find_fit(size_t size) {
    if (alloc_alg == 0) {
        for (Head *ptr = root->next; ptr != NULL; ptr = ptr->next) {
            if (ptr->valid == 13 && ptr->size >= size + PADDING) {
                return ptr;
            }
        }
    }
    return NULL;
}

void myfree(void *ptr) {
    if (ptr < heap || ptr > (VIT(heap) + HEAPSIZE)) {
        printf("error: not a heap pointer\n");
        return;
    }
    struct Head *h = ptr - sizeof(Head);
    if (h->valid == 13) {
        printf("error: double free\n");
        return;
    }
    if (h->valid != 1989) {
        printf("not a malloced address\n");
        return;
    }

    // don't worry about coalescing for now
    h->valid = 13;
    h->next = root->next;
    root->next = h;
}

void *myrealloc(void *ptr, size_t size) {
    return NULL;
}

void mycleanup() {
    free(heap);
}

double utilization() {
    return 0.0;
}
