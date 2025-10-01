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
#include <cstdlib>
#include <cstring>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <string>
#include <vector>

#include "gameobject.h"
#include "timer.h"

struct SDLState
{
    SDL_Window* MyWindow;
    SDL_Renderer* renderer;
    int GAME_HEIGHT, GAME_WIDTH, logWidth, logHeight;
    const bool* keys;
    SDLState() : keys(SDL_GetKeyboardState(nullptr)) {}

    const char* basePath;
};

const size_t LAYER_IDX_LEVEL = 0;
const size_t LAYER_IDX_CHARACTERS = 1;
const int MAP_ROWS = 5;
const int MAP_COLS = 50;
const int TILE_SIZE = 32;

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
    const int ANIM_PLAYER_RUN = 1;
    std::vector<Animation> playerAnims;

    SDL_Texture *idleTex, *runTex, *groundTex, *panelTex, *brickTex, *grassTex;
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
        playerAnims[ANIM_PLAYER_IDLE] = Animation(7, 1.2f);
        playerAnims[ANIM_PLAYER_RUN] = Animation(8, 0.6f);
        idleTex = loadTexture(state, "data/Sprite-0001.png");
        runTex = loadTexture(state, "data/Run-sheet.png");
        groundTex = loadTexture(state, "data/Ground.png");
        panelTex = loadTexture(state, "data/Panel.png");
        brickTex = loadTexture(state, "data/Brick.png");
        grassTex = loadTexture(state, "data/Grass.png");
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
void update(const SDLState& state, GameState& gs, Resources& res, GameObject& obj, float deltaTime);
void createTiles(const SDLState& state, GameState& gs, const Resources& res);

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
    createTiles(state, gs, res);
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
                update(state, gs, res, obj, deltaTime);
                // update the animation
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
    float srcX = obj.currentAnimation != -1
                     ? obj.animations[obj.currentAnimation].currentFrame() * spriteSize
                     : 0.0f;

    SDL_FRect src{.x = srcX, .y = 0, .w = spriteSize, .h = spriteSize};
    SDL_FRect dest{.x = obj.position.x, .y = obj.position.y, .w = spriteSize, .h = spriteSize};

    SDL_FlipMode flipMode = obj.direction == -1 ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
    SDL_RenderTextureRotated(state.renderer, obj.texture, &src, &dest, 0, nullptr, flipMode);
}

void update(const SDLState& state, GameState& gs, Resources& res, GameObject& obj, float deltaTime)
{
    if (obj.dynamic)
    {
        obj.velocity += glm::vec2(0, 500) * deltaTime;
    }

    if (obj.type == ObjectType::player)
    {
        float currentDirection = 0;
        if (state.keys[SDL_SCANCODE_A])
        {
            currentDirection += -1;
        }
        if (state.keys[SDL_SCANCODE_D])
        {
            currentDirection += 1;
        }

        if (currentDirection)
        {
            obj.direction = currentDirection;
        }

        switch (obj.data.player.state)
        {
            case PlayerState::idle:
            {
                if (currentDirection)
                {
                    obj.data.player.state = PlayerState::running;
                    obj.texture = res.runTex;
                    obj.currentAnimation = res.ANIM_PLAYER_RUN;
                }
                else
                {
                    // deaccelatere
                    if (obj.velocity.x)
                    {
                        const float factor = obj.velocity.x > 0 ? -1.5f : 1.5f;
                        float amount = factor * obj.acceleration.x * deltaTime;

                        if (std::abs(obj.velocity.x) < std::abs(amount))
                        {
                            obj.velocity.x = 0;
                        }
                        else
                        {
                            obj.velocity.x += amount;
                        }
                    }
                }
                break;
            }
            case PlayerState::running:
            {
                if (!currentDirection)
                {
                    obj.data.player.state = PlayerState::idle;
                    obj.texture = res.idleTex;
                    obj.currentAnimation = res.ANIM_PLAYER_IDLE;
                }
                break;
            }
        }

        // add acceleration to velocity

        obj.velocity += currentDirection * obj.acceleration * deltaTime;
        if (std::abs(obj.velocity.x) > obj.maxSpeedX)
        {
            obj.velocity.x = currentDirection * obj.maxSpeedX;
        }
    }

    // add velocity to position
    obj.position += obj.velocity * deltaTime;
}
void checkCollison(const SDLState& state, GameState& gs, Resources& res, GameObject& a,
                   GameObject& b, float deltaTime)
{
    SDL_FRect reactA{.x = a.position.x, .y = a.position.y, .w = TILE_SIZE, .h = TILE_SIZE};
    SDL_FRect reactB{.x = b.position.x, .y = b.position.y, .w = TILE_SIZE, .h = TILE_SIZE};

    SDL_FRect reactC{0};

    if (SDL_GetRectIntersectionFloat(&reactA, &reactB, &reactC))
    {
    }
}
void createTiles(const SDLState& state, GameState& gs, const Resources& res)
{
    /*
     * 1.Ground
     * 2.Panel
     * 3.Enemy
     * 4.Player
     * 5.Grass
     * 6.Brick
     */
    short map[MAP_ROWS][MAP_COLS];

    memset(map, 0, MAP_ROWS * MAP_COLS);
    map[0][0] = 4;
    map[4][0] = map[4][1] = map[4][2] = map[3][1] = 1;

    // r = rows
    // c = columns
    const auto createObject = [&state](int r, int c, SDL_Texture* tex, ObjectType type)
    {
        GameObject o;
        o.type = type;

        o.position = glm::vec2(c * TILE_SIZE, state.logHeight - (MAP_ROWS - r) * TILE_SIZE);

        o.texture = tex;
        return o;
    };
    for (int i = 0; i < MAP_ROWS; i++)
    {
        for (int j = 0; j < MAP_COLS; j++)
        {
            switch (map[i][j])
            {
                case 1:  // Ground
                {
                    GameObject o = createObject(i, j, res.groundTex, ObjectType::level);
                    gs.layers[LAYER_IDX_LEVEL].push_back(o);
                    break;
                }
                case 2:  // Panel
                {
                    GameObject o = createObject(i, j, res.panelTex, ObjectType::level);
                    gs.layers[LAYER_IDX_LEVEL].push_back(o);
                    break;
                }
                case 4:  // player
                {
                    GameObject player = createObject(i, j, res.idleTex, ObjectType::player);
                    player.data.player = PlayerData();
                    player.animations = res.playerAnims;
                    player.currentAnimation = res.ANIM_PLAYER_IDLE;
                    player.acceleration = glm::vec2(250, 0);
                    player.maxSpeedX = 80;
                    player.dynamic = true;
                    gs.layers[LAYER_IDX_CHARACTERS].push_back(player);
                    break;
                }
            }
        }
    }
}
