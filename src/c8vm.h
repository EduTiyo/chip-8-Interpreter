#include <stdint.h>
#include "display.h"
#include "audio.h"

class VM {
  private:
    uint8_t RAM[4096];
    uint16_t PC;
    uint8_t V[16];
    uint8_t SP;
    uint16_t I;
    uint16_t stack[16];
    Display display;
    Audio audio;
    uint8_t delay_timer;
    uint8_t sound_timer;
    uint8_t keypad[16];
    void executarInstrucaoF(uint8_t X, uint8_t NN, uint16_t inst);
    bool romValida(const char* arq_rom);

    public:
    VM() : display(10), audio(44100) {}
    VM(int pixel_scale) : display(pixel_scale), audio(44100) {}
    void inicializar(uint16_t pc_inicial);
    bool carregarROM(const char* arq_rom, uint16_t pc_inicial);
    void executarInstrucao();
    void imprimirRegistradores();
    bool displayIsOpen();
    void renderDisplay();
    void setKey(uint8_t key, bool pressed);
    bool isKeyPressed(uint8_t key);
    void atualizarTimers();
  };