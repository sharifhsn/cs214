#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "mymalloc.h"

/// Size of the header and footer added together
const int PADDING = sizeof(int) + sizeof(void *) + sizeof(void *) + sizeof(int);

/// 1 MB of heap size in bytes
const int HEAPSIZE = 1 << 20;

/// Add to "header" pointer to get address of "next" pointer.
const int NEXT_D = sizeof(int);

/// Add to "header" pointer to get address of "prev" pointer.
const int PREV_D = sizeof(int) + sizeof(void *);

/// Size of the "footer" at the end of each block
const int FOOTSIZE = sizeof(int);


/// pointer to the first byte of the heap
static struct Head *heap;

/// pointer to the first free block in the explicit free list
static struct Head *root;

/**
 * Allocation algorithm:
 * - 0: first-fit
 * - 1: next-fit
 * - 2: best-fit
 */
static int alloc_alg;

/// header structure to place at the beginning of node in the explicit free list
struct Head {
    int size; // 4 bytes
    int valid; // 13 if free, 1989 if allocated, 875 if root
    struct Head *next; // 8 bytes
    struct Head *prev; // 8 bytes
} Head;


void myinit(int allocAlg) {
    // alloc_alg = allocAlg;
    // heap = malloc(HEAPSIZE);
    // front = heap;
    // *front = HEAPSIZE;
    // *(front + NEXT_D) = 0x0;
    // *(front + PREV_D) = 0x0;
    // *(front + *front - FOOTSIZE) = *front;
    // void *ptr = front;
    // for (int i = 0; i < HEAPSIZE; i++, ptr++) {
    //     printf("pointer is: %p\n", ptr);
    // }
    // printf("top pointer is %p\n", front);

    alloc_alg = allocAlg;
    heap = malloc(HEAPSIZE);
    root = malloc(sizeof(Head));
    root->size = 0;
    root->valid = 875;

    root->next = heap;
    root->next->size = HEAPSIZE;
    root->next->valid = 13;
    root->next->next = NULL;
    root->next->prev = root;
    *((int *) (root->next + root->next->size - FOOTSIZE)) = HEAPSIZE;
    printf("front is: %p\n", root->next);
    printf("payload addr is: %p\n", root->next + sizeof(Head));
    printf("head size is: %ld\n", sizeof(Head));
}

// void split(int *ptr, size_t size) {
//     int *tnext = *(ptr + 1);
//     int *tprev = *(ptr + 2);
// }

void *mymalloc(size_t size) {
    // if (size == 0) {
    //     return NULL;
    // }
    // if (alloc_alg == 0) {
    //     for (int *ptr = front; ptr != NULL; ptr = *(ptr + NEXT_D)) {
    //         // amount to allocate fits in given block
    //         if (size <= *ptr) {
    //             // find an address that aligns with 8
    //             for (int *aptr = ptr + HEADSIZE; aptr < ptr + *ptr - FOOTSIZE; aptr++) {
    //                 if (*aptr % 8 == 0) {
                        
    //                 }
    //             }
    //         }
    //     }
    // }

    // return front;

    if (size == 0) {
        return NULL;
    }

    printf("allocate %ld bytes\n", size);

    // first fit
    if (alloc_alg == 0) {
        for (struct Head *ptr = root->next; ptr != NULL; ptr = ptr->next) {
            // amount to allocate fits in given block
            if (size <= ptr->size) {
                // find an address that fits alignment
                void *aptr = ptr;
                while (((uintptr_t) aptr) % 8 != 0) {
                    aptr++;
                }
                
                // check if aligned block can fit allocation
                if ((ptr->size - (((uintptr_t) aptr) - ((uintptr_t) ptr))) < size) {
                    continue;
                } else {
                    // split free block
                    struct Head *h = aptr + size;
                    printf("aptr is %p, h is %p\n", aptr, h);
                    h->size = ptr->size - size;
                    h->valid = 13;
                    h->prev = ptr->prev;
                    if (ptr->prev != NULL) {
                        ptr->prev->next = h;
                    }
                    h->next = ptr->next;
                    if (ptr->next != NULL) {
                        ptr->next->prev = h;
                    }
                    *((int *) (h + h->size - FOOTSIZE)) = ptr->size - size;
                    printer();
                    ptr->valid = 1989;
                    return ptr;
                }
            }
        }
    }
    return NULL;
}

void printer() {
    printf("explicit free list: ");
    for (struct Head *ptr = root->next; ptr != NULL; ptr = ptr->next) {
        printf("%p (%d) -> ", ptr, ptr->size);
    }
    printf("\n");
}


void myfree(void *ptr) {

    if (ptr == NULL) {
        return;
    }

    // not in heap
    if (ptr < heap || ptr > (heap + HEAPSIZE)) {
        printf("error: not a heap pointer\n");
        return;
    }

    // not a malloced address
    struct Head *aptr = ptr;
    if (!(aptr->valid == 1989 || aptr->valid == 13 || aptr->valid == 875)) {
        printf("error: not a malloced address\n");
        return;
    }

    // double free
    if (aptr->valid == 13) {
        printf("error: double free\n");
        return;
    }

    // case 4: coalescing on both ends
    

}

void *myrealloc(void *ptr, size_t size) {
    return 0;
}

void mycleanup() {
    free(heap);
}

double utilization() {
    return 0.0;
}