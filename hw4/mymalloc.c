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
static double usage;
static struct Head *last_alloc;
static struct Head *last_searched;

void myinit(int allocAlg) {
    alloc_alg = allocAlg;
    heap = malloc(HEAPSIZE);
    root = malloc(sizeof(Head));
    root->size = 0;
    root->valid = 875; 
    last_alloc = heap;

    //printf("heap is %p\n", heap);


    struct Head *h = heap;
    h->size = HEAPSIZE;
    h->valid = 13;
    h->next = NULL;
    h->prev = root;
    FOOT(h) = HEAPSIZE;
    root->next = h;
    root->prev = NULL;

    last_searched = root->next;

    //printf("front is: %p\n", h);
    printf("end of heap: %p\n", VIT(h) + h->size);
    //printf("%d\n", HEAPSIZE);
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
    if (blk == NULL) {
        return NULL;
    }
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
    last_searched = h;

    //printf("usage adding: %d\n", blk->size);
    usage += blk->size;
    if (blk >= last_alloc) {
        last_alloc = blk;
        //printf("new last_alloc is %p\n", last_alloc);
    }

    return VIT(blk) + sizeof(Head);
}

Head *find_fit(size_t size) {
    if (alloc_alg == 0) {
        for (Head *ptr = root->next; ptr != NULL; ptr = ptr->next) {
            // printf("pointer is %p size %d valid %d\n", ptr, ptr->size, ptr->valid);
            if (ptr->valid == 13 && ptr->size >= size + PADDING) {
                return ptr;
            }
        }
    } else if (alloc_alg == 1) {
        for (Head *ptr = last_searched; ptr != NULL; ptr = ptr->next) {
            //printf("considering %p\n", ptr);
            if (ptr->valid == 13 && ptr->size >= size + PADDING) {
                return ptr;
            }
            if (ptr == NULL) {
                ptr = root->next;
            }
        }
    } else {
        Head *best_ptr = NULL;
        int best_sz = HEAPSIZE + 1;
        for (Head *ptr = root->next; ptr != NULL; ptr = ptr->next) {
            if (ptr->valid == 13 && ptr->size >= size + PADDING) {
                if (ptr->size - size - PADDING < best_sz) {
                    best_sz = ptr->size - size - PADDING;
                    best_ptr = ptr;
                }
            }
        }
        return best_ptr;
    }
    //printf("nothing here!\n");
    return NULL;
}

void myfree(void *ptr) {
    if (ptr == NULL) {
        return;
    }
    if (ptr < heap || ptr > (VIT(heap) + HEAPSIZE)) {
        printf("ptr is %p\n", ptr);
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
    usage -= h->size;
    //printf("usage subracting: %d\n", h->size);

    int isp = 0;
    int isn = 0;

    unsigned long *pfoot = (VIT(h) - FOOTSIZE);
    Head *phead = NULL;
    if (pfoot > heap && VIT(pfoot) - *pfoot > heap) {
        phead = VIT(pfoot) - *pfoot;
        if (phead->valid == 13) {
            isp = 1;
        }
    }
    Head *nhead = NULL;
    if ((VIT(h) + h->size) < (VIT(heap) + HEAPSIZE)) {
        nhead = VIT(h) + h->size;
        if (nhead->valid == 13) {
            isn = 1;
        }
    }

    if (isp == 0 && isn == 0) {
        // case 1
        h->valid = 13;
        h->next = root->next;
        root->next = h;
    } else if (isp == 1 && isn == 0) {
        // case 2
        //printf("coalesce back\n");
        if (phead->prev != NULL) {
            phead->prev->next = phead->next;
        }
        if (phead->next != NULL) {
            phead->next->prev = phead->prev;
        }
        phead->size += h->size;
        FOOT(phead) = phead->size;
        phead->next = root->next;
        root->next = phead;
    } else if (isp == 0 && isn == 1) {
        // case 3
        //printf("coalesce forward\n");
        if (nhead->prev != NULL) {
            nhead->prev->next = nhead->next;
        }
        if (nhead->next != NULL) {
            nhead->next->prev = nhead->prev;
        }
        h->valid = 13;
        h->size += nhead->size;
        FOOT(h) = h->size;
        h->next = root->next;
        root->next = h;
    } else if (isp == 1 && isn == 1) {
        if (phead->prev != NULL) {
            phead->prev->next = phead->next;
        }
        if (phead->next != NULL) {
            phead->next->prev = phead->prev;
        }
        if (nhead->prev != NULL) {
            nhead->prev->next = nhead->next;
        }
        if (nhead->next != NULL) {
            nhead->next->prev = nhead->prev;
        }
        phead->size += h->size + nhead->size;
        FOOT(phead) = phead->size;
        phead->next = root->next;
        root->next = phead;
    }

}

void *myrealloc(void *ptr, size_t size) {
    if (ptr == NULL && size == 0) {
        return NULL;
    } else if (ptr == NULL) {
        return mymalloc(size);
    } else if (size == 0) {
        myfree(ptr);
        return NULL;
    }
    myfree(ptr);
    return mymalloc(size);
}

void mycleanup() {
    //printf("Heap was %p\n", heap);
    free(heap);
    heap = NULL;
    free(root);
    root = NULL;
    last_alloc = NULL;
}

double utilization() {
    struct Head *last = root->next;
    double u = 0.0;
    struct Head *sptr = heap;
    while (sptr < (VIT(heap) + HEAPSIZE)) {
        if (sptr->valid == 1989) {
            u += (sptr->size - PADDING);
            last = sptr;
            sptr = VIT(sptr) + sptr->size;
        } else if (sptr->valid == 13) {
            sptr = VIT(sptr) + sptr->size;
        } else {
            sptr = VIT(sptr) + 1;
        }
        //printf("ptr is now %p\n", sptr);
    }
    // unsigned long h = VIT(heap);
    // unsigned long la = VIT(last_alloc);
    // double mem = la + last_alloc->size - h;
    // printf("last_alloc is %p\n", la);
    // return usage / mem;
    double mem = VIT(last) + last->size - VIT(heap);
    //printf("usage: %f, mem: %f\n", u, mem);

    return u / mem;
}
