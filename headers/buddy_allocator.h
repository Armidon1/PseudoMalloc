#include "bit_map.h"

#define MAX_LEVELS 16
#define MAX_NUMBITS_BITMAP ((2^MAX_LEVELS)-1)


typedef struct  {
  BitMap bitmap; //bitmap which contains occupied index and free ones
  int num_levels;
  char* memory; // the memory area to be managed
  int min_bucket_size; // the minimum page of RAM that can be returned
} BuddyAllocator;

// returns the number of indexes to initialize the bitmap's buffer
int maxNumIndexesFromLevel(int num_levels);

// initializes the buddy allocator, and checks that the buffer is large enough
void BuddyAllocator_init(BuddyAllocator* alloc,int num_levels,char* buffer,int buffer_size,char* memory,int min_bucket_size);

// allocates memory
void* BuddyAllocator_malloc(BuddyAllocator* alloc, int size);

// releases allocated memory
int BuddyAllocator_free(BuddyAllocator* alloc, void* mem);
int BuddyAllocator_HardFree(BuddyAllocator* alloc, void* memReleased); //stronger version of free

//print the buddyallocator
void BuddyAllocator_print(BuddyAllocator* alloc);