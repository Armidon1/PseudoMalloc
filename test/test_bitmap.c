#include <assert.h>
#include <stdio.h>
#include "../headers/bit_map.h"

int main(){
    //creating a bitmap
    int num_bits = 7; //just one byte, includes 0
    int buffer_size = BitMap_getBytes(num_bits);
    uint8_t buffer[buffer_size];
    BitMap bm;
    BitMap_init(&bm, num_bits, buffer);
    printf("created a Bitmap with num_bits = %d\n", num_bits);

    //testing bitmap_print
    printf("testing BitMap_print\n");
    BitMap_print(&bm);

    //testing bitmap_setBit and bitmap_bit
    printf("testing BitMap_setbit and BitMap_bit\n");
    int position =3;
    int status=1;
    printf("setting bit position=%d to status=%d\n", position, status);
    BitMap_setBit(&bm, position, status);
    printf("bit value: %d\n", BitMap_bit(&bm, position));
    BitMap_print(&bm);
    status=0;
    printf("setting bit position=%d to status=%d\n", position, status);
    BitMap_setBit(&bm, position, status);
    printf("bit value: %d\n", BitMap_bit(&bm, position));
    BitMap_print(&bm);

    return 0;
}
