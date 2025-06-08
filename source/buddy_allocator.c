#include <assert.h>
#include <stdio.h>

#include "buddy_allocator.h"

//Aux functions
int fromIndextoLevel(size_t index){
  int level = (int)floor(log2(index));
  return level;
};
  
int buddyIndex(int index){
  if (index&0x1){ //if it's even then return the index before
    return index-1;
  }
  return index+1;
}
  
int parentIndex(int index){
  return index/2;
}
  
int startIndex(int index){
  return (index-(1<<levelIdx(index)));
}

// initializes the buddy allocator, and checks that the buffer is large enough
void BuddyAllocator_init(BuddyAllocator* alloc,int num_levels, char* buffer, int buffer_size, char* memory, int min_bucket_size);

// allocates memory
void* BuddyAllocator_malloc(BuddyAllocator* alloc, int size);

// releases allocated memory
void BuddyAllocator_free(BuddyAllocator* alloc, void* mem);

//print the buddyallocator
void BuddyAllocator_print(BuddyAllocator* alloc);