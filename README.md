# Memory Allocation

Challenge 1 (Memory utilization):
If we reuse freed memory sectors with bigger sizes than required, we’ll be wasting memory
(internal fragmentation).
Solution: Implement a function that smalloc() will use, such that if a pre-allocated block
is reused and is large enough, the function will cut the block into two smaller blocks with two
separate meta-data structs. One will serve the current allocation, and another will remain
unused for later (marked free and added to the list).
Definition of “large enough”: After splitting, the remaining block (the one that is not used) has
at least 128 bytes of free memory, excluding the size of your meta-data structure.
Note: Once again, you are not requested to find the “best” free block for this section, but the
first block that satisfies the allocation defined above.


• Challenge 2 (Memory utilization):
Many allocations and de-allocations might cause two adjacent blocks to be free, but separate.
Solution: Implement a function that sfree() will use, such that if one adjacent block (next
or previous) was free, the function will automatically combine both free blocks (the current
one and the adjacent one) into one large free block. On the corner case where both the next
and previous blocks are free, you should combine all 3 of them into one large block.


• Challenge 3 (Memory utilization):
Define the “Wilderness” chunk as the topmost allocated chunk. Let’s presume this chunk is
free, and all others are full. It is possible that the new allocation requested is bigger than the
wilderness block, thus requiring us to call sbrk() once more – but now, it is easier to simply
enlarge the wilderness block, saving us an addition of a meta-data structure.
Solution: Change your current implementation, such that if:
1. A new request has arrived, and no free memory chunk was found big enough.
2. And the wilderness chunk is free.
Then enlarge the wilderness chunk enough to store the new request.


• Challenge 4 (Large allocations):
Recall from our first discussion that modern dynamic memory managers not only use sbrk()
but also mmap(). This process helps reduce the negative effects of memory fragmentation
when large blocks of memory are freed but locked by smaller, more recently allocated blocks
lying between them and the end of the allocated space. In this case, had the block been
allocated with sbrk(), it would have probably remained unused by the system for some time
(or at least most of it).
Solution: Change your current implementation, by looking up how you can use mmap()
and munmap() instead of sbrk() for your memory allocation unit. Use this only for
allocations that require 128kb space or more.

Different functions:

void* smalloc(size_t size):
● Searches for a block with ‘size’ bytes or allocates (sbrk()) one if none are found.
● Return value:
i. Success – returns pointer to the first byte in the allocated block (excluding the metadata of course)
ii. Failure –
a. If size is 0 returns NULL.
b. If ‘size’ is more than 108
, return NULL.
c. If sbrk fails in allocating the needed space, return NULL.
2. void* scalloc(size_t num, size_t size):
● Searches for a block of ‘num’ elements, each ‘size’ bytes that are all set to 0 or allocates if
none are found. In other words, find/allocate size * num bytes and set all bytes to 0.
● Return value:
i. Success - returns pointer to the first byte in the allocated block.
ii. Failure –
a. If size is 0 returns NULL.
b. If ‘size * num’ is more than 108
, return NULL.
c. If sbrk fails in allocating the needed space, return NULL.
3. void sfree(void* p):
● Releases the usage of the block that starts with the pointer ‘p’.
● If ‘p’ is NULL or already released, simply returns.
● Presume that all pointers ‘p’ truly points to the beginning of an allocated block.
4. void* srealloc(void* oldp, size_t size):
● If ‘size’ is smaller than the current block’s size, reuse the same block. Otherwise,
finds/allocates ‘size’ bytes for a new space, copies content of old space into new space
and frees the old space.
● Return value:
i. Success –
a. Returns pointer to the first byte in the (newly) allocated space.
b. If ‘oldp’ is NULL, allocate space for ‘size’ bytes and return a pointer to it.
ii. Failure –
a. If size is 0 returns NULL.
b. If ‘size’ if more than 108
, return NULL.
c. If sbrk or brk fail in allocating the needed space, return NULL.
d. Do not free ‘oldp’ if srealloc() fails.
5. size_t _num_free_blocks():
● Returns the number of allocated blocks in the heap that are currently free.
6. size_t _num_free_bytes():
● Returns the number of bytes in all allocated blocks in the heap that are currently free,
excluding the bytes used by the meta-data structs. 
Page 8 of 14
7. size_t _num_allocated_blocks():
● Returns the overall (free and used) number of allocated blocks in the heap.
8. size_t _num_allocated_bytes():
● Returns the overall number (free and used) of allocated bytes in the heap, excluding the
bytes used by the meta-data structs.
9. size_t _num_meta_data_bytes();
● Returns the overall number of meta-data bytes currently in the heap.
10. size_t _size_meta_data():
● Returns the number of bytes of a single meta-data structure in your system.
