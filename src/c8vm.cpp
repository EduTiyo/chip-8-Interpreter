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

  for(int i = 0; i < 16; i++) {
      keypad[i] = 0; // liberar as teclas
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
      // NOP
      else if (inst == 0x0000) {
        // No operation
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

        
    case 3:
      // 3XNN: Skip next if equal
      if(V[X] == NN) {
        this->PC += 2;
      }
      break;

    // 4XNN: Skip next if not equal
    case 4:
      if(V[X] != NN) {
        this->PC += 2;
      }
      break;

    // 5xy0 : Skip next if Vx == Vy
    case 5:
      if(V[X] == V[Y]) {
        this->PC += 2;
      }
      break;

    // 6XNN: Set
    case 6: 
      V[X] = NN;
      break;

    // 7XNN: Add
    case 7: 
      V[X] += NN; 
      break;

    // 9XY0: Skip next if Vx != Vy
    case 9:
      if(V[X] != V[Y]) {
        this->PC += 2;
      }
      break;

    case 8:
      // 8XY0: Set Vx = Vy
      if (N == 0) {
        V[X] = V[Y];
      }
      // 8XY1: Set Vx = Vx OR Vy
      else if (N == 1) {
        V[X] |= V[Y];
      }
      // 8XY2: Set Vx = Vx AND Vy
      else if (N == 2) {
        V[X] &= V[Y];
      }
      // 8XY3: Set Vx = Vx XOR Vy
      else if (N == 3) {
        V[X] ^= V[Y];
      }
      // 8XY4: Add Vy to Vx, set VF = carry
      else if (N == 4) {
        uint16_t soma = V[X] + V[Y];
        V[0xF] = (soma > 255) ? 1 : 0;
        V[X] = soma & 0xFF;
      }
      // 8XY5: Subtract Vy from Vx, set VF = NOT borrow
      else if (N == 5) {
        V[0xF] = (V[X] >= V[Y]) ? 1 : 0;
        V[X] -= V[Y];
      }
      // 8XY6: Shift Vx right by 1, set VF = least
      else if (N == 6) {
        V[0xF] = V[X] & 0x01;
        V[X] >>= 1;
      }
      // 8XY7: Set Vx = Vy - Vx, set VF = NOT borrow
      else if (N == 7) {
        if (V[Y] > V[X]) {
          V[0xF] = 1;
        } else {
          V[0xF] = 0;
        }
        V[X] = V[Y] - V[X];
      }
      // 8XYE: Shift Vx left by 1, set VF = most
      else if (N == 0xE) {
        V[0xF] = V[X] & 0x80;
        V[X] <<= 1;
      }
      else {
        printf("Instrução 0x8 não reconhecida: 0x%04X\n", inst);
        exit(1);
      }
      break;

    // ANNN: Set index
    case 0xA:
      I = NNN;
      break;

    case 0xB:
      // BNNN: Jump with offset
      PC = V[0] + NNN;
      break;

    case 0xC:
      // CXNN: Random
      V[X] = (rand() % 256) & NN;
      break;

    // DXYN: Draw
    case 0xD: {
      int xcoord = V[X] % 64;
      int ycoord = V[Y] % 32;
      V[0xF] = this->display.drawSprite(xcoord, ycoord, &this->RAM[I], N);
      break;
    }

    case 0xE:
    // EX9E: Skip next if key in VX is pressed
      if (NN == 0x9E) {
        if (isKeyPressed(V[X])) {
          this->PC += 2;
        }
      }
    // EXA1: Skip next if key in VX is NOT pressed
      else if (NN == 0xA1) {
        if (!isKeyPressed(V[X])) {
          this->PC += 2;
        }
      }
      else {
        printf("Instrução 0xE não reconhecida: 0x%04X\n", inst);
        exit(1);
      }
      break;


    // 2NNN: Call subroutine
    case 0x2:
      if (SP >= 16) { printf("Stack overflow!\n"); exit(1); }
      stack[SP++] = PC;
      PC = NNN;
      break;

    case 0xF:
      // FX07: Vx = delay_timer
      if (NN == 0x07) {        
          V[X] = delay_timer;
      }
      // FX15: delay_timer = Vx
      else if (NN == 0x15) {   
          delay_timer = V[X];
      }
      // FX18: sound_timer = Vx
      else if (NN == 0x18) {   
          sound_timer = V[X];
      } 
      // FX1E: I = I + Vx
      else if (NN == 0x1E) {    
          uint16_t soma = I + V[X];
          if (soma > 0xFFF) {
              V[0xF] = 1; 
          } else {
              V[0xF] = 0;
          }
          I = soma & 0xFFF;
      }
      // FX29: set sprite location for digit VX to I
      else if (NN == 0x29) {
        I = V[X] * 0x05;
      }
      // FX33: Store BCD representation
      else if (NN == 0x33) {
        uint8_t valor = V[X];
        const int h = valor / 100;
        const int t = (valor - h * 100) / 10;
        const int o = valor - h * 100 - t * 10;

        RAM[I] = h;
        RAM[I + 1] = t;
        RAM[I + 2] = o;
      } else if (NN == 0x65) {
        // FX65 Load the memory data starting at address I into the registers v0 to vx
        for (int i = 0; i <= X; i++) {
          V[i] = RAM[I + i];
        }
      } else if (NN == 0x55) {
        // LD[I], VX: store register from V0 to VX int the main memory, starting at location I.
        for (int i = 0; i <= X; i++) {
          RAM[I + i] = V[i];
        }
      }
      else {
        printf("Instrução 0xF não reconhecida: 0x%04X\n", inst);
        exit(1);
      }
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

void VM::setKey(uint8_t key, bool pressed) {
    if (key < 16) keypad[key] = pressed;
}

bool VM::isKeyPressed(uint8_t key) {
    return (key < 16) ? keypad[key] : false;
}