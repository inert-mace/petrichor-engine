#include <iostream>
#include "Sound.h"
#include "SDL3/SDL.h"
#include "SDL3/SDL_audio.h"
Sound::Sound(std::string filePath) {
    // loads the wav; if wav can't be loaded, logs an error.
    if(!SDL_LoadWAV(filePath.c_str(), &audioSpec, &waveStart, &waveLength)) {
        throw std::runtime_error(std::string("Sound loading error: ") + SDL_GetError());
    }
}

Sound::~Sound() {
    if (waveStart) {
        SDL_free(waveStart);
        waveStart = nullptr;
    }
}

SoundInstance::SoundInstance(std::shared_ptr<Sound> sound) : soundAsset(std::move(sound)) {}

SoundInstance::~SoundInstance() {
    if(stream) {
        SDL_DestroyAudioStream(stream);
        stream = nullptr;
    }
}