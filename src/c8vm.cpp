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
  printf("PC: 0x%04X - Opcode: 0x%04X\n", this->PC, inst);

  this->PC += 2;
  uint8_t  grupo = inst >> 12;
  uint8_t  X     = (inst & 0x0F00) >> 8;
  uint8_t  Y     = (inst & 0X00F0) >> 4;
  uint8_t  N     = (inst & 0x000F);
  uint8_t  NN    = (inst & 0x00FF);
  uint16_t NNN   = (inst & 0x0FFF);

  switch(grupo) {

    case 0:
      // CLS
      if(inst == 0x00E0){
        this->display.clear();
      }
      // RET
      else if (inst == 0x00EE) {
        if (SP == 0) { printf("Stack underflow!\n"); exit(1); }
        PC = stack[--SP];
      }
      else {
        printf("Instrução 0x0 não reconhecida: 0x%04X\n", inst);
        exit(1);
      }
      break;
    
    // 1NNN: Jump
    case 1:
      this->PC = NNN; 
      break;

    // 6XNN: Set
    case 6: 
      V[X] = NN;
      break;

    // 7XNN: Add
    case 7: 
      V[X] += NN; 
      break;

    // ANNN: Set index
    case 0xA:
      I = NNN;
      break;

    // DXYN: Draw
    case 0xD: {
      int xcoord = V[X] % 64;
      int ycoord = V[Y] % 32;
      V[0xF] = this->display.drawSprite(xcoord, ycoord, &this->RAM[I], N);
      break;
    }

    // 2NNN: Call subroutine
    case 0x2:
      if (SP >= 16) { printf("Stack overflow!\n"); exit(1); }
      stack[SP++] = PC + 2;
      PC = NNN;
      break;

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

bool VM::displayIsOpen() {
  return this->display.isOpen();
}

void VM::renderDisplay() {
  this->display.render();
}