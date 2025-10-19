#include "c8vm.h"
#include "defs.h"
#include "SDL2/SDL.h"

int main(int argc, char const *argv[])
{
  VM vm;
  vm.inicializar(0x200);
  vm.carregarROM(argv[1], 0x200);
  #ifdef DEBUG
    vm.imprimirRegistradores();
  #endif

  while(vm.displayIsOpen()) {
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        if(event.type == SDL_QUIT) return 0;

        if(event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
            bool pressed = (event.type == SDL_KEYDOWN);
            switch(event.key.keysym.sym) {
              case SDLK_1: vm.setKey(0x1, pressed); break;
              case SDLK_2: vm.setKey(0x2, pressed); break;
              case SDLK_3: vm.setKey(0x3, pressed); break;
              case SDLK_4: vm.setKey(0xC, pressed); break;
              case SDLK_q: vm.setKey(0x4, pressed); break;
              case SDLK_w: vm.setKey(0x5, pressed); break;
              case SDLK_e: vm.setKey(0x6, pressed); break;
              case SDLK_r: vm.setKey(0xD, pressed); break;
              case SDLK_a: vm.setKey(0x7, pressed); break;
              case SDLK_s: vm.setKey(0x8, pressed); break;
              case SDLK_d: vm.setKey(0x9, pressed); break;
              case SDLK_f: vm.setKey(0xE, pressed); break;
              case SDLK_z: vm.setKey(0xA, pressed); break;
              case SDLK_x: vm.setKey(0x0, pressed); break;
              case SDLK_c: vm.setKey(0xB, pressed); break;
              case SDLK_v: vm.setKey(0xF, pressed); break;
            }
        }
    }

    vm.executarInstrucao();
    vm.renderDisplay();
    #ifdef DEBUG
      vm.imprimirRegistradores();
    #endif
  }
  return 0;
}
