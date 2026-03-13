#include "AudioManager.h"
#include <iostream>
#include <algorithm>

AudioManager::~AudioManager() {
    // wipe our vector of instances and dictionary of sound assets to put them out of scope and cause their destructors to run
    soundInstances.clear();
    soundAssets.clear();
    if (device != 0) {
        SDL_CloseAudioDevice(device);
        device = 0;
    }
}

void AudioManager::init() {
    // open the device
    device = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);
    if(device == 0) {
        throw std::runtime_error(std::string("Failed to open audio device:") + SDL_GetError());
    }
}

void AudioManager::createSound(const std::string& filePath, const std::string& key) {
    soundAssets.emplace(key, std::make_shared<Sound>(filePath));
}

void AudioManager::playSound(const std::string& key, bool shouldLoop) {
    auto sound = soundAssets.at(key);
    // unique to ensure the instance is deleted when it goes out of scope
    auto instance = std::make_unique<SoundInstance>(sound);
    instance->looping = shouldLoop;
    instance->stream = SDL_CreateAudioStream(&sound->audioSpec, NULL);
    if (!instance->stream) {
        throw std::runtime_error(std::string("Failed to create audio stream: ") + SDL_GetError());
    }
    // bind to the device w/error checking
    if (!SDL_BindAudioStream(device, instance->stream)) {
        throw std::runtime_error(std::string("Failed to bind audio stream: ") + SDL_GetError());
    }
    // actually plays the sound by loading sound data into the audio stream; again, w/error checking
    if (!SDL_PutAudioStreamData(instance->stream, sound->waveStart, sound->waveLength)) {
        throw std::runtime_error(std::string("Failed to queue audio data: ") + SDL_GetError());
    }
    soundInstances.push_back(std::move(instance));
}

// refill sound instances that are meant to loop + clear dead instances
void AudioManager::update() {
    for(auto &instance : soundInstances) {
        // null checking; skip sound instances that aren't there or lack an audio stream
        if(!instance || !instance->stream) continue;
        const int bytesQueued = SDL_GetAudioStreamQueued(instance->stream);

        // refill threshold set to half
        const int refillThreshold = static_cast<int>(instance->soundAsset->waveLength / 2);

        // if the audio loops and has equal to or less than half the length of its audio left to play, refill the stream
        if ((bytesQueued <= refillThreshold) && instance->looping) {
            if(!SDL_PutAudioStreamData(instance->stream, instance->soundAsset->waveStart, instance->soundAsset->waveLength)) {
                throw std::runtime_error(std::string("Failed to refill looping audio: ") + SDL_GetError());
            }
        }
    }

    clearInstances();
}

// erase-remove idiom to clear dead sound instances; 
// if instances in the vector lack data to play, they're moved to the end of the vector and removed with erase
void AudioManager::clearInstances() {
    auto noBytesQueued = [](std::unique_ptr<SoundInstance>& soundInstance) { 
        // returns true (that there are no bytes queued for the audio stream) if there is no sound instance, if there is no stream,
        // if the audio is meant to loop indefinitely or if SDL_GetAudioStreamQueued returns 0
        return !soundInstance || !soundInstance->stream || (!soundInstance->looping && SDL_GetAudioStreamQueued(soundInstance->stream) == 0);
    };
    soundInstances.erase(std::remove_if(soundInstances.begin(), soundInstances.end(), noBytesQueued), soundInstances.end());
}