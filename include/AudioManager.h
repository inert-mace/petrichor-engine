#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include "Sound.h"
#include "SDL3/SDL.h"

class AudioManager {
    public:
        AudioManager() = default;
        ~AudioManager();
        void init();
        void createSound(const std::string& fileName, const std::string& key);
        void playSound(const std::string& key, bool shouldLoop);
        void stopSound(std::string key);
        void update();
        void clearInstances();
        std::unordered_map<std::string, std::shared_ptr<Sound>> soundAssets;
        std::vector<std::unique_ptr<SoundInstance>> soundInstances;
        SDL_AudioDeviceID device = 0;
};