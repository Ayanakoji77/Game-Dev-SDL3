#include "application.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_video.h>

#include "core/resourceManager.h"

bool Application::Initialize()
{
    bool initSuccess = true;
    // Intialization of the sdl
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Sdl3  Intialization Failed",
                                 nullptr);
        initSuccess = false;
    }

    this->basePath = SDL_GetBasePath();
    if (this->basePath == nullptr)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error",
                                 "Sdl3  Base Path Intialization Failed", nullptr);
        initSuccess = false;
    }
    // creation of the Window
    this->window =
        SDL_CreateWindow("Sdl Demo", this->GAME_WIDTH, this->GAME_HEIGHT, SDL_WINDOW_RESIZABLE);

    if (this->window == nullptr)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Window creating error",
                                 this->window);
        this->Destroy();
        initSuccess = false;
    }

    // create the renderer

    this->renderer = SDL_CreateRenderer(this->window, nullptr);
    if (this->renderer == nullptr)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "error", "Render not able to intialize",
                                 this->window);
        this->Destroy();
        initSuccess = false;
    }

    // loading the resources
    this->resourceManager = new ResourceManager(renderer, this->basePath ? this->basePath : "");

    resourceManager->LoadTexture("player", "data/player.png");
    resourceManager->LoadTexture("ground", "data/Ground.png");
    resourceManager->LoadTexture("panel", "data/Panel.png");
    resourceManager->LoadTexture("grass", "data/Grass.png");
    resourceManager->LoadTexture("brick", "data/Brick.png");
    resourceManager->LoadTexture("background_1", "data/Background_1.png");
    resourceManager->LoadTexture("background_2", "data/Background_2.png");
    resourceManager->LoadTexture("bullet", "data/bullet-sheet.png");
    resourceManager->LoadTexture("enemy", "data/player.png");

    // intialize input
    this->keys = SDL_GetKeyboardState(nullptr);
    // configure  presentation

    SDL_SetRenderLogicalPresentation(this->renderer, this->logWidth, this->logHeight,
                                     SDL_LOGICAL_PRESENTATION_LETTERBOX);

    SDL_SetRenderVSync(this->renderer, 1);
    // intialize level
    currentLevel = new Level();
    currentLevel->LoadMap(this->resourceManager);

    return initSuccess;
}

void Application::Destroy()
{
    delete currentLevel;
    delete resourceManager;
    SDL_DestroyRenderer(this->renderer);
    SDL_DestroyWindow(this->window);
    SDL_Quit();
}

void Application::Run()
{
    bool running = true;
    uint64_t prevTime = SDL_GetTicks();

    while (running)
    {
        // delta time
        uint64_t nowTime = SDL_GetTicks();
        float deltaTime = (nowTime - prevTime) / 1000.0f;
        prevTime = nowTime;

        // input polling
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
            {
                running = false;
            }
            else if (event.type == SDL_EVENT_WINDOW_RESIZED)
            {
                this->GAME_WIDTH = event.window.data1;
                this->GAME_HEIGHT = event.window.data2;
            }
            else if (keys[SDL_SCANCODE_F3])
            {
                debugMode = !debugMode;
            }
        }

        // game update level
        if (currentLevel)
        {
            currentLevel->Update(deltaTime, keys);
        }

        // render game
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 20, 10, 30, 255);
        SDL_FRect bgreact = {0, 0, static_cast<float>(logWidth), static_cast<float>(logHeight)};
        SDL_RenderFillRect(renderer, &bgreact);

        if (currentLevel)
        {
            currentLevel->Render(renderer, debugMode);
        }

        SDL_RenderPresent(renderer);
    }
}
