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
