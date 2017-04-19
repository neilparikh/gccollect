# GCcollect

A mark and sweep garbage collector in/for C.

## Usage
On a high level:
- use GCmalloc and GCfree to allocate and free memory you want garbage collected
- register all pointers that point to memory allocated using GCmalloc
- call GCcollect to trigger a garbage collection. This will trigger a GC pause, as the collector uses mark and sweep.
- call GCFreeAll at the end of your program so the collector can do some cleanup

See gccollect.h for the specific API.
