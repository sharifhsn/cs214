typedef struct Head {
    int size;
    int valid;
    struct Head *next;
    struct Head *prev;
} Head;

/**
 * Create a 1 MB "heap" and perform any other initializations your code needs.
 * You can assume that any application using your library will call this first.
 * 
 * The `allocAlg` argument describes what algorithm to use to find a free block
 * (see Lec. 17, p. 19):
 * 0: first
 * 1: next fit
 * 2: best fit
 */
void myinit(int allocAlg);

/**
 * From the "heap", allocate a region of at least the requested `size` and
 * return a pointer to the beginning of the region. If it cannot be allocated,
 * return `NULL`.
 * 
 * All returned addresses must be 8-byte aligned. That is, the region you
 * allocate should start at an address that's divisible by 8.
 * 
 * If `size` is 0, `mymalloc` does nothing and returns `NULL`.
 */
void *mymalloc(size_t size);

/**
 * Mark the given region as free and available to be allocated for future
 * requests. It should be coalesced with adjacent free regions.
 * 
 * You should maintain an explicit free list.
 * 
 * If `ptr` is `NULL`, `myfree` does nothing.
 * 
 * You should detect a number of error conditions:
 * - trying to free an invalid address
 * If the address given isn't in your "heap", print "error: not a heap pointer".
 * (For example, if your "heap" ran from 0x4000 to 0x6000, but the `ptr` given 
 * to `myfree` was 0x10.)
 * If the address given is in your "heap" but wasn't returned by `mymalloc`,
 * print "error: not a malloced address". (For example, if `mymalloc` returns
 * 0x4040, but the `ptr` given to `myfree` was 0x4041.)
 * - double free
 * If the address given was returned by `mymalloc` but has already been freed,
 * print "error: double free".
 */
void myfree(void *ptr);

/**
 * Reallocate the region pointed to by `ptr` to be at least the new given
 * `size`. If this cannot be done in-place, a new region should be allocated,
 * the data from the original region should be copied over, and the old region
 * should be freed.
 * 
 * If the reallocation can't be done, return `NULL`.
 * 
 * If `ptr` is `NULL`, this is equivalent to `mymalloc(size)`.
 * 
 * If `size` is 0, this is equivalent to `myfree(ptr)` and `myrealloc` returns
 * `NULL`.
 * 
 * If both `ptr` is `NULL` and `size` is 0, `myrealloc` does nothing and returns
 * `NULL`.
 */
void *myrealloc(void *ptr, size_t size);

/**
 * Free the 1 MB "heap" and perform any other cleanup your code needs. You can
 * assume any application using your library will call this last.
 * 
 * Your library should support "resetting" everything by calling `mycleanup`
 * followed by `myinit`, since the performance testing program will need to do
 * this.
 * 
 * Blocks that have been freed should not count towards memory used. The space
 * used in the "heap" is the distance from the beginning of the "heap" to the
 * end of the last allocated block.
 */
void mycleanup();

/**
 * Calculate space utilization as a ratio of memory used vs. space used in the
 * "heap". For example, if you called `mymalloc(50)` followed by `mymalloc(64)`
 * and determined that you needed 128 bytes on the "heap" to satisfy these
 * requests, the ratio would be
 * (50 + 64)/128 ≈ 0.89.
 * 
 */
double utilization();


/**
 * Given a `ptr` to the first part of the header, and `size` the amount to split
 * the block with, modifies explicit free list.
 */
void split(int *ptr, size_t size);

/**
 * @brief Print explicit free list
 * 
 */
void printer();

/**
 * @brief Find the best fitting block associated with the allocation algo
 * 
 */
Head *find_fit(size_t size);