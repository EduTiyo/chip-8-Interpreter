#include "audio.h"
#include <stdlib.h>
#include <string.h>

Audio::Audio(int sample_rate_) : device(0), buffer(nullptr), buflen(0), playing(false), sample_rate(sample_rate_) {
  SDL_InitSubSystem(SDL_INIT_AUDIO);

  // parâmetros de áudio
  spec.freq = sample_rate;
  spec.format = AUDIO_U8;
  spec.channels = 1;
  spec.samples = 4096;
  spec.callback = nullptr;
  spec.userdata = nullptr;

  // gera 1s de onda quadrada
  generateSquareWave(440);

  device = SDL_OpenAudioDevice(nullptr, 0, &spec, nullptr, 0);
  if (device == 0) {
    // falha ao abrir, mas não fatal — VM continua sem som
    buffer = nullptr;
    buflen = 0;
  } else {
    // deixa pausado até ser pedido play()
    SDL_PauseAudioDevice(device, 1);
  }
}

Audio::~Audio() {
  if (device != 0) {
    SDL_CloseAudioDevice(device);
  }
  if (buffer) {
    free(buffer);
  }
  // não fecha SDL por inteiro (Display ainda usa SDL); apenas fecha subsistema
  SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

void Audio::generateSquareWave(int freq_hz) {
  // 1 segundo de amostras
  buflen = sample_rate;
  buffer = (Uint8*)malloc(buflen);
  if (!buffer) {
    buflen = 0;
    return;
  }

  const int amplitude = 60;
  const float period = (float)sample_rate / (float)freq_hz;
  for (int i = 0; i < buflen; ++i) {
    float pos = fmodf((float)i, period);
    Uint8 sample = 128 + (pos < (period * 0.5f) ? amplitude : -amplitude);
    buffer[i] = sample;
  }
}

void Audio::play() {
  if (device == 0 || buffer == nullptr) return;
  if (!playing) {
    SDL_ClearQueuedAudio(device);
    SDL_QueueAudio(device, buffer, buflen);
    SDL_PauseAudioDevice(device, 0);
    playing = true;
  } else {
    if (SDL_GetQueuedAudioSize(device) < (Uint32)(buflen / 2)) {
      SDL_QueueAudio(device, buffer, buflen);
    }
  }
}

void Audio::stop() {
  if (device == 0) return;
  SDL_ClearQueuedAudio(device);
  SDL_PauseAudioDevice(device, 1);
  playing = false;
}