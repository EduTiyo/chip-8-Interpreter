#include "c8vm.h"
#include "defs.h"
#include "SDL2/SDL.h"
#include <string>
#include <iostream>

using namespace std;

// --- LÓGICA DE TIMING
int TIMER_HERTZ = 60;

int CPU_HERTZ = 500;

int CICLOS_POR_FRAME = (CPU_HERTZ / TIMER_HERTZ);

int FRAME_DELAY = (1000 / TIMER_HERTZ);

int scale = 10; 

int main(int argc, char const *argv[])
{
  char* rom_path;
  for (int i = 0; i < argc; i++)
  {
    string arg = argv[i];
    if (arg == "--help") {
      cout << "Uso: ./chip8 [opcoes] roms/NOME_DA_ROM" << endl;
      cout << "Opções de Linha de Comando:" << endl;
      cout << "--clock <velocidade>: Define a velocidade do clock da CPU em Hz. Padrão: 500Hz." << endl;
      cout << "--scale <fator>: Define o fator de escala da janela. \n Um fator de 10 resulta em uma janela de 640x320. Padrão: 10." << endl;
      cout << "--help: Exibe esta mensagem de ajuda." << endl;
      return 0;
    }
    else if (arg == "--scale" && i + 1 < argc) {
      scale = stoi(argv[i + 1]);
    }
    else if (arg == "--clock" && i + 1 < argc) {
      CPU_HERTZ = stoi(argv[i + 1]);
      CICLOS_POR_FRAME = (CPU_HERTZ / TIMER_HERTZ);
      FRAME_DELAY = (1000 / TIMER_HERTZ);
    }
    else {
      rom_path = (char*)arg.c_str();
    }
  }

  VM vm(scale);
  vm.inicializar(0x200);

  if (!vm.carregarROM(rom_path, 0x200)) {
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