# Collatz-Shared-Memory
A C-language implementation of the Collatz Conjecture

This program uses shared memory, where the parent process and child process share a region of memory.
In this program, I have set it to where the child is only allowed to write to the shared memory region, and the parent is only allowed to read from the shared memory region.

Due to this being my first C program (I have prior experience with C++), I decided to give up on inbuilt string functions like snprintf() and do the hair-ripping-out char work myself because coding everything from scratch was easier, at least until the very end (the last 8 hours of me coding this) where I somewhat figured out some of the inbuilt C string functions.

In the program, the starting # for the collatz() function is referred to as "seed".

Future Idea: Add a collatz() recursion depth function parameter to indicate how many #s are produced by a starting #.
