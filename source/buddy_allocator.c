#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "../headers/buddy_allocator.h"

//DEBUG
#define DEBUG 0

//Aux functions
int fromIndextoLevel(size_t index){
  if (index == 0) return 0;
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
  return (index-(1<<fromIndextoLevel(index)));
}

int maxNumIndexesFromLevel(int num_levels){
  return (1 << (num_levels + 1)) - 1; // if there are n indexes (not includes the 0) then this function retrives a valure from 0 to n-1
}

// initializes the buddy allocator, and checks that the buffer is large enough
void BuddyAllocator_init(BuddyAllocator* alloc,int num_levels, char* buffer, int buffer_size, char* memory, int min_bucket_size){
  //checking that num_leveles is correct and buffer_size is enough
  if(num_levels<0){
    perror("num_levels<0. Failed to initialize BuddyAllocator\n");
    exit(EXIT_FAILURE);
  }
  #if DEBUG==1
    printf("DEBUG:INIT Im about to get required bytes for buffer\n");
  #endif
  int num_bits=maxNumIndexesFromLevel(num_levels); //(2*numlevels)-1 are the numbers of indexes possible
  int required_bytes_for_buffer = BitMap_getBytes(num_bits);
  #if DEBUG==1
    printf("DEBUG:INIT required bytes for buffer =%d and i have buffer_size = %d\n", required_bytes_for_buffer, buffer_size);
  #endif
  if (buffer_size < required_bytes_for_buffer){
    fprintf(stderr, "ERROR: buffer is too small to initialize BitMap: needed at least %d bytes, got %d\n", required_bytes_for_buffer, buffer_size);
    exit(EXIT_FAILURE);
  }
  #if DEBUG==1
    printf("DEBUG:INIT Passed all checks\n");
  #endif
  
  //finally i can initialize my BuddyAllocator
  alloc->memory=memory;
  alloc->min_bucket_size=min_bucket_size;
  alloc->num_levels=num_levels;
  BitMap bm;
  BitMap_init(&bm, num_bits, (uint8_t*)buffer);
  alloc->bitmap = bm;
  #if DEBUG==1
    printf("DEBUG:INIT finished INIT\n");
  #endif
}

// MALLOC ZONE
void* findMemoryPointer(void* memory, int indexBuddy, int target_level, int bucket_size) {
  //working
  // void* start_point = memory;
  // int currentIndex = indexBuddy+1;
  // int indexes_in_level_target_before = maxNumIndexesFromLevel(target_level);
  // int indexBuddy_in_target_level = currentIndex - indexes_in_level_target_before;
  // start_point += bucket_size*(indexBuddy_in_target_level - 1);
  // return start_point;
  
  int first_index = (1 << target_level) - 1;
  int indexBuddy_in_target_level = indexBuddy - first_index;
  return (uint8_t*)memory + bucket_size * indexBuddy_in_target_level;
}

int findBuddyIndex_dfs(BitMap* bm,int index, int current_level, int target_level) {
  if (current_level == target_level) {
      if (BitMap_bit(bm, index) == 0){ //if is free
          if (current_level == target_level && BitMap_bit(bm, index) == 0) {
              printf("DEBUG: allocando index=%d a livello=%d\n", index, target_level);
          }
          return index;
      }
        
      else
          return -1;
  }

  if (BitMap_bit(bm, index) != 0) //if is already occupied, exit
      return -1;

  int left = 2 * index + 1; //left child
  int right = 2 * index + 2;  //right child

  int res = findBuddyIndex_dfs(bm,left, current_level + 1, target_level);
  if (res != -1) return res;
  return findBuddyIndex_dfs(bm, right, current_level + 1, target_level);
}

void* getBuddy(BuddyAllocator* alloc, int target_level){
  BitMap* bm = &(alloc->bitmap);
  int buddyIndex= findBuddyIndex_dfs(bm, 0, 0, target_level);
  if (buddyIndex == -1) return NULL; //if everything in target_level is occupied, then exit 
  BitMap_setBit(bm, buddyIndex, 1);
  int bucket_size = ((alloc->min_bucket_size)<<(alloc->num_levels))>>target_level;
  return findMemoryPointer(alloc->memory, buddyIndex, target_level, bucket_size);
}
void* BuddyAllocator_malloc(BuddyAllocator* alloc, int size){
  if (!size){
    printf("MALLOC: WARNING: you are trying to allocate 0 bytes. You will recieve a NULL pointer!\n");
    return NULL;
  }
  int mem_size=(1<<alloc->num_levels)*alloc->min_bucket_size; // we determine the level of the page
  int level = fromIndextoLevel((size_t) mem_size/size);
  printf("DEBUG: MALLOC richiesta size=%d, livello=%d\n", size, level);

  // if the level is too small, we pad it to max
  if (level>alloc->num_levels) level=alloc->num_levels;
  printf("requested: %d bytes, level %d \n", size, level);

  //find the correct index
  void* pointer = getBuddy(alloc, level);
  if (!pointer){
    perror("MALLOC: Not enough memory!\n");
  }
  return pointer;
}

// releases allocated memory
void BuddyAllocator_free(BuddyAllocator* alloc, void* mem);

//print the buddyallocator
void BuddyAllocator_print(BuddyAllocator* alloc);