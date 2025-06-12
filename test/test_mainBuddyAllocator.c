#include <stdio.h>
#include <stdlib.h>

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
    BuddyAllocator_init(&allocator, BUDDY_LEVELS, buffer_bitmap, buffer_size, memory, MIN_BUCKET_SIZE);
    printf("|Buddy Initializated|\n");

    // Alloca un blocco minimo
    void* p1 = BuddyAllocator_malloc(&allocator, MIN_BUCKET_SIZE);
    if (!p1) {
        printf("ERROR: Allocazione blocco minimo fallita!\n");
        return 1;
    }
    BitMap_print(&(allocator.bitmap));
    printf("OK: Allocazione blocco minimo riuscita, ptr=%p\n", p1);


    // Libera il blocco
    BuddyAllocator_free(&allocator, p1);
    printf("OK: Blocco minimo liberato.\n");
    BitMap_print(&(allocator.bitmap));

    // Prova a riallocare: dovrebbe riuscire e restituire lo stesso puntatore
    void* p2 = BuddyAllocator_malloc(&allocator, MIN_BUCKET_SIZE);
    BitMap_print(&(allocator.bitmap));
    if (p2 == p1)
        printf("OK: Riallocazione dopo free restituisce lo stesso blocco.\n");
    else if (p2)
        printf("OK: Riallocazione dopo free riuscita, ma puntatore diverso (va comunque bene).\n");
    else
        printf("ERROR: Riallocazione dopo free fallita!\n");
    BuddyAllocator_free(&allocator, p2);

    // Testa la liberazione di più blocchi
    BitMap_print(&(allocator.bitmap));
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