#include <stdint.h>

class VM {
  private:
    uint8_t RAM[4096];
    uint16_t PC;
    uint8_t V[16];
    uint8_t SP;
    uint16_t I;
    uint8_t stack[16];
    uint8_t DISPLAY[64 * 32];
    uint8_t delay_timer;
    uint8_t sound_timer;
  public:
    void inicializar(uint16_t pc_inicial);
    void carregarROM(const char* arq_rom, uint16_t pc_inicial);
    void executarInstrucao();
    void imprimirRegistradores();
};