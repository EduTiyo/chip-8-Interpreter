#include "display.h"

void Display::clear() {
  for(int i = 0; i < 64 * 32; i++){ 
    this->DISPLAY[i] = 0;
  }
}