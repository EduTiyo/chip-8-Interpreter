#include "c8vm.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

#define FONTSET_START 0x50

void VM::inicializar(uint16_t pc_inicial) {
  this->PC = pc_inicial;
  this->SP = 0;
  this->I = 0;
  this->delay_timer = 0;
  this->sound_timer = 0;
  
  // Limpa a RAM (4096 bytes)
  for(int i = 0; i < 4096; i++) {
    this->RAM[i] = 0;
  }

  // Limpa Registradores (16), Stack (16) e Keypad (16)
  for(int i = 0; i < 16; i++) {
    this->V[i] = 0;
    this->stack[i] = 0;
    this->keypad[i] = 0; // liberar as teclas
  }

  this->display.clear();

  static const uint8_t fontset[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
  };

  // Carrega os sprites da fonte na RAM a partir do endereço 0x50 (FONTSET_START)
  for (int i = 0; i < 80; i++) {
    this->RAM[FONTSET_START + i] = fontset[i];
  }

}

bool VM::romValida(const char* nome) {
    // Caminho base aceito
    const char* base = "roms/";

    // Verifica se o nome começa com roms/
    if (strncmp(nome, base, strlen(base)) != 0) {
        printf("Erro: ROM deve estar no diretório roms/\n");
        return false;
    }

    // Verifica se o arquivo existe
    struct stat buffer;
    if (stat(nome, &buffer) != 0) {
        printf("Erro: Arquivo %s não encontrado.\n", nome);
        return false;
    }

    // Verifica se não é diretório
    if (S_ISDIR(buffer.st_mode)) {
        printf("Erro: %s é um diretório, não um arquivo.\n", nome);
        return false;
    }

    return true;
}

bool VM::carregarROM(const char* arq_rom, uint16_t pc_inicial) {
  if (!romValida(arq_rom)) {
    return false;
  }

  FILE* rom = fopen(arq_rom, "rb");
  if(rom == NULL) {
    printf("Erro ao abrir o arquivo %s\n", arq_rom);
    return false;
  }
  
  fseek(rom, 0, SEEK_END);
  int tam_rom = ftell(rom);
  rewind(rom);
  
  if(tam_rom > (4096 - pc_inicial)) {
    printf("Erro: ROM muito grande para caber na memoria\n");
    fclose(rom);
    return false;
  }

  fread(&this->RAM[pc_inicial], sizeof(uint8_t), tam_rom, rom);
  fclose(rom);

  return true;
}

void VM::executarInstrucao() {
  uint16_t inst = (this->RAM[this->PC] << 8) | this->RAM[this->PC+1];
  printf("PC: 0x%04X - Opcode: 0x%04X\n", this->PC, inst);

  this->PC += 2;
  uint8_t  grupo = inst >> 12;
  uint8_t  X     = (inst & 0x0F00) >> 8;
  uint8_t  Y     = (inst & 0x00F0) >> 4;
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
      // SYS NNN (Ignorado)
      else {
        printf("Instrução 0x0 (SYS) não implementada: 0x%04X\n", inst);
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
        V[0xF] = (V[Y] >= V[X]) ? 1 : 0;
        V[X] = V[Y] - V[X];
      }
      // 8XYE: Shift Vx left by 1, set VF = most
      else if (N == 0xE) {
        V[0xF] = V[X] >> 7;
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
      executarInstrucaoF(X, NN, inst);
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

void VM::executarInstrucaoF(uint8_t X, uint8_t NN, uint16_t inst) {
    // FX0A: Wait for key press, store in VX
    if (NN == 0x0A) {
        bool keyPressed = false;
        for (int i = 0; i < 16; i++) {
            if (this->keypad[i]) {
                this->V[X] = i; 
                keyPressed = true;
                break;
            }
        }

        if (!keyPressed) {
            this->PC -= 2; 
        }
    }
    // FX07: Vx = delay_timer
    else if (NN == 0x07) {        
        this->V[X] = this->delay_timer;
    }
    // FX15: delay_timer = Vx
    else if (NN == 0x15) {   
        this->delay_timer = this->V[X];
    }
    // FX18: sound_timer = Vx
    else if (NN == 0x18) {   
        this->sound_timer = this->V[X];
    } 
    // FX1E: I = I + Vx
    else if (NN == 0x1E) {    
        uint16_t soma = this->I + this->V[X];
        this->V[0xF] = (soma > 0xFFF) ? 1 : 0; 
        this->I = soma; // **CORRIGIDO**
    }
    // FX29: set sprite location for digit VX to I
    else if (NN == 0x29) {
        this->I = FONTSET_START + (this->V[X] & 0x0F) * 0x05;
    }
    // FX33: Store BCD representation
    else if (NN == 0x33) {
        uint8_t valor = this->V[X];
        this->RAM[this->I]     = valor / 100;
        this->RAM[this->I + 1] = (valor / 10) % 10;
        this->RAM[this->I + 2] = valor % 10;
    } 
    // FX65: Load memory into V0..VX
    else if (NN == 0x65) {
        for (int i = 0; i <= X; i++) {
            this->V[i] = this->RAM[this->I + i];
        }
    } 
    // FX55: Store V0..VX into memory
    else if (NN == 0x55) {
        for (int i = 0; i <= X; i++) {
            this->RAM[this->I + i] = this->V[i];
        }
    }

    else {
        printf("Instrução 0xF não reconhecida: 0x%04X\n", inst);
        exit(1);
    }
}

void VM::atualizarTimers() {
    if (this->delay_timer > 0) {
        --this->delay_timer;
    }
    if (this->sound_timer > 0) {
        --this->sound_timer;
    }

    // controla reprodução do beep via SDL
    if (this->sound_timer > 0) {
        this->audio.play();
    } else {
        this->audio.stop();
    }
}