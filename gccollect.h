// Register a pointer to be tracked
// p is the address of the pointer to be tracked
void registerPointer(void** p);

// allocates size bytes of memory and returns the address of that memory
// returns NULL if the allocation fails
void* GCmalloc(size_t size);

// collects memory not pointed to by any tracked pointer
// and frees memory that was freed using GCfreeDeferred
void GCcollect();

// frees the memory pointed to by ptr
// O(n), where n is number of pointers tracked
void GCfree(void* ptr);

// frees the memory pointed to by ptr
// O(1), but memory is not freed till next call of GCcollect
void GCfreeDeferred(void* ptr);

// frees all memory, as well the linked lists that track pointers and memory blocks
void GCfreeAll();
