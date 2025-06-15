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
void BitMap_setBit(BitMap* bit_map, int bit_num, int status){
  int byte_num = bit_num >> 3; //bit_num/8 = which byte is 
  if (byte_num >= bit_map->buffer_size){  //check if we are inside the buffer
    fprintf(stderr,"byte_num=%d is higher or equal than bit_map->buffer_size=%d\n", byte_num, bit_map->buffer_size);
    return;
  }
  int bit_in_byte = 7 - (bit_num % 8);  //finding which bit is inside the byte
  if (status) bit_map->buffer[byte_num] |= (1 << bit_in_byte); //set bit 1 in location bit_in_byte
  else  bit_map->buffer[byte_num] &= ~(1 << bit_in_byte); //set bit 0 in location bit_in_byte
}

// inspects the status of the bit bit_num
int BitMap_bit(const BitMap* bit_map, int bit_num){
  int byte_num = bit_num >> 3;              //bit_num/8 = which byte is 
  if (byte_num >= bit_map->buffer_size){    //check if we are inside the buffer
    fprintf(stderr,"byte_num=%d is higher or equal than bit_map->buffer_size=%d\n", byte_num, bit_map->buffer_size);
    return;
  }
  int bit_in_byte = 7 - (bit_num % 8);  //finding which bit is inside the byte;
  return (bit_map->buffer[byte_num] & (1 << bit_in_byte)) != 0;
}


void BitMap_print(BitMap* bit_map){
  printf("BitMap:\n");
  int level = 0;
  int nodes_in_level = 1;
  int printed = 0;
  for (int i = 0; i < bit_map->num_bits; i++) {
    int byte_num = i / 8;   //I take the byte cell
    int bit_in_byte = 7 - (i % 8);    //i take the extra bits, in particular im iterating the byte from left to right: needed for a correct print
    int bit = (bit_map->buffer[byte_num] >> bit_in_byte) & 1;
    printf("%d", bit);
    printed++;
    if (printed == nodes_in_level) {
      printf("\n");
      level++;
      nodes_in_level <<= 1; // nodes_in_level *= 2
      printed = 0;
    }
  }
  if (printed != 0) printf("\n");
}
