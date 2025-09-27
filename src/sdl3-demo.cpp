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

#include <array>
#include <cstdint>
#include <glm/glm.hpp>
#include <string>
#include <vector>

#include "gameobject.h"
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

const size_t LAYER_IDX_LEVEL = 0;
const size_t LAYER_IDX_CHARACTERS = 1;

struct GameState
{
    std::array<std::vector<GameObject>, 2> layers;
    int playerIndex;

    GameState()
    {
        playerIndex = 0;  // will chnage this when we load maps
    }
};
struct Resources
{
    const int ANIM_PLAYER_IDLE = 0;
    std::vector<Animation> playerAnims;

    SDL_Texture* idleTex;
    std::vector<SDL_Texture*> textures;
    SDL_Texture* loadTexture(SDLState& state, const std::string& filepath)
    {
        std::string imagePath = std::string(state.basePath) + filepath;
        SDL_Texture* tex = IMG_LoadTexture(state.renderer, imagePath.c_str());
        SDL_SetTextureScaleMode(tex, SDL_SCALEMODE_NEAREST);

        textures.push_back(tex);
        return tex;
    }

    void load(SDLState& state)
    {
        playerAnims.resize(5);
        playerAnims[ANIM_PLAYER_IDLE] = Animation(4, 0.8f);

        idleTex = loadTexture(state, "data/Sprite-0001-2t.png");
    }

    void unload()
    {
        for (SDL_Texture* tex : textures)
        {
            SDL_DestroyTexture(tex);
        }
    }
};

void cleanup(SDLState& state);
bool intialize(SDLState& state);
void drawObject(const SDLState& state, GameState& gs, GameObject& obj, float deltaTime);

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
    Resources res;
    res.load(state);

    // setup game data
    GameState gs;
    GameObject player;
    player.type = ObjectType::player;
    player.texture = res.idleTex;
    player.animations = res.playerAnims;
    player.currentAnimation = res.ANIM_PLAYER_IDLE;
    gs.layers[LAYER_IDX_CHARACTERS].push_back(player);

    const bool* keys = SDL_GetKeyboardState(nullptr);

    uint64_t prevTime = SDL_GetTicks();

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

        // update all objects
        for (auto& layer : gs.layers)
        {
            for (GameObject& obj : layer)
            {
                if (obj.currentAnimation != -1)
                {
                    obj.animations[obj.currentAnimation].step(deltaTime);
                }
            }
        }
        SDL_SetRenderDrawColor(state.renderer, 20, 10, 30, 255);
        SDL_RenderClear(state.renderer);

        // draw all objects
        // layers are the peicies on screen one after another and it contains the game objects or
        // sprites for each layer
        for (auto& layer : gs.layers)
        {
            for (GameObject& obj : layer)
            {
                drawObject(state, gs, obj, deltaTime);
            }
        }
        // swap of buffers
        SDL_RenderPresent(state.renderer);
        prevTime = nowTime;
    }

    res.unload();
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

void drawObject(const SDLState& state, GameState& gs, GameObject& obj, float deltaTime)
{
    const float spriteSize = 32;
    float srcX = obj.currentAnimation != 1
                     ? obj.animations[obj.currentAnimation].currentFrame() * spriteSize
                     : 0.0f;

    SDL_FRect src{.x = srcX, .y = 0, .w = spriteSize, .h = spriteSize};
    SDL_FRect dest{.x = obj.position.x, .y = obj.position.y, .w = spriteSize, .h = spriteSize};

    SDL_FlipMode flipMode = obj.direction == -1 ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
    SDL_RenderTextureRotated(state.renderer, obj.texture, &src, &dest, 0, nullptr, flipMode);
}
