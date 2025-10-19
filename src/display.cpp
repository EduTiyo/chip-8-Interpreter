#include "display.h"

Display::Display(int pixel_scale) : scale(pixel_scale) {
  for(int i = 0; i < 64 * 32; i++){ 
    this->DISPLAY[i] = 0;
  }

  if(SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("Erro ao inicializar SDL: %s\n", SDL_GetError());
    exit(1);
  }

  window = SDL_CreateWindow(
    "CHIP-8 Interpreter",
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    64 * scale,
    32 * scale,
    SDL_WINDOW_SHOWN
  );

  if(!window) {
    printf("Erro ao criar janela: %s\n", SDL_GetError());
    exit(1);
  }

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if(!renderer) {
    printf("Erro ao criar renderer: %s\n", SDL_GetError());
    exit(1);
  }

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);
  SDL_RenderPresent(renderer);
}

Display::~Display() {
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

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

void Display::render() {
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  
  for(int y = 0; y < 32; y++) {
    for(int x = 0; x < 64; x++) {
      if(DISPLAY[y * 64 + x]) {
        SDL_Rect rect = {x * scale, y * scale, scale, scale};
        SDL_RenderFillRect(renderer, &rect);
      }
    }
  }

  SDL_RenderPresent(renderer);
}

bool Display::isOpen() {
  SDL_Event event;
  while(SDL_PollEvent(&event)) {
    if(event.type == SDL_QUIT) {
      return false;
    }
  }
  return true;
}