#include "c8vm.h"
#include <stdio.h>
#include <stdlib.h>

void VM::inicializar(uint16_t pc_inicial) {
  this->PC = pc_inicial;
  this->SP = 0;
  this->I = 0;
  this->delay_timer = 0;
  this->sound_timer = 0;
  for(int i = 0; i < 4096; i++) {
    this->RAM[i] = 0;
    if(i < 16) {
      this->V[i] = 0;
      this->stack[i] = 0;
    }
  }
  this->display.clear();
}

void VM::carregarROM(const char* arq_rom, uint16_t pc_inicial) {
  FILE* rom = fopen(arq_rom, "rb");
  
  if(rom == NULL) {
    printf("Erro ao abrir o arquivo %s\n", arq_rom);
    return;
  }
  
  fseek(rom, 0, SEEK_END);
  int tam_rom = ftell(rom);
  rewind(rom);
  
  if(tam_rom > (4096 - pc_inicial)) {
    printf("Erro: ROM muito grande para caber na memoria\n");
    fclose(rom);
    return;
  }

  fread(&this->RAM[pc_inicial], sizeof(uint8_t), tam_rom, rom);
  fclose(rom);
}

void VM::executarInstrucao() {
  uint16_t inst = (this->RAM[this->PC] << 8) | this->RAM[this->PC+1];
  this->PC += 2;
  uint8_t  grupo = inst >> 12;
  uint8_t  X     = (inst & 0x0F00) >> 8;
  uint8_t  Y     = (inst & 0X00F0) >> 4;
  uint8_t  N     = (inst & 0x000F);
  uint8_t  NN    = (inst & 0x0FF);
  uint16_t NNN   = (inst & 0xFFF);

  switch(grupo) {

    case 0:
      // CLS
      if(inst == 0x00E0){
        this->display.clear();
        break;
      }
    
    default:
      printf("Grupo não implementado. Instrução 0x%04X\n", inst);
      exit(1);
  }
}

void VM::imprimirRegistradores() {
    printf("PC: 0x%04X I: 0x%04X SP: 0x%02X\n", this->PC, this->I, this->SP);
    for(int i = 0; i < 16; i++) {
        printf("V[%X]: 0x%02X ", i, this->V[i]);
    }
    printf("\n");
}