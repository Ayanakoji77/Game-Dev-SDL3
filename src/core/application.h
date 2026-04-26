#pragma once
#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>

#include "game/Level.h"
#include "resourceManager.h"

class Application
{
   private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    const bool* keys = nullptr;
    const char* basePath = nullptr;
    ResourceManager* resourceManager = nullptr;
    Level* currentLevel = nullptr;
    int GAME_WIDTH = 1600;
    int GAME_HEIGHT = 900;
    const int logWidth = 640;
    const int logHeight = 320;
    bool debugMode = false;

   public:
    bool Initialize();
    void Destroy();
    void Run();
};
