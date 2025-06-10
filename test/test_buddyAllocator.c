#include <stdio.h>
#include <stdlib.h>

#include "../headers/buddy_allocator.h"

//DEBUG INIT
#define SHOW_DEBUG_VALUES 0                 //must be 0 or 1
#define DEBUG_CHECK_INIT_BUFFER_BITMAP 0    //must be 0 or 1
#define DEBUG_CHECK_INIT_NEGATIVE_LEVELS 0  //must be o or 1

//DEBUG MALLOC
#define DEBUG_CHECK_MALLOC 2                //must be 0,1 or 2
#define DEBUG_CHECK_MALLOC_ALL 0            //must be 0 or 1


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
    printf("|STARTING TESTING MALLOC|\n");
    BuddyAllocator allocator;
    int buffer_size = BitMap_getBytes(maxNumIndexesFromLevel(BUDDY_LEVELS));
    char buffer_bitmap[buffer_size];
    char memory[MEMORY_SIZE];
    BuddyAllocator_init(&allocator, BUDDY_LEVELS, buffer_bitmap, buffer_size, memory, MIN_BUCKET_SIZE);

    #if DEBUG_CHECK_MALLOC == 0 || DEBUG_CHECK_MALLOC_ALL==1
        // 1. Testing 0 byte allocation
        void* p0 = BuddyAllocator_malloc(&allocator, 0);
        if (p0) printf("ERROR: Allocation of 0 bytes should not work! Something must be fixed\n");
        else printf("OK: Allocation of 0 bytes correctly handled.\n");
    #endif

    #if DEBUG_CHECK_MALLOC == 1 || DEBUG_CHECK_MALLOC_ALL==1
        // 2. Testing too big allocations
        void* p1 = BuddyAllocator_malloc(&allocator, MEMORY_SIZE + 1);
        if (p1) printf("ERROR: too big allocations should not work! Something must be fixed\n");
        else printf("OK: too big allocations correctly handled.\n");
    #endif

    #if DEBUG_CHECK_MALLOC == 2 || DEBUG_CHECK_MALLOC_ALL==1
        // 3. Testing Allocations that fills all memory
        printf("memory=%p\n", memory);
        int total_blocks = 1 << BUDDY_LEVELS;
        void* blocks[total_blocks];
        int i;
        for (i = 0; i < total_blocks; ++i) {
            printf("TEST: i=%d\n", i);
            void* p = BuddyAllocator_malloc(&allocator, MIN_BUCKET_SIZE);
            printf("TEST: pointer=%p\n", p);
            blocks[i] = p;
            if (!blocks[i]) break;
        }
        if (i == total_blocks) printf("OK: memory filled correctly.\n");
        else printf("ERROR: i failed filling all the memory.\n");

        void* p2 = BuddyAllocator_malloc(&allocator, MIN_BUCKET_SIZE);
        if (p2) printf("ERROR: allocation beyond memory should fail!\n");
        else printf("OK: allocation beyond properly managed memory.\n");
    #endif

    printf("|FINISHING TESTING MALLOC|\n");
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
    return 0;
}