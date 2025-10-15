#include <stdint.h>

class Display {
  private:
    uint8_t DISPLAY[64 * 32];
  public:
    void clear();
    uint8_t drawSprite(int x, int y, uint8_t* sprite, int n);
};