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

int fromSizeToLevel(int size, BuddyAllocator* alloc){
  int level = MAX_LEVELS;
  int mem_size=(1<<alloc->num_levels)*alloc->min_bucket_size;
  assert((mem_size & (mem_size - 1)) == 0); //memsize has to be a power of 2
  while (level>0){
    int bucket_size=(mem_size>>level);
    if (size<=bucket_size) break;
    level-=1;
  }
  return level;
}

//AUX stuff for side effect on bitmap, all implemented recursivelly
//for Malloc
void setOccupiedAllAncestors(BitMap* bm, int index){
  if (!index){ //i'm in the root
    BitMap_setBit(bm, index, 1);
    return;
  } 
  setOccupiedAllAncestors(bm, parentIndex(index));
}
void setOccupiedAllDescendants(BitMap* bm, int index, int indexLevel){
  BitMap_setBit(bm, index, 1);
  if(indexLevel<MAX_LEVELS){
    int left_child = (2*index)+1;
    int right_child = (2 * index)+2;
    setOccupiedAllDescendants(bm, left_child, indexLevel+1);
    setOccupiedAllDescendants(bm, right_child, indexLevel+1);
  }
}
void Malloc_doAllTaskOnBitmap(BitMap* bm, int index, int current_level){
  BitMap_setBit(bm, index, 1);
  setOccupiedAllAncestors(bm, parentIndex(index));
  int left_child = (2*index)+1;
  int right_child = (2 * index)+2;
  setOccupiedAllDescendants(bm, left_child, current_level+1);  
  setOccupiedAllDescendants(bm, right_child, current_level+1);
}

//for Free (WORKING IN PROGRESS)
void setFreeAllAncestors(BitMap* bm, int index);
void setFreeAllDescendants(BitMap* bm, int index, int indexLevel, int max_level);
void Free_doAllTaskOnBitmap(BitMap* bm, int index);

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

//i scan all the bitmap everytime. I don't like it but i guess is the only way :(. Everytime i find a node which isn't free, then continue searching 
int findBuddyIndex_dfs(BitMap* bm,int index, int current_level, int target_level) { 
  if (current_level == target_level){
    if (BitMap_bit(bm, index)) return -1; //seems like that this node is occupied
    return index; //so is free, i'm taking that node
  }
  //if i'm here, it means that i'm not in the target level
  int left_child = (2*index)+1;
  int right_child = (2 * index)+2;
  int ret= findBuddyIndex_dfs(bm, left_child, current_level+1, target_level); 
  if (ret!=-1) return ret; //seems like i found something
  ret= findBuddyIndex_dfs(bm, right_child, current_level+1, target_level);
  return ret;
}

void* getBuddy(BuddyAllocator* alloc, int target_level){
  BitMap* bm = &(alloc->bitmap);
  int index= findBuddyIndex_dfs(bm, 0, 0, target_level);
  if (index == -1) return NULL; //if everything in target_level is occupied, then exit 
  Malloc_doAllTaskOnBitmap(bm, index, fromIndextoLevel(index));
  int bucket_size = ((alloc->min_bucket_size)<<(alloc->num_levels))>>target_level;
  return findMemoryPointer(alloc->memory, index, target_level, bucket_size);
}
void* BuddyAllocator_malloc(BuddyAllocator* alloc, int size){
  if (!size){
    printf("MALLOC: WARNING: you are trying to allocate 0 bytes. You will recieve a NULL pointer!\n");
    return NULL;
  }
  int mem_size=(1<<alloc->num_levels)*alloc->min_bucket_size; // we determine the level of the page
  if (size>mem_size){
    printf("MALLOC: WARNING: too big allocation: requested:%d and the allocator have %d. You will recieve a NULL pointer!\n",size, mem_size);
    return NULL;
  }
  int level = fromSizeToLevel(size, alloc);
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