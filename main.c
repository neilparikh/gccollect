#include <stdio.h>
#include "gccollect.h"

int main() {
  void* test = GCmalloc(100);
  registerPointer(&test);
  test = GCmalloc(400);
  GCcollect(); // should collect initial 100 bytes due to first call of GCmalloc
  GCfree(test);
  test = GCmalloc(200);
  GCfreeDeferred(test);
  GCcollect(); // should collect due to deferred free
  GCfreeAll();
}
