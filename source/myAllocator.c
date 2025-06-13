#include <stdio.h>

#include "../headers/myAllocator.h" 

BitMap bitmap;
char buffer_bitmap[BITMAP_BUFFER_SIZE];
BuddyAllocator buddy_allocator;
char memory[MEMORY_SIZE];


void myAllocator_init(){ //buddyAllocator_init already does everything 
    BuddyAllocator_init(&buddy_allocator, BUDDY_LEVELS, buffer_bitmap, BITMAP_BUFFER_SIZE, memory, MIN_BUCKET_SIZE);
}
void* malloc(int size_requested){
    
}
int free(void* pointer);