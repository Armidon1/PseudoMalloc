#include <stdio.h>
#include <sys/mman.h>
#include <errno.h>
#include <string.h>

#include "../headers/myAllocator.h" 

#define SHOW_IN_TEST 1

BitMap bitmap;
char buffer_bitmap[BITMAP_BUFFER_SIZE];
BuddyAllocator buddy_allocator;
char memory[MEMORY_SIZE];

//AUX
int myCheck(int size_requested){
    return size_requested<=PAGE_SIZE/4;
}

__attribute__((constructor)) //this helps the user because initialize myAllocator before main()
void myAllocator_init(){ //my buddyAllocator_init already does everything 
    BuddyAllocator_init(&buddy_allocator, BUDDY_LEVELS, buffer_bitmap, BITMAP_BUFFER_SIZE, memory, MIN_BUCKET_SIZE);
}
void* myMalloc(int size_requested){
    if (size_requested<=0){
        printf("ERROR: myAllocator:malloc: requested size=%d<0. Returned NULL pointer.\n",size_requested);
        return NULL;
    }
    //int size_requested_for_buddyAllocator = size_requested+sizeof(int); 
    if (myCheck(size_requested)){
        return BuddyAllocator_malloc(&buddy_allocator, size_requested); //my buddyAllocator_malloc already handles errors and returns NULL
    }
    else{ //So here we use mmap
        void *pointer = mmap(NULL, size_requested, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (pointer == MAP_FAILED){
            errno = ENOMEM; //"NOT ENOUGH MEMORY"
            perror("ERROR: MMAP failed!"); // also prints a message which describe the error contained in errno 
            return NULL;
        }
        #if SHOW_IN_TEST == 1
            printf("OK: Poiter allocated memory with mmap\n");
        #endif
        return pointer;
    }
    
}
int myFree(void* pointer, int size_requested){
    if (pointer == NULL){   
        printf("ERROR: myFree: the pointer recieved is a NULL pointer!\n");
        return -1;
    }
    if (size_requested <= 0){   
        printf("ERROR: myFree: the size recieved is %d which is not ok!\n", size_requested);
        return -1;
    }

    //int size_requested_for_buddyAllocator = size_requested + sizeof(int);
    if (myCheck(size_requested)){
        if(BuddyAllocator_free(&buddy_allocator, pointer)==-1){
            printf("ERROR: myFree: something went wrong with BuddyAllocator_free\n");
            return -1;
        }
        return 0;  
    }
    else{
        if (munmap(pointer, size_requested) == -1){
            perror("ERROR: something went wrong with munmap");
            return -1;
        }
        #if SHOW_IN_TEST == 1
            printf("OK: Poiter free with munmap\n");
        #endif
        return 0;
    }
}

int myHardFree(void* pointer, int size_requested){
    if (pointer == NULL){   
        printf("ERROR: myFree: the pointer recieved is a NULL pointer!\n");
        return -1;
    }
    if (size_requested <= 0){   
        printf("ERROR: myFree: the size recieved is %d which is not ok!\n", size_requested);
        return -1;
    }

    //int size_requested_for_buddyAllocator = size_requested + sizeof(int);
    if (myCheck(size_requested)){
        if(BuddyAllocator_HardFree(&buddy_allocator, pointer)==-1){
            printf("ERROR: myFree: something went wrong with BuddyAllocator_free\n");
            return -1;
        }
        return 0;  
    }
    else{
        if (munmap(pointer, size_requested) == -1){ //munmap already destroys everything
            perror("ERROR: something went wrong with munmap");
            return -1;
        }
        #if SHOW_IN_TEST == 1
            printf("OK: Poiter free with munmap\n");
        #endif
        return 0;
    }
}