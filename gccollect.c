#include <stdlib.h>
#include <assert.h>

// Flags
// 0 - untouched (ie. should be collected)
// 1 - is in use
// 2 - user called GCfreeDeferred, should be collected

struct pointerNode {
  void** p;
  struct pointerNode* next;
};

struct memoryNode {
  void* m;
  struct memoryNode* next;
};

struct pointerNode* pointerList;
struct memoryNode* memoryList;

void registerPointer(void** p) {
  struct pointerNode* new_node = malloc(sizeof(struct pointerNode));
  new_node->next = pointerList;
  new_node->p = p;
  pointerList = new_node;
}

void registerMemory(void* m) {
  struct memoryNode* new_node = malloc(sizeof(struct memoryNode));
  new_node->next = memoryList;
  new_node->m = m;
  memoryList = new_node;
}

void* GCmalloc(size_t size) {
  void* newMemory = malloc(sizeof(short) + size);
  if (newMemory == NULL) {
    return NULL;
  }
  *(short*) newMemory = 0;
  registerMemory(newMemory);
  return (newMemory + sizeof(short));
}

void GCcollect() {
  // Go through the pointer list, and update the flag on the pointed to memory
  struct pointerNode* currentPointer = pointerList;
  while (currentPointer != NULL) {
    const void* pointer = *(currentPointer->p);
    if (!pointer) {
      // pointer doesn't point to anything, skip
      currentPointer = currentPointer->next;
      continue;
    }
    const void* memory_loc = pointer - sizeof(short);
    const short flag = *((short *) memory_loc);
    if (flag == 2) {
      // User called GCfreeDeferred on this memory, set pointer to NULL
      (*(currentPointer->p)) = NULL;
    } else {
      // Mark this memory as used
      *((short*) memory_loc) = 1;
    }
    currentPointer = currentPointer->next;
  }
 
  // go through the memory list and free all memory that has a 0 or 2 flag
  struct memoryNode* currentMemory = memoryList;
  struct memoryNode* prevMemory = NULL;
  while (currentMemory != NULL) {
    short* flag = ((short*)(currentMemory->m));
    if (*flag == 0 || *flag == 2) {
      // drop this node from list
      free(currentMemory->m);
      if (prevMemory == NULL) {
        memoryList = currentMemory->next;
        free(currentMemory);
        currentMemory = memoryList;
      } else {
        prevMemory->next = currentMemory->next;
        free(currentMemory);
        currentMemory = prevMemory->next;
      }
    } else {
      prevMemory = currentMemory;
      *flag = 0;
      currentMemory = currentMemory->next;
    }
  }
}

void GCfree(void* ptr) {
  /* realloc is used here instead of free, because there's a pointer to this
   * memory in the memoryList. On the next run of GCcollect, free will be
   * called on this location, since no pointers point to it anymore.
   * If free was used here, a "pointer being freed was not allocated" would be
   * raised, since it's already freed.
   *
   * To get around that, we call realloc with 1 instead, so the that free can
   * be called on it again. However, realloc is not guaranteed to return the
   * same address if the new size is smaller. If it returns a different
   * address, we'll just the "pointer being freed was not allocated" error
   * again.
   *
   * For now, we just assert that the new pointer is the same as the old one,
   * until I come up with a better solution :)
   */
  void *ptrToFree = ptr - sizeof(short);
  void *newPtr = realloc(ptrToFree, 1);
  assert(newPtr == ptrToFree);

  // Set all pointers that point to this location to NULL
  struct pointerNode* currentPointer = pointerList;
  while (currentPointer != NULL) {
    if (*(currentPointer->p) == ptr) {
      *(currentPointer->p) = NULL;
    }
    currentPointer = currentPointer->next;
  }
}

void GCfreeDeferred(void* ptr) {
  *((short*) (ptr - sizeof(short))) = 2;
}

void GCfreeAll() {
  // Free the linked list for pointers
  struct pointerNode* currentPointer = pointerList;
  struct pointerNode* nextPointer = NULL;
  while (currentPointer != NULL) {
    nextPointer = currentPointer->next;
    free(currentPointer);
    currentPointer = nextPointer;
  }

  // Free the linked list for memory, as well as all allocated memory
  struct memoryNode* currentMemory = memoryList;
  struct memoryNode* nextMemory = NULL;
  while (currentMemory != NULL) {
    nextMemory = currentMemory->next;
    free(currentMemory->m);
    free(currentMemory);
    currentMemory = nextMemory;
  }
}
