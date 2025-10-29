#include "c8vm.h"
#include "defs.h"
#include "SDL2/SDL.h"

// --- LÓGICA DE TIMING

// Specs: "A tela... 60Hz" e "Os temporizadores... 60Hz."
const int TIMER_HERTZ = 60;

// Specs: "A velocidade... configurável... (ex: 500Hz)"
const int CPU_HERTZ = 700; 

// Quantos ciclos de CPU por frame (700Hz / 60Hz = ~11.6)
// Arredondamos para 12
const int CICLOS_POR_FRAME = CPU_HERTZ / TIMER_HERTZ;

// Tempo de cada frame (1000ms / 60Hz = 16.66ms)
const int FRAME_DELAY = 1000 / TIMER_HERTZ; 


int main(int argc, char const *argv[])
{
  VM vm;
  vm.inicializar(0x200);

  if (!vm.carregarROM(argv[1], 0x200)) {
    printf("Uso correto: %s roms/NOME_DA_ROM\n", argv[0]);
    return 1; // Sai se não conseguiu carregar a ROM
  }
  
  // Flag para controlar o loop principal
  bool quit = false;

  while(!quit) {

    // Pega o tempo exato de quando o frame começou
    Uint32 frameStart = SDL_GetTicks();
    
    // --- Processamento de Input ---
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        if(event.type == SDL_QUIT) {
            quit = true; // Seta a flag para sair do loop
            break; // Sai do loop de eventos
        }

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

    for (int i = 0; i < CICLOS_POR_FRAME; ++i) {
        vm.executarInstrucao();
    }

    vm.atualizarTimers();

    vm.renderDisplay();


    Uint32 frameTime = SDL_GetTicks() - frameStart;

    if (frameTime < FRAME_DELAY) {
        SDL_Delay(FRAME_DELAY - frameTime);
    }
  }
  
  return 0;
}