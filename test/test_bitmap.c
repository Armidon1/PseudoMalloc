#include <assert.h>
#include <stdio.h>
#include "../headers/bit_map.h"

int main(){
    //creating a bitmap
    int num_bits = 8; //just one byte
    int buffer_size = BitMap_getBytes(num_bits);
    uint8_t buffer[buffer_size];
    BitMap bm;
    BitMap_init(&bm, num_bits, buffer);
    printf("created a Bitmap with num_bits = %d\n", num_bits);

    //testing bitmap_print
    printf("testing BitMap_print\n");
    BitMap_print(&bm);

    
    return 0;
}
