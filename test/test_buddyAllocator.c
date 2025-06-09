#include <stdio.h>
#include <stdlib.h>

#include "../headers/buddy_allocator.h"

//DEBUG
#define SHOW_DEBUG_VALUES 0 //must be 0 or 1
#define DEBUG_CHECK_INIT_BUFFER_BITMAP 0  //must be 0 or 1
#define DEBUG_CHECK_INIT_NEGATIVE_LEVELS 0 //must be o or 1


#define BUDDY_LEVELS 9
#define MEMORY_SIZE (1024*1024) //1MB
#define MIN_BUCKET_SIZE (MEMORY_SIZE>>(BUDDY_LEVELS))

int testing_init(){
    printf("|STARTING TESTING INIT|\n");
    BuddyAllocator allocator;
    #if DEBUG_CHECK_INIT_BUFFER_BITMAP == 0
        int buffer_size = BitMap_getBytes(maxNumIndexesFromLevel(BUDDY_LEVELS)); //should pass the check
        printf("num levels = %d, maxIndexes= %d\n", BUDDY_LEVELS, buffer_size);
    #elif DEBUG_CHECK_INIT_BUFFER_BITMAP == 1
        int required_bytes_for_buffer = BitMap_getBytes(maxNumIndexesFromLevel(BUDDY_LEVELS));
        int buffer_size = required_bytes_for_buffer -1; //should not pass the check
        printf("num levels = %d, required_bytes_for_buffer= %d, buffer_size= %d\n", BUDDY_LEVELS, required_bytes_for_buffer, buffer_size);
    #else
        printf("DEBUG must be 0,1,2\n");
        return(1);
    #endif

    char buffer_bitmap[buffer_size];
    char memory[MEMORY_SIZE];

    #if DEBUG_CHECK_INIT_NEGATIVE_LEVELS==1
        BuddyAllocator_init(&allocator, -1, buffer_bitmap, buffer_size, memory, MIN_BUCKET_SIZE);
        printf("this message should not be showed. something went wrong while checking num_levels < 0 in BuddyAllocator_init\n");
        return(1);
    #else
        BuddyAllocator_init(&allocator, BUDDY_LEVELS, buffer_bitmap, buffer_size, memory, MIN_BUCKET_SIZE);
        printf("buddy initializated correctly, (in theory, lol)\n");
    #endif
    printf("|FINISHING TESTING INIT|\n");
    return 0;
}

//NEEDED
int testing_malloc(){
    return 0;
}
//NEEDED
int testing_free(){
    return 0;
}
//NEEDED
int testing_print(){
    return 0;
}

int main(){
    #if SHOW_DEBUG_VALUES == 1
        printf("------------------------\n");
        printf("||DEBUG VALUES||\n");
        printf("SHOW_DEBUG_VALUES = %d\n", SHOW_DEBUG_VALUES);
        printf("DEBUG_CHECK_INIT_BUFFER_BITMAP = %d\n", DEBUG_CHECK_INIT_BUFFER_BITMAP);
        printf("DEBUG_CHECK_INIT_NEGATIVE_LEVELS = %d\n", DEBUG_CHECK_INIT_NEGATIVE_LEVELS);
        printf("||END DEBUG VALUES||");
    #endif

    printf("------------------------\n");
    if(testing_init()) exit(EXIT_FAILURE);
    printf("------------------------\n");
    if(testing_malloc()) exit(EXIT_FAILURE);
    printf("------------------------\n");
    if(testing_free()) exit(EXIT_FAILURE);
    printf("------------------------\n");
    if(testing_print()) exit(EXIT_FAILURE);
    printf("------------------------\n");
    printf("ALL TEST ARE COMPLETED!! NICE JOB SIMONE!\n");
    printf("sizeof(bitmap)=%ld, sizeof(buddyallocator)=%ld\n", sizeof(BitMap), sizeof(BuddyAllocator));
    return 0;
}