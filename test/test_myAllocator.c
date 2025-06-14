#include <stdio.h>
#include <string.h>

#include "../headers/myAllocator.h" 

#define PAGE_SIZE 4096 //needed for testing
#define MAX_SIZE_TO_USE_MYBUDDYALLOCATOR PAGE_SIZE/4


int main(){
    printf("|STARTING TESTING|\n");

    //Testing allocating 0 bytes
    printf("\n\033[1;36mWelcome to my final test. Press ENTER to Start with allocate memory!...\033[0m");
    getchar();
    printf("Let's start by checking if allocating 0 bytes:\n");
    void* pointer = myMalloc((0));
    if (pointer) {
        printf("\33[1;31mERROR\033[0m: Allocation succeded even if i'm allocating 0 bytes!\n");
        return 1;
    }
    printf("\33[1;32mOK\033[0m: Allocation Blocked\n");

    //testing allocating 40Bytes
    int size = sizeof(int) * 10;
    printf("\n\033[1;36mNow i allocate %dbytes. Press ENTER to continue!...\033[0m", size);
    getchar();
    pointer = myMalloc(size);
    if (!pointer) {
        printf("\33[1;31mERROR\033[0m: Something went wrong with MyMalloc!\n");
        return 1;
    }
    printf("\33[1;32mOK\033[0m: MyMalloc Succeded. Check if BuddyAllocator actually worked\n");

    //testing myFree which should use BuddyAllocator
    printf("\n\033[1;36mNow i free those size=%d. Press ENTER to continue!...\033[0m", size);
    getchar();
    if (myFree(pointer, size)==-1){
        printf("\33[1;31mERROR\033[0m: Something went wrong when i'm freeing those size=%d!\n", size);
        return 1;
    }
    printf("\33[1;32mOK\033[0m: MyFree Succeded. Check if BuddyAllocator actually worked\n");

    //testing allocating limits size to check if i'm using again BuddyAllocator_malloc
    size = MAX_SIZE_TO_USE_MYBUDDYALLOCATOR;
    printf("\n\033[1;36mNow i allocate %d bytes. Press ENTER to continue!...\033[0m",size);
    getchar();
    pointer = myMalloc(size);
    if (!pointer) {
        printf("\33[1;31mERROR\033[0m: Something went wrong!\n");
        return 1;
    }
    printf("\33[1;32mOK\033[0m: MyMalloc Succeded. Check if BuddyAllocator actually worked\n");

    //testing free of limits size to check if i'm using BuddyAllocator_free
    printf("\n\033[1;36mNow i free those size=%d. Press ENTER to continue!...\033[0m", size);
    getchar();
    if (myFree(pointer, size)==-1){
        printf("\33[1;31mERROR\033[0m: Something went wrong when i'm freeing those size=%d!\n", size);
        return 1;
    }
    printf("\33[1;32mOK\033[0m: MyFree Succeded. Check if BuddyAllocator actually worked\n");

    //testing allocation with mmap
    size +=1;
    printf("\n\033[1;36mNow i allocate %dbytes, it should use mmap. Press ENTER to continue!...\033[0m", size);
    getchar();
    pointer = myMalloc(size);
    if (!pointer) {
        printf("\33[1;31mERROR\033[0m: Something went wrong with MyMalloc!\n");
        return 1;
    }
    printf("\33[1;32mOK\033[0m: MyMalloc Succeded. Check if mmap actually worked\n");

    //testing free of munmap
    printf("\n\033[1;36mNow i free those size=%d. Press ENTER to continue!...\033[0m", size);
    getchar();
    if (myFree(pointer, size)==-1){
        printf("\33[1;31mERROR\033[0m: Something went wrong when i'm freeing those size=%d!\n", size);
        return 1;
    }
    printf("\33[1;32mOK\033[0m: MyFree Succeded. Check if munmap actually worked\n");
    
    //testing HARD free
    size = 100;
    printf("\n\033[1;36mNow i'll be testing myHardFree, allocating a memory of size=%d and the freeing it with that. Press ENTER to continue!...\033[0m", size);
    getchar();
    printf("i'm going to allocate with myMalloc...\n");
    char* string = (char*)myMalloc(size);
    strncpy(string, "Hello! i love pizza and mozzarella. I hope i won't be destroyed soon!", size - 1);
    string[size - 1] = '\0'; 
    printf("created a String: %s\n", string);
    printf("\n\033[1;36mPress ENTER to destroy that innocent string...\033[0m");
    getchar();
    if (myHardFree((void*)string, size)==-1){
        printf("\33[1;31mERROR\033[0m: Something went wrong when i'm freeing those size=%d!\n", size);
        return 1;
    }
    printf("\33[1;32mOK\033[0m: MyFree Succeded. Innocent String destroyed\n");
    printf("this is the string's content: %s\n", string);


    //finishing
    printf("|TESTING FINISHED!");
    return 0;
}