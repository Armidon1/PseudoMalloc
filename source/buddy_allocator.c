#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "../headers/buddy_allocator.h"

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

int maxNumIndexesFromLevel(int num_levels){
  return (1<<num_levels)-1;
}

// initializes the buddy allocator, and checks that the buffer is large enough
void BuddyAllocator_init(BuddyAllocator* alloc,int num_levels, char* buffer, int buffer_size, char* memory, int min_bucket_size){
  //checking that num_leveles is correct and buffer_size is enough
  if(num_levels<0)perror("num_levels<0. Failed to initialize BuddyAllocator\n");
  int num_bits=maxNumIndexesFromLevel(num_levels); //(2*numlevels)-1 are the numbers of indexes possible
  int required_bytes_for_buffer = BitMap_getBytes(num_bits);
  if (buffer_size < required_bytes_for_buffer){
    fprintf(stderr, "buffer is too small to initialize BitMap: needed at least %d bytes, got %d\n", required_bytes_for_buffer, buffer_size);
    exit(EXIT_FAILURE);
  }
  //finally i can initialize my BuddyAllocator
  alloc->memory=memory;
  alloc->min_bucket_size=min_bucket_size;
  alloc->num_levels=num_levels;
  BitMap bm;
  BitMap_init(&bm, num_bits, buffer);
  alloc->bitmap = bm;
}

// allocates memory
void* BuddyAllocator_malloc(BuddyAllocator* alloc, int size);

// releases allocated memory
void BuddyAllocator_free(BuddyAllocator* alloc, void* mem);

//print the buddyallocator
void BuddyAllocator_print(BuddyAllocator* alloc);