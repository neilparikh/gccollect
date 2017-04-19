#include <stdlib.h>

// Flags
// 0 - untouched (ie. should be collected)
// 1 - is in use
// 2 - user called free, should be collected

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
  struct pointerNode* currentPointer = pointerList;
  while (currentPointer != NULL) {
    if (*((short*) ((*(currentPointer->p)) - sizeof(short))) == 2) {
      (*(currentPointer->p)) = NULL;
    } else {
      *((short*) ((*(currentPointer->p)) - sizeof(short))) = 1;
    }
    currentPointer = currentPointer->next;
  }
 
  struct memoryNode* currentMemory = memoryList;
  struct memoryNode* prevMemory = NULL;
  while (currentMemory != NULL) {
    if (*((short*)(currentMemory->m)) == 0 || *((short*)(currentMemory->m)) == 2) {
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
      *((short*)(currentMemory->m)) = 0;
      currentMemory = currentMemory->next;
    }
  }
}

void GCfree(void* ptr) {
  free(ptr - sizeof(short));
  struct pointerNode* currentPointer = pointerList;
  while (currentPointer != NULL) {
    if (currentPointer->p == ptr) {
      currentPointer->p = NULL;
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
