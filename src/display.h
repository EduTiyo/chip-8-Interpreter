#include <stdint.h>

class Display {
  private:
    uint8_t DISPLAY[64 * 32];
  public:
    void clear();
};