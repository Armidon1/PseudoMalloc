#include <stdio.h>

#include "../headers/buddy_allocator.h" 

#define PAGE_SIZE 4096  //for example
#define BUDDY_LEVELS 5
#define MEMORY_SIZE (1024*1024) //1MB
#define MIN_BUCKET_SIZE (MEMORY_SIZE>>(BUDDY_LEVELS))
#define MAX_NUM_INDEX (1 << (BUDDY_LEVELS + 1)) - 1
#define BITMAP_BUFFER_SIZE MAX_NUM_INDEX%8!=0 ? (MAX_NUM_INDEX/8)+1 : (MAX_NUM_INDEX/8)

void myAllocator_init();
void* myMalloc(int size_requested);
int myFree(void* pointer, int size_requested);
int myHardFree(void* pointer, int size_requested);
