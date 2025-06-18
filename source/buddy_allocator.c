#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "../headers/buddy_allocator.h"

//DEBUG
#define DEBUG 0
#define SHOW_IN_TEST 1 //needed to show in test some result

//Aux functions
int fromIndextoLevel(size_t index){
  if (index == 0) return 0;
  int level = (int)floor(log2(index+1));
  return level;
};
  
int buddyIndex(int index){
  if (index == 0) return 0; //if im the root, i'm alone
  if (index&0x1){ //if it is't even then return the index before
    return index+1;
  }
  return index-1;
}
  
int parentIndex(int index){
  return (index-1)/2;
}

int firstIdx(int level){
  return (1 << level)-1; 
}
  
int startIndex(int index){
  //return (index-(1<<fromIndextoLevel(index)));
  return (index-firstIdx(fromIndextoLevel(index)));
}

int maxNumIndexesFromLevel(int num_levels){
  return (1 << (num_levels + 1)) - 1; // if there are n indexes (not includes the 0) then this function retrives a valure from 0 to n-1
}

int fromSizeToLevel(int size, BuddyAllocator* alloc){
  int level = alloc->num_levels;
  int mem_size=(1<<alloc->num_levels)*alloc->min_bucket_size;
  assert((mem_size & (mem_size - 1)) == 0); //memsize has to be a power of 2
  while (level>=0){
    int bucket_size=(mem_size>>level);
    if (size<=bucket_size) break;
    level-=1;
  }
  return level;
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
  int num_bits=maxNumIndexesFromLevel(num_levels); //(2*(numlevels+1))-1 are the numbers of indexes possible
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
  int first_index = (1 << target_level) - 1; 
  int indexBuddy_in_target_level = indexBuddy - first_index; //normalize the relative index to the level
  void* pointer =(uint8_t*)memory + (bucket_size * indexBuddy_in_target_level);
  #if DEBUG ==1
  printf("DEBUG:MALLOC: indexBuddy=%d, target_level=%d,bucket_size=%d, pointer=%p\n",indexBuddy, target_level, bucket_size, pointer);
  #endif
  return pointer;
}

//i scan all the bitmap everytime. I don't like it but i guess is the only way :(. Everytime i find a node which isn't free, then continue searching 
void setOccupiedAllAncestors(BitMap* bm, int index){
  if (index < 0) return; //to be safe
  #if DEBUG==1
  printf("DEBUG: MALLOC_setOccupiedAllAncestors: index=%d\n",index);
  #endif
  BitMap_setBit(bm, index, 1);
  if (index == 0) return; //if im in root
  setOccupiedAllAncestors(bm, parentIndex(index));
}
void setOccupiedAllDescendants(BitMap* bm, int index, int index_level, int max_level){
  #if DEBUG==1
    printf("DEBUG: MALLOC_setOccupiedAllDescendants: index=%d, index_level:%d, max_level=%d\n",index, index_level, max_level);
  #endif
  int max_index = bm->num_bits - 1; //to 
  if (index > max_index) return;    //be
  if(index_level>max_level) return; //safe
  BitMap_setBit(bm, index, 1);
  if(index_level!=max_level){
    int left_child = (2*index)+1;
    int right_child = (2 * index)+2;
    setOccupiedAllDescendants(bm, left_child, index_level+1,max_level);
    setOccupiedAllDescendants(bm, right_child, index_level+1, max_level);
  }
}
void Malloc_doAllTaskOnBitmap(BitMap* bm, int index, int current_level, int max_level){
  BitMap_setBit(bm, index, 1);
  #if DEBUG == 1
  printf("DEBUG: MALLOC_doAllStuffOnBitmap: index=%d, current_level:%d\n",index, current_level);
  BitMap_print(bm);
  #endif
  setOccupiedAllAncestors(bm, parentIndex(index));
  int left_child = (2*index)+1;
  int right_child = (2 * index)+2;
  setOccupiedAllDescendants(bm, left_child, current_level+1, max_level);  
  setOccupiedAllDescendants(bm, right_child, current_level+1, max_level);
}

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
  Malloc_doAllTaskOnBitmap(bm, index, fromIndextoLevel(index), alloc->num_levels);
  int bucket_size = ((alloc->min_bucket_size)<<(alloc->num_levels))>>target_level;
  #if DEBUG==1
  BitMap_print(&alloc->bitmap);
  #endif
  int* pointer = (int*)findMemoryPointer(alloc->memory, index, target_level, bucket_size);
  *pointer = index; //i store in pointer[0] his bitamp's index 
  //printf("DEBUG: MALLOC_GetBuddy: pointer[0]=%d should be equals to index=%d, also pointer[1]=%d should be a strange number\n", pointer[0], index, pointer[1]);
  return (void*)pointer;
}

void* BuddyAllocator_malloc(BuddyAllocator* alloc, int sizeRequested){
  if (!sizeRequested){
    printf("MALLOC: WARNING: you are trying to allocate 0 bytes. You will recieve a NULL pointer!\n");
    return NULL;
  }
  int size = sizeRequested+sizeof(int); //4 bytes are needed to store the bitmap's index
  
  int mem_size=(1<<alloc->num_levels)*alloc->min_bucket_size; // we determine the level of the page
  if (size>mem_size){
    printf("MALLOC: WARNING: too big allocation: requested:%d and the allocator have %d. You will recieve a NULL pointer!\n",size, mem_size);
    return NULL;
  }
  int level = fromSizeToLevel(size, alloc);

  // if the level is too small, we pad it to max
  if (level>alloc->num_levels) level=alloc->num_levels; //useless check, implemented fromSizeToLevel which already pad it to max
  
  #if DEBUG==1 || SHOW_IN_TEST == 1
    printf("DEBUG: BuddyAllocator_malloc: requested: %d bytes, level %d \n", size, level);
  #endif
  //find the correct index
  int* pointer = (int*)getBuddy(alloc, level);
  if (!pointer){
    printf("MALLOC: Not enough memory! you will recieve a NULL pointer\n");
    return NULL;
  }
  return (void*)(pointer+sizeof(int)); //i give him sizeRequested bytes, as he asked
}

// releases allocated memory 
void setFreeAllAncestors(BitMap* bm, int index){
  if (index < 0) return; //to be safe
  #if DEBUG==1
  printf("DEBUG: FREE_setFreeAllAncestors: index=%d\n",index);
  #endif
  BitMap_setBit(bm, index, 0);
  if (!index) return; //if im root, i dont have a brother
  if (!BitMap_bit(bm,buddyIndex(index))){ //i check if my brother is free
    setFreeAllAncestors(bm, parentIndex(index));
  }
}
void setFreeAllDescendants(BitMap* bm, int index, int index_level, int max_level){
  #if DEBUG==1
    printf("DEBUG: FREE_setFreeAllDescendants: index=%d, index_level:%d, max_level=%d\n",index, index_level, max_level);
  #endif
  int max_index = bm->num_bits - 1; //to 
  if (index > max_index) return;    //be
  if(index_level>max_level) return; //safe
  BitMap_setBit(bm, index, 0);
  if(index_level!=max_level){
    int left_child = (2*index)+1;
    int right_child = (2*index)+2;
    setFreeAllDescendants(bm, left_child, index_level+1, max_level);  
    setFreeAllDescendants(bm, right_child, index_level+1, max_level);
  }
}
int Free_doAllTaskOnBitmap(BitMap* bm, int index, int max_level){
  if (!BitMap_bit(bm, index)){
    printf("ERROR: FREE: Bit index=%d already free. Double free error! Nothing will happen\n", index);
    return -1;
  }
  BitMap_setBit(bm, index, 0); 
  #if DEBUG == 1
  printf("DEBUG: FREE_doAllTaskOnBitmap: index=%d, current_level:%d\n",index, current_level);
  BitMap_print(bm);
  #endif
  if (!BitMap_bit(bm,buddyIndex(index))){ //i check if my brother is free
    setFreeAllAncestors(bm, parentIndex(index));
  }
  int index_level = fromIndextoLevel(index);
  int left_child = (2*index)+1;
  int right_child = (2*index)+2;
  setFreeAllDescendants(bm, left_child, index_level+1, max_level);  
  setFreeAllDescendants(bm, right_child, index_level+1, max_level);
  return 0;
}

int checkIfMemoryIsInsideAlloc(BuddyAllocator* alloc, void* mem){
  int sizeOfMemory = (alloc->min_bucket_size)<<(alloc->num_levels); //in bytes
  int* startAllocMemory =(int*)(alloc->memory);
  int* endAllocMemory = startAllocMemory+(sizeOfMemory/sizeof(int)); 
  if ((int*)mem < startAllocMemory || (int*)mem >= endAllocMemory) return 0; //is out of the buffer
  return 1; //is inside the buffer
}

int BuddyAllocator_free(BuddyAllocator* alloc, void* memReleased){
  if (memReleased==NULL) {
    printf("ERROR: FREE: You are trying to free a NULL pointer! Nothing will happen\n");
    return -1;
  }
  //i have to find which index is 
  int* mem = (int*)memReleased -sizeof(int); //real memory
  if (!checkIfMemoryIsInsideAlloc(alloc, (void*)mem)){
    printf("ERROR: FREE: You are trying to free a pointer which isn't inside the allocator's buffer! Nothing will happen\n");
    return -1; //which will be used to the user to check directly if free function worked
  }
  int index =mem[0];
  #if DEBUG==1 || SHOW_IN_TEST == 1
    printf("DEBUG: BuddyAllocator_free: trying to free index=%d\n", index);
  #endif
  if (Free_doAllTaskOnBitmap(&alloc->bitmap, index, alloc->num_levels)==-1) return -1; //double free error 
  return 0; //everything went good
}

//for security reasons, frees the memory and also set all bits to 0
int BuddyAllocator_HardFree(BuddyAllocator* alloc, void* memReleased){
  int* mem = (int*)memReleased -sizeof(int);
  int index = mem[0];
  //printf("INDEX=%d\n", index);
  int level = fromIndextoLevel(index);
  int bucket_size = ((alloc->min_bucket_size)<<(alloc->num_levels))>>level;
  memset(memReleased, 0, bucket_size-sizeof(int)); 
  if (!BuddyAllocator_free(alloc, memReleased)) return 0;
  return 1; //everything went good
}

//print the buddyallocator
void BuddyAllocator_print(BuddyAllocator* alloc){
  printf("\n--- STATE OF BUDDY ALLOCATOR ---\n");
  printf("Number of Levels (contains level 0): %d\n", alloc->num_levels);
  printf("Minimum bucket size: %d bytes\n", alloc->min_bucket_size);
  //as usual, calc the size of total memory
  int memory_size = alloc->min_bucket_size * (1 << alloc->num_levels);
  printf("Total memory size: %d bytes\n", memory_size);
  BitMap_print(&(alloc->bitmap));
  printf("----------------------------------\n");
}