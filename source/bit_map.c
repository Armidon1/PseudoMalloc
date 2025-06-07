#include <assert.h>
#include <string.h>
#include "../headers/bit_map.h"

// returns the number of bytes to store bits booleans
int BitMap_getBytes(int bits){
  int bytes = bits/8;
  if (bits%8) return bytes+1; //if there are any bits left, then I add an extra byte
  return bytes;
}

// initializes a bitmap on an external array. it just fills the struct with its own values
void BitMap_init(BitMap* bit_map, int num_bits, uint8_t* buffer){
  bit_map->buffer=buffer;
  bit_map->num_bits=num_bits;
  bit_map->buffer_size=BitMap_getBytes(num_bits);
  memset(bit_map->buffer, 0, bit_map->buffer_size); //clear the buffer
}

// sets a the bit bit_num in the bitmap
// status= 0 or 1
void BitMap_setBit(BitMap* bit_map, int bit_num, int status);

// inspects the status of the bit bit_num
int BitMap_bit(const BitMap* bit_map, int bit_num);

// print the entire bitmap
void BitMap_print(BitMap* bit_map){
  printf("BitMap: ");
  for (int i=0; i<bit_map->num_bits; i++){
    int byte_num = i / 8; //I take the byte cell
    int bit_in_byte = 7 - (i % 8);  //i take the extra bits, in particular im iterating the byte from left to right: needed for a correct print
    int bit = (bit_map->buffer[byte_num] >> bit_in_byte) & 1; 
    printf("%d", bit);
  }
  printf("\n");
}