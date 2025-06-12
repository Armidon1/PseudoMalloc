#include <stdio.h>
#include <stdlib.h>

#include "../headers/buddy_allocator.h"

//DEBUG INIT
#define SHOW_DEBUG_VALUES 0                 //must be 0 or 1
#define DEBUG_CHECK_INIT_BUFFER_BITMAP 0    //must be 0 or 1
#define DEBUG_CHECK_INIT_NEGATIVE_LEVELS 0  //must be o or 1

//DEBUG MALLOC
#define DEBUG_CHECK_MALLOC 3               //must be 0,1,2,3 or 4
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
        //BitMap_print(&allocator.bitmap);
    #endif

    #if DEBUG_CHECK_MALLOC == 3 || DEBUG_CHECK_MALLOC_ALL==1
        printf("TEST: allocation with different sizes\n");
        int sizes[] = { 1, 100, 512, 2048, 4096, 10000, 50000, 100000, 200000, 400000, 800000};
        int num_sizes = sizeof(sizes) / sizeof(sizes[0]);
        void* ptrs[num_sizes];
        for (int i = 0; i < num_sizes; ++i) {
            ptrs[i] = BuddyAllocator_malloc(&allocator, sizes[i]);
            if (ptrs[i])
                printf("OK: Allocation of %d bytes Success, ptr=%p\n", sizes[i], ptrs[i]);
            else
                printf("ERROR: Allocation of %d bytes FAILED :(\n", sizes[i]);
        }
        BitMap_print(&allocator.bitmap);
    #endif
    #if DEBUG_CHECK_MALLOC == 4 || DEBUG_CHECK_MALLOC_ALL==1
        printf("TEST: less different sizes allocations\n");
        int sizes[] = {400000, 800000 };
        int num_sizes = sizeof(sizes) / sizeof(sizes[0]);
        void* ptrs[num_sizes];
        for (int i = 0; i < num_sizes; ++i) {
            ptrs[i] = BuddyAllocator_malloc(&allocator, sizes[i]);
            if (ptrs[i])
                printf("OK: Allocation of %d bytes Success, ptr=%p\n", sizes[i], ptrs[i]);
            else
                printf("ERROR: Allocation of %d bytes FAILED :(\n", sizes[i]);
        }
    #endif

    printf("|FINISHING TESTING MALLOC|\n");
    return 0;
}
//NEEDED
int testing_free(){
    printf("|STARTING TESTING FREE|\n");
    BuddyAllocator allocator;
    int buffer_size = BitMap_getBytes(maxNumIndexesFromLevel(BUDDY_LEVELS));
    char buffer_bitmap[buffer_size];
    char memory[MEMORY_SIZE];
    BuddyAllocator_init(&allocator, BUDDY_LEVELS, buffer_bitmap, buffer_size, memory, MIN_BUCKET_SIZE);

    // Alloca un blocco minimo
    void* p1 = BuddyAllocator_malloc(&allocator, MIN_BUCKET_SIZE);
    if (!p1) {
        printf("ERROR: Allocazione blocco minimo fallita!\n");
        return 1;
    }
    printf("OK: Allocazione blocco minimo riuscita, ptr=%p\n", p1);

    // Libera il blocco
    BuddyAllocator_free(&allocator, p1);
    printf("OK: Blocco minimo liberato.\n");

    // Prova a riallocare: dovrebbe riuscire e restituire lo stesso puntatore
    void* p2 = BuddyAllocator_malloc(&allocator, MIN_BUCKET_SIZE);
    if (p2 == p1)
        printf("OK: Riallocazione dopo free restituisce lo stesso blocco.\n");
    else if (p2)
        printf("OK: Riallocazione dopo free riuscita, ma puntatore diverso (va comunque bene).\n");
    else
        printf("ERROR: Riallocazione dopo free fallita!\n");
    BuddyAllocator_free(&allocator, p1);

    // Testa la liberazione di più blocchi
    int total_blocks = 4;
    void* blocks[total_blocks];
    for (int i = 0; i < total_blocks; ++i) {
        blocks[i] = BuddyAllocator_malloc(&allocator, MIN_BUCKET_SIZE);
        if (!blocks[i]) {
            printf("ERROR: Allocazione multipla fallita al blocco %d!\n", i);
            return 1;
        }
        BitMap_print(&(allocator.bitmap));
    }
    BitMap_print(&(allocator.bitmap));
    for (int i = 0; i < total_blocks; ++i) {
        if (BuddyAllocator_free(&allocator, blocks[i])==-1) {
            printf("Something went wrong with the free\n");
            return 1;
        };
        BitMap_print(&(allocator.bitmap));
    }
    
    printf("OK: Liberazione multipla riuscita.\n");

    // TEST DOUBLE FREE
    printf("TEST: double free sul primo blocco...\n");
    int res = BuddyAllocator_free(&allocator, blocks[0]);
    if (res == -1)
        printf("OK: Double free correttamente rilevato e gestito.\n");
    else
        printf("ERROR: Double free NON rilevato!\n");

    printf("|FINISHING TESTING FREE|\n");
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