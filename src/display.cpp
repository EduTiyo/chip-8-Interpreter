#include "display.h"

void Display::clear() {
  for(int i = 0; i < 64 * 32; i++){ 
    this->DISPLAY[i] = 0;
  }
}

uint8_t Display::drawSprite(int x, int y, uint8_t* sprite, int n) {
  uint8_t collision = 0;
  int xcoord = x % 64;
  int ycoord = y % 32;

  for(int row = 0; row < n; row++) {
    uint8_t bits = sprite[row];
    int cy = (ycoord + row) % 32;

    for(int col = 0; col < 8; col++) {
      int cx = (xcoord + col) % 64;
      
      uint8_t bit = (bits >> (7 - col)) & 0x01;
      
      if(bit != 0) {
        int index = cy * 64 + cx;
        
        if(this->DISPLAY[index] == 1) {
          collision = 1;
        }
        
        this->DISPLAY[index] ^= 1;
      }

      if(cx == 63) {
        break;
      }
    }

    if(cy == 31) {
      break;
    }
  }

  return collision;
}