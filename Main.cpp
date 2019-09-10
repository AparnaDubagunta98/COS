#include "Ackerman.h"
#include "BuddyAllocator.h"

int main(int argc, char ** argv) {

  int basic_block_size = 128, memory_length = 128*1024 * 1024;

  // create memory manager
  BuddyAllocator * allocator = new BuddyAllocator(basic_block_size, memory_length);
//   char* ub1 = allocator->alloc(128*1024);
//   char * buf2 = allocator->alloc(128*1024);
//   char * b3 = allocator->alloc(10*1024);
// // //  allocator->alloc(256*1024);
//   //allocator->free(ub1);
//   allocator->free(ub1);
//   allocator->free(b3);
//   allocator->debug();
  // test memory manager
  Ackerman* am = new Ackerman ();
  am->test(allocator); // this is the full-fledged test.

  // destroy memory manager
  delete allocator;
}
