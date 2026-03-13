#pragma once
#include <string>
#include <memory>
#include "SDL3/SDL.h"

class Sound {
    public:
        Sound(std::string filePath);
        ~Sound();
        // wave file properties
        SDL_AudioSpec audioSpec{};
        uint8_t* waveStart = nullptr;
        uint32_t waveLength = 0;
};

class SoundInstance {
    public:
    SoundInstance(std::shared_ptr<Sound> sound);
    ~SoundInstance();
    SDL_AudioStream* stream = nullptr;
    std::shared_ptr<Sound> soundAsset;
    bool looping = false;
};