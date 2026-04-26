#pragma once
#include <SDL3/SDL_rect.h>

#include <array>
#include <memory>
#include <vector>

#include "bullet.h"
#include "core/camera.h"
#include "core/resourceManager.h"
#include "enemy.h"
#include "game/player.h"
#include "gameobject.h"

struct ParallaxLayer
{
    SDL_Texture* texture;
    float scrollSpeed{0.0f};  // 0.0 static backgorund render , and 1,0 player moves
    float yposition = 0;
};

class Level
{
   private:
    // layers background , player and blocks
    static const int LAYER_IDX_LEVEL = 0;
    static const int LAYER_IDX_CHARACTERS = 1;
    static const int TOTAL_LAYERS = 2;
    std::unique_ptr<Camera> camera;
    Player* player = nullptr;
    std::array<std::vector<std::unique_ptr<GameObject>>, TOTAL_LAYERS> layers;
    std::vector<GameObject> backgroundTiles;
    std::vector<GameObject> foregroundTiles;
    std::vector<Bullet> bullets;
    std::vector<ParallaxLayer> backgroundLayers;
    static const int MAP_ROWS = 5;
    static const int MAP_COLS = 50;
    static const int TILE_SIZE = 32;
    void CheckCollisions(float deltaTime);
    void ResolveCollision(GameObject& a, GameObject& b, float deltaTime, SDL_FRect intersection);

   public:
    void LoadMap(ResourceManager* res);
    void Update(float deltaTime, const bool* keys);
    void Render(SDL_Renderer* renderer, bool debugMode);
    void ParallaxBackgroundDraw(SDL_Renderer* renderer);
    void SetMap(short map[MAP_ROWS][MAP_COLS], short background[MAP_ROWS][MAP_COLS],
                short foreground[MAP_ROWS][MAP_COLS]);
    void UpdateGroundState();
};
