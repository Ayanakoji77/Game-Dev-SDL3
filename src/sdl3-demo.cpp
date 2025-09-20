#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_messagebox.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_video.h>
#include <SDL3_image/SDL_image.h>

#include <string>

int GAME_HEIGHT = 720;
int GAME_WIDTH = 1280;

struct SDLState
{
    SDL_Window* MyWindow;
    SDL_Renderer* renderer;
};

void cleanup(SDLState& win);

int main(int argc, char* argv[])
{
    // Intialization of the state sdl
    SDLState state;

    // Intialization of the sdl
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Sdl3  Intialization Failed",
                                 nullptr);
        return 1;
    }

    const char* basePath = SDL_GetBasePath();
    if (!basePath)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error",
                                 "Sdl3  Base Path Intialization Failed", nullptr);
        return 1;
    }
    // creation of the Window
    state.MyWindow = SDL_CreateWindow("Sdl Demo", GAME_WIDTH, GAME_HEIGHT, SDL_WINDOW_RESIZABLE);

    if (!state.MyWindow)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Window creating error",
                                 state.MyWindow);
        cleanup(state);
        return 1;
    }

    // create the renderer

    state.renderer = SDL_CreateRenderer(state.MyWindow, nullptr);
    if (!state.renderer)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "error", "Render not able to intialize",
                                 state.MyWindow);
        cleanup(state);
        return 1;
    }

    // configure  presentation
    int logWidth = 1280;
    int logHeight = 720;

    SDL_SetRenderLogicalPresentation(state.renderer, logWidth, logHeight,
                                     SDL_LOGICAL_PRESENTATION_LETTERBOX);

    std::string imagePath = std::string(basePath) + "data/Sprite-0001-2t.png";
    SDL_Texture* idleTex = IMG_LoadTexture(state.renderer, imagePath.c_str());
    SDL_SetTextureScaleMode(idleTex, SDL_SCALEMODE_NEAREST);

    // start the game loop
    bool running = true;
    while (running)
    {
        SDL_Event event{0};
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_EVENT_QUIT:
                    running = false;
                    break;
                case SDL_EVENT_WINDOW_RESIZED:
                    GAME_HEIGHT = event.window.data2;
                    GAME_WIDTH = event.window.data1;
                    break;
            }
        }

        // some draw commands
        SDL_SetRenderDrawColor(state.renderer, 20, 10, 30, 255);
        SDL_RenderClear(state.renderer);

        SDL_FRect src{.x = 0, .y = 0, .w = 32, .h = 32};
        SDL_FRect dest{.x = 0, .y = 0, .w = 32, .h = 32};
        SDL_RenderTexture(state.renderer, idleTex, &src, &dest);

        // swap of buffers
        SDL_RenderPresent(state.renderer);
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
