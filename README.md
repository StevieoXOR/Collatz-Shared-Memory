# Collatz-Shared-Memory
A C-language implementation of the Collatz Conjecture

This program uses shared memory, where the parent process and child process share a region of memory.
In this program, I have set it to where the child is only allowed to write to the shared memory region, and the parent is only allowed to read from the shared memory region.

Due to this being my first C program (I have prior experience with C++), I decided to give up on inbuilt string functions like snprintf() and do the hair-ripping-out char work myself because coding everything from scratch is easier (imo). In the very end (the last 8 hours of me coding this), I somewhat figured out some of the inbuilt C string functions so I added a global bool option to use either my raw implementation or the inbuilt string functions.

In the program, the starting # for the collatz() function is referred to as "seed".

Limitations:
* This program uses ints (32 bits per int with max#thatCanBeStored == (2^31)-1, (2^31 because of negative values halving the range)), so if ANY of the individual results produced by the collatz sequence are negative, 0, or are larger than the largest positive # (which overflow to negative #s due to the sign bit) a signed int can store, then the program complains and quits. You can naively change them to long (or maybe even long long?) but don't be surprised if some functionality breaks (truncation at best, possible segmentation fault), given that I allocated a max of around 11 chars (10 digits + 1 comma) per collatz result.
* The shared memory has been allocated 4,096 bytes. If you do change the ints to longs or some longer data type, and don't run into overflow issues, then you still may have truncation (though truncation due to limited shared memory is unlikely imo).
* Be aware of uncommenting my if(seed<1) block that is in main(). It gives a SEGMENTATION FAULT if you uncomment it, but I'm not entirely certain why; I suspect the shared memory block and not modification of const char pointers.

Future Idea: Add a collatz() recursion depth function parameter to indicate how many #s are produced by a starting #. Make the global bool an executable argument that can go in the makefile.
