#include <assert.h>
#include "../headers/bit_map.h"

// returns the number of bytes to store bits booleans
int BitMap_getBytes(int bits){
  int bytes = bits/8;
  if (bits%8) return bytes+1; //if there are any bits left, then I add an extra byte
  return bytes;
}

// initializes a bitmap on an external array
void BitMap_init(BitMap* bit_map, int num_bits, uint8_t* buffer);

// sets a the bit bit_num in the bitmap
// status= 0 or 1
void BitMap_setBit(BitMap* bit_map, int bit_num, int status);

// inspects the status of the bit bit_num
int BitMap_bit(const BitMap* bit_map, int bit_num);

// print the entire bitmap
void BitMap_print(BitMap* bit_map);