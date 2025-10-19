#include <stdint.h>
#include "display.h"

class VM {
  private:
    uint8_t RAM[4096];
    uint16_t PC;
    uint8_t V[16];
    uint8_t SP;
    uint16_t I;
    uint8_t stack[16];
    Display display;
    uint8_t delay_timer;
    uint8_t sound_timer;
    uint8_t keypad[16];

    public:
    VM() : display(10) {}
    void inicializar(uint16_t pc_inicial);
    void carregarROM(const char* arq_rom, uint16_t pc_inicial);
    void executarInstrucao();
    void imprimirRegistradores();
    bool displayIsOpen();
    void renderDisplay();
    void setKey(uint8_t key, bool pressed);
    bool isKeyPressed(uint8_t key);
  };