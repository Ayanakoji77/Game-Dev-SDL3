#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_messagebox.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include <SDL3_image/SDL_image.h>

#include <cstdint>
#include <string>
#include <vector>

#include "animation.h"
#include "timer.h"

struct SDLState
{
    SDL_Window* MyWindow;
    SDL_Renderer* renderer;
    int GAME_HEIGHT;
    int GAME_WIDTH;

    int logWidth;
    int logHeight;
    const char* basePath;
};

void cleanup(SDLState& state);
bool intialize(SDLState& state);

struct Resources
{
    const int ANIM_PLAYER_IDLE = 0;
    std::vector<Animation> playerAnims;

    std::vector<SDL_Texture*> textures;
    SDL_Texture* loadTexture()

        void load(SDLState& state)
    {
        playerAnims.resize(5);
        playerAnims[ANIM_PLAYER_IDLE] = Animation(8, 1.6f);
    }
};
int main(int argc, char* argv[])
{
    // Intialization of the state sdl
    SDLState state;

    state.GAME_WIDTH = 1600;
    state.GAME_HEIGHT = 900;
    state.logWidth = 640;
    state.logHeight = 320;

    if (!intialize(state))
    {
        return 1;
    }

    std::string imagePath = std::string(state.basePath) + "data/Sprite-0001-2t.png";
    SDL_Texture* idleTex = IMG_LoadTexture(state.renderer, imagePath.c_str());
    SDL_SetTextureScaleMode(idleTex, SDL_SCALEMODE_NEAREST);

    // setup game data
    const bool* keys = SDL_GetKeyboardState(nullptr);
    float playerX = 150;
    float floor = state.logHeight;
    uint64_t prevTime = SDL_GetTicks();
    bool flipHorizontal = false;
    // start the game loop
    bool running = true;
    while (running)
    {
        uint64_t nowTime = SDL_GetTicks();
        float deltaTime = (nowTime - prevTime) / 1000.0f;
        SDL_Event event{0};
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_EVENT_QUIT:
                    running = false;
                    break;
                case SDL_EVENT_WINDOW_RESIZED:
                    state.GAME_HEIGHT = event.window.data2;
                    state.GAME_WIDTH = event.window.data1;
                    break;
            }
        }
        // handle movement
        float moveAmount = 0;
        if (keys[SDL_SCANCODE_A])
        {
            moveAmount += -75.0f;
            flipHorizontal = true;
        }
        if (keys[SDL_SCANCODE_D])
        {
            moveAmount += 75.0f;
            flipHorizontal = false;
        }
        playerX += moveAmount * deltaTime;
        const float spriteSize = 32;

        // some draw commands
        SDL_SetRenderDrawColor(state.renderer, 20, 10, 30, 255);
        SDL_RenderClear(state.renderer);

        SDL_FRect src{.x = 0, .y = 0, .w = spriteSize, .h = spriteSize};
        SDL_FRect dest{.x = playerX, .y = floor - spriteSize, .w = spriteSize, .h = spriteSize};
        SDL_RenderTextureRotated(state.renderer, idleTex, &src, &dest, 0, nullptr,
                                 (flipHorizontal) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
        // swap of buffers
        SDL_RenderPresent(state.renderer);
        prevTime = nowTime;
    }

    SDL_DestroyTexture(idleTex);
    cleanup(state);
    return 0;
}

void cleanup(SDLState& state)
{
    SDL_DestroyRenderer(state.renderer);
    SDL_DestroyWindow(state.MyWindow);
    SDL_Quit();
}

bool intialize(SDLState& state)
{
    bool initSuccess = true;
    // Intialization of the sdl
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Sdl3  Intialization Failed",
                                 nullptr);
        initSuccess = false;
    }

    state.basePath = SDL_GetBasePath();
    if (!state.basePath)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error",
                                 "Sdl3  Base Path Intialization Failed", nullptr);
        initSuccess = false;
    }
    // creation of the Window
    state.MyWindow =
        SDL_CreateWindow("Sdl Demo", state.GAME_WIDTH, state.GAME_HEIGHT, SDL_WINDOW_RESIZABLE);

    if (!state.MyWindow)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Window creating error",
                                 state.MyWindow);
        cleanup(state);
        initSuccess = false;
    }

    // create the renderer

    state.renderer = SDL_CreateRenderer(state.MyWindow, nullptr);
    if (!state.renderer)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "error", "Render not able to intialize",
                                 state.MyWindow);
        cleanup(state);
        initSuccess = false;
    }

    // configure  presentation

    SDL_SetRenderLogicalPresentation(state.renderer, state.logWidth, state.logHeight,
                                     SDL_LOGICAL_PRESENTATION_LETTERBOX);

    return initSuccess;
}
