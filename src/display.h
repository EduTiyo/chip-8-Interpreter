#include <stdint.h>
#include <SDL2/SDL.h>

class Display {
  private:
    uint8_t DISPLAY[64 * 32];
    SDL_Window* window;
    SDL_Renderer* renderer;
    int scale = 10;
  public:
    Display(int pixel_scale);
    ~Display();
  
    void clear();
    uint8_t drawSprite(int x, int y, uint8_t* sprite, int n);
    void render();
    bool isOpen();
};