#include <stdio.h>
#include <string.h>

#include "../headers/buddy_allocator.h"

#define BUDDY_LEVELS 5
#define MEMORY_SIZE (1024*1024) //1MB
#define MIN_BUCKET_SIZE (MEMORY_SIZE>>(BUDDY_LEVELS))

int main(){
    printf("|STARTING TESTING|\n");
    BuddyAllocator allocator;
    int buffer_size = BitMap_getBytes(maxNumIndexesFromLevel(BUDDY_LEVELS));
    char buffer_bitmap[buffer_size];
    char memory[MEMORY_SIZE];
    int min_bucket_size_requested = MIN_BUCKET_SIZE-sizeof(int);

    printf("\n\033[1;36mPress ENTER TO initialize the BuddyAllocator...\033[0m");
    getchar();
    BuddyAllocator_init(&allocator, BUDDY_LEVELS, buffer_bitmap, buffer_size, memory, MIN_BUCKET_SIZE);
    printf("|\33[1;32mBuddy Initializated\033[0m|\n");
    BuddyAllocator_print(&allocator);

    // Alloc min size block
    printf("\n\033[1;36mPress ENTER alloc a block of min_bucket_size_requested=%d...\033[0m", min_bucket_size_requested);
    getchar();
    void* p1 = BuddyAllocator_malloc(&allocator, min_bucket_size_requested);
    if (!p1) {
        printf("\33[1;31mERROR\033[0m: Allocation failed!\n");
        return 1;
    }
    BitMap_print(&(allocator.bitmap));
    printf("\33[1;32mOK\033[0m: Allocation succeded, ptr=%p\n", p1);


    // free the block
    printf("\n\033[1;36mPress ENTER to free that block of min_bucket_size_requested=%d...\033[0m", min_bucket_size_requested);
    getchar();
    if (BuddyAllocator_free(&allocator, p1)==-1){
        printf("\33[1;31mERROR\033[0m: Free failed!\n");
        return 1;
    }
    BitMap_print(&(allocator.bitmap));
    printf("\33[1;32mOK\033[0m: Min block free.\n");
    
    // testing allocation the same size as before: should return the same pointer 
    printf("\n\033[1;36mPress ENTER to realloc that block of the same size (min_bucket_size_requested=%d...\033[0m", min_bucket_size_requested);
    getchar();
    void* p2 = BuddyAllocator_malloc(&allocator, min_bucket_size_requested);
    BitMap_print(&(allocator.bitmap));
    if (p2 == p1)
        printf("\33[1;32mOK\033[0m: Reallocation after the free returned the exact same pointer as before.\n");
    else if (p2)
        printf("\33[1;32mOK\033[0m: Reallocation after the free did't return the exact same pointer (it is good anyways).\n");
    else
        printf("\33[1;31mERROR\033[0m: Reallocation after free failed!\n");
    BuddyAllocator_free(&allocator, p2);
    BitMap_print(&(allocator.bitmap));

    // Testing the allocation and free of multiple blocks of the same size
    printf("\n\033[1;36mPress ENTER to alloc multiple blocks of the same size (min_bucket_size_requested=%d)...\033[0m", min_bucket_size_requested);
    getchar();
    int total_blocks = 4;
    void* blocks[total_blocks];
    for (int i = 0; i < total_blocks; ++i) {
        blocks[i] = BuddyAllocator_malloc(&allocator, min_bucket_size_requested);
        if (!blocks[i]) {
            printf("\33[1;31mERROR\033[0m: Allocazione multipla fallita al blocco %d!\n", i);
            return 1;
        }
        BitMap_print(&(allocator.bitmap));
    }
    printf("\33[1;32m OK\033[0m: allocation of multiple blocks succeded\n");

    printf("\n\033[1;36mPress ENTER to free multiple blocks of the same size (min_bucket_size_requested=%d)...\033[0m", min_bucket_size_requested);
    getchar();
    for (int i = 0; i < total_blocks; ++i) {
        if (BuddyAllocator_free(&allocator, blocks[i])==-1) {
            printf("\33[1;31mERROR\033[0m: Something went wrong with the free\n");
            return 1;
        };
        BitMap_print(&(allocator.bitmap));
    }
    printf("\33[1;32mOK\033[0m: Multiple free succeded.\n");

    // Testing Allocations that fills all memory
    printf("\n\033[1;36mPress ENTER to fill all the memory with all possibile allocatios of min_bucket_size (min_bucket_size_requested=%d)...\033[0m", min_bucket_size_requested);
    getchar();
    total_blocks = 1 << BUDDY_LEVELS;
    void* blocks2[total_blocks];
    int i;
    for (i = 0; i < total_blocks; ++i) {
        printf("TEST: i=%d\n", i);
        void* p = BuddyAllocator_malloc(&allocator, min_bucket_size_requested);
        printf("TEST: pointer=%p\n", p);
        blocks2[i] = p;
        if (!blocks2[i]) break;
        BitMap_print(&allocator.bitmap);
        printf("\n\033[1;36mPress ENTER to continue...\033[0m");
        getchar();
    }
    if (i == total_blocks) printf("\33[1;32mOK\033: memory filled correctly.\n");
    else printf("\33[1;31mERROR\033[0m: i failed filling all the memory.\n");
    p2 = BuddyAllocator_malloc(&allocator, min_bucket_size_requested);
    //printf("p2=%p\n",p2);
    if (p2) printf("\33[1;31mERROR\033[0m: allocation beyond memory should fail!\n");
    else printf("\33[1;32mOK\033[0m: allocation beyond properly managed memory.\n");
    
    //Testing free all the memory
    printf("\n\033[1;36mPress ENTER to free all the memory again...\033[0m");
    getchar();
    for (i = 0; i < total_blocks; ++i) {
        printf("TEST: i=%d\n", i);
        if (BuddyAllocator_free(&allocator, blocks2[i])==-1) {
            printf("\33[1;31mERROR\033[0m: Something went wrong with the free\n");
            return 1;
        };
        BitMap_print(&(allocator.bitmap));
        printf("\n\033[1;36mPress ENTER to continue...\033[0m");
        getchar();
    }
    if (i == total_blocks) printf("\33[1;32mOK\033[0m: memory completelly freed correctly.\n");
    else printf("\33[1;31mERROR\033[0m: i failed freeing all the memory.\n");

    // TEST DOUBLE FREE
    printf("\n\033[1;36mPress ENTER to test the double free error...\033[0m");
    getchar();
    printf("TEST: double free to the exact same block...\n");
    int res = BuddyAllocator_free(&allocator, blocks[0]);
    if (res == -1)
        printf("\33[1;32mOK\033[0m: Double free correctly revelead and handled.\n");
    else
        printf("\33[1;31mERROR\033[0m: Double free NOT releaved!\n");

    //Testing Malloc with differents sizes 
    printf("\n\033[1;36mPress ENTER to test allocation with different sizes...\033[0m");
    getchar();
    int sizes[] = { 1, 100, 512, 2048, 4096, 10000, 50000, 100000, 200000, 400000, 800000};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);
    void* ptrs[num_sizes];
    for (int i = 0; i < num_sizes; ++i) {
        ptrs[i] = BuddyAllocator_malloc(&allocator, sizes[i]);
        if (ptrs[i])
            printf("\33[1;32mOK\033[0m: Allocation of %d bytes Success, ptr=%p\n", sizes[i], ptrs[i]);
        else
            printf("\33[1;31mERROR\033[0m: Allocation of %d bytes FAILED :(\n", sizes[i]);
        BitMap_print(&allocator.bitmap);
        printf("\n\033[1;36mPress ENTER to continue...\033[0m");
        getchar();
    }

    //Testing Free with differents sizes 
    printf("\n\033[1;36mPress ENTER to test Free with different sizes...\033[0m");
    getchar();
    for (int i = 0; i < num_sizes; ++i) {
        if (BuddyAllocator_free(&allocator, ptrs[i])==-1) 
            printf("\33[1;31mERROR\033[0m: free of %d bytes FAILED :(\n", sizes[i]);
        else
            printf("\33[1;32mOK\033[0m: free of %d bytes Success, ptr=%p\n", sizes[i], ptrs[i]);
        BitMap_print(&allocator.bitmap);
        printf("\n\033[1;36mPress ENTER to continue...\033[0m");
        getchar();
    }
    
    //Testing Hard Free
    printf("\n\033[1;36mPress ENTER to test Hard Free. Now i'll create a string=\"hello!\" with my buddyAllocator_buddy...\033[0m");
    getchar();
    char* string = (char*)BuddyAllocator_malloc(&allocator, 16);
    if (string) { // Assicurati che l'allocazione sia riuscita
        strncpy(string, "hello!\n", 16 - 1); // Copia la stringa, lasciando spazio per il terminatore null
        string[16 - 1] = '\0'; // Assicurati che la stringa sia terminata con null
        printf("printing string1: %s\n", string);
    } else {
        printf("ERROR: Impossible allocate memory!\n");
    }
    printf("\n\033[1;36mPress ENTER to free the pointer...\033[0m");
    getchar();
    if(BuddyAllocator_free(&allocator, (void*)string)==-1){
        printf("ERROR: something went wrong with the free!\n");
    }
    else printf("OK: String now is free\n");
    printf("\n\033[1;36mPress ENTER to show the content of string (he is free)...\033[0m");
    getchar();
    printf("printing string: ");
    printf("%s\n",string);

    printf("\n\033[1;36mPress ENTER to do the exact same thing but with HARD free...\033[0m");
    getchar();
    char* string2 = (char*)BuddyAllocator_malloc(&allocator, 16);
    if (string2) { // Assicurati che l'allocazione sia riuscita
        strncpy(string2, "hello!\n", 16 - 1); // Copia la stringa, lasciando spazio per il terminatore null
        string2[16 - 1] = '\0'; // Assicurati che la stringa sia terminata con null
        printf("printing string2: %s\n", string2);
    } else {
        printf("ERROR: Impossible allocate memory!\n");
    }

    printf("\n\033[1;36mPress ENTER to free the pointer...\033[0m");
    getchar();
    if(BuddyAllocator_HardFree(&allocator, (void*)string2)==-1){
        printf("ERROR: something went wrong with the free!\n");
    }
    else printf("OK: String now is Hard free\n");
    printf("\n\033[1;36mPress ENTER to show the content of string2 (he is HardFree, so the content should be 0)...\033[0m");
    getchar();
    printf("printing string2: ");
    printf("%s\n",string2);

    printf("|FINISHING TESTING|\n");
    return 0;
}