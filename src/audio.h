#pragma once
#include <SDL2/SDL.h>

class Audio {
  public:
    Audio(int sample_rate = 44100);
    ~Audio();

    void play();
    void stop();
    bool isPlaying() const { return playing; }

  private:
    void generateSquareWave(int freq_hz);

    SDL_AudioDeviceID device;
    SDL_AudioSpec spec;
    Uint8* buffer;
    int buflen;
    bool playing;
    int sample_rate;
};