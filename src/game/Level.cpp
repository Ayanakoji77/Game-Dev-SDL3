#include "Level.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_blendmode.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>

#include <algorithm>
#include <format>
#include <glm/fwd.hpp>
#include <memory>

#include "core/camera.h"
#include "core/resourceManager.h"
#include "driver/SerialController.h"
#include "game/bullet.h"
#include "game/gameobject.h"
#include "player.h"

void Level::LoadMap(ResourceManager* res)
{
    backgroundLayers.push_back({res->GetTexture("background_1"), 0.0f, 0});
    backgroundLayers.push_back({res->GetTexture("background_2"), 0.5f, 220});
    short map[MAP_ROWS][MAP_COLS] = {{0}};
    short foreground[MAP_ROWS][MAP_COLS] = {{0}};
    short background[MAP_ROWS][MAP_COLS] = {{0}};
    this->SetMap(map, background, foreground);
    camera = std::make_unique<Camera>(640, 320, MAP_COLS * TILE_SIZE, 320);
    const auto load_map_layers = [res, this](short layer[MAP_ROWS][MAP_COLS])
    {
        for (int r = 0; r < MAP_ROWS; r++)
        {
            for (int c = 0; c < MAP_COLS; c++)
            {
                int type = layer[r][c];
                if (type == 0)
                    continue;

                float x = c * TILE_SIZE;
                float y = (320) - (MAP_ROWS - r) * TILE_SIZE;
                if (type == 4)
                {
                    auto pl = std::make_unique<Player>(res->GetTexture("player"));
                    pl->position = {x, y};
                    pl->onShoot = [this, res](glm::vec2 pos, float dir)
                    {
                        bool found_bullet = false;
                        for (int i = 0; i < (int)bullets.size() && !found_bullet; i++)
                        {
                            if (bullets[i].GetState() == BulletState::Inactive)
                            {
                                bullets[i].reset(pos, dir);
                                found_bullet = true;
                            }
                        }
                        if (!found_bullet)
                        {
                            Bullet b(res->GetTexture("bullet"), pos, dir);
                            this->bullets.push_back(std::move(b));
                        }
                    };
                    this->player = pl.get();
                    pl->tag = GameObject::Tag::player;
                    layers[LAYER_IDX_CHARACTERS].push_back(std::move(pl));
                }
                else if (type == 2)
                {
                    auto panel = std::make_unique<GameObject>();
                    panel->texture = res->GetTexture("panel");
                    panel->position = {x, y};
                    panel->tag = GameObject::Tag::level;
                    layers[LAYER_IDX_LEVEL].push_back(std::move(panel));
                }
                else if (type == 1)
                {
                    auto ground = std::make_unique<GameObject>();
                    ground->texture = res->GetTexture("ground");
                    ground->position = {x, y};
                    ground->tag = GameObject::Tag::level;
                    layers[LAYER_IDX_LEVEL].push_back(std::move(ground));
                }
                else if (type == 5)
                {
                    auto& grass = foregroundTiles.emplace_back();
                    grass.texture = res->GetTexture("grass");
                    grass.position = {x, y};
                    grass.tag = GameObject::Tag::level;
                }
                else if (type == 6)
                {
                    auto& brick = backgroundTiles.emplace_back();
                    brick.texture = res->GetTexture("brick");
                    brick.position = {x, y};
                    brick.tag = GameObject::Tag::level;
                }
            }
        }
    };
    load_map_layers(map);
    load_map_layers(foreground);
    load_map_layers(background);

    SDL_assert_release(this->player != nullptr && "No Player intialized check itup ");
}

void Level::Update(float deltaTime, const bool* keys, SerialController* serialController)
{
    for (auto& layer : layers)
    {
        for (auto& obj : layer)
        {
            obj->update(deltaTime, keys);
        }
    }
    for (auto& obj : bullets)
    {
        obj.update(deltaTime, keys);
    }

    if (player)
    {
        player->update(deltaTime, keys, serialController);
        camera->Follow(player->position);
    }

    // Check Physics
    CheckCollisions(deltaTime);
    UpdateGroundState();
}

void Level::Render(SDL_Renderer* renderer, bool debugMode)
{
    ParallaxBackgroundDraw(renderer);
    for (auto& obj : backgroundTiles)
    {
        obj.Render(renderer, camera->GetOffset());
    }
    for (int i = 0; i < this->TOTAL_LAYERS; i++)
    {
        for (auto& obj : layers[i])
        {
            obj->Render(renderer, camera->GetOffset());

            if (debugMode)
            {
                SDL_FRect rectA = {obj->position.x + obj->collider.x + camera->GetOffset().x,
                                   obj->position.y + obj->collider.y + camera->GetOffset().y,
                                   obj->collider.w, obj->collider.h};
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 150);
                SDL_RenderFillRect(renderer, &rectA);
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
            }
        }
    }
    for (auto& obj : bullets)
    {
        if (obj.GetState() == BulletState::Inactive)
            continue;
        obj.Render(renderer, camera->GetOffset());
        if (debugMode)
        {
            SDL_FRect rectA = {obj.position.x + obj.collider.x + camera->GetOffset().x,
                               obj.position.y + obj.collider.y + camera->GetOffset().y,
                               obj.collider.w, obj.collider.h};
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 150);
            SDL_RenderFillRect(renderer, &rectA);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        }
    }
    for (auto& obj : foregroundTiles)
    {
        obj.Render(renderer, camera->GetOffset());
    }
    if (debugMode)
    {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDebugText(renderer, 5, 5,
                            std::format("S:{} G:{} B:{}", static_cast<int>(player->getState()),
                                        player->isGrounded(), bullets.size())
                                .c_str());
    }
}

void Level::ParallaxBackgroundDraw(SDL_Renderer* renderer)
{
    float screenW = 640.0f;

    float camX = -camera->GetOffset().x;

    for (const auto& layer : backgroundLayers)
    {
        if (!layer.texture)
            continue;

        float w, h;
        SDL_GetTextureSize(layer.texture, &w, &h);

        float boundary_position_x = -std::fmod(camX * layer.scrollSpeed, w);

        if (boundary_position_x > 0)
        {
            boundary_position_x = boundary_position_x - w;
        }

        float current_x_pos = boundary_position_x;

        while (current_x_pos < screenW)
        {
            SDL_FRect dst = {current_x_pos, layer.yposition, w, h};
            SDL_RenderTexture(renderer, layer.texture, nullptr, &dst);
            current_x_pos += w;
        }
    }
}

void Level::CheckCollisions(float deltaTime)
{
    for (auto& tile : layers[LAYER_IDX_LEVEL])
    {
        for (auto& character : layers[LAYER_IDX_CHARACTERS])
        {
            if (!character->dynamic)
                continue;

            SDL_FRect rectA = {character->position.x + character->collider.x,
                               character->position.y + character->collider.y, character->collider.w,
                               character->collider.h};
            SDL_FRect rectB = {tile->position.x + tile->collider.x,
                               tile->position.y + tile->collider.y, tile->collider.w,
                               tile->collider.h};
            SDL_FRect intersection;

            if (SDL_GetRectIntersectionFloat(&rectA, &rectB, &intersection))
            {
                ResolveCollision(*character, *tile, deltaTime, intersection);
            }
        }
        for (auto& b : bullets)
        {
            if (b.GetState() == BulletState::Inactive)
            {
                continue;
            }
            else
            {
                SDL_FRect rectA = {b.position.x + b.collider.x, b.position.y + b.collider.y,
                                   b.collider.w, b.collider.h};
                SDL_FRect rectB = {tile->position.x + tile->collider.x,
                                   tile->position.y + tile->collider.y, tile->collider.w,
                                   tile->collider.h};
                SDL_FRect intersection;
                if (SDL_GetRectIntersectionFloat(&rectA, &rectB, &intersection))
                {
                    ResolveCollision(b, *tile, deltaTime, intersection);
                    b.SetState(BulletState::Colliding);
                }
            }
        }
    }
}
void Level::ResolveCollision(GameObject& a, GameObject& b, float deltaTime, SDL_FRect intersection)
{
    if ((a.tag == GameObject::Tag::player || a.tag == GameObject::Tag::bullet) &&
        b.tag == GameObject::Tag::level)
    {
        // Horizontal Collision
        if (intersection.w < intersection.h)
        {
            if (a.velocity.x > 0)
                a.position.x -= intersection.w;
            else if (a.velocity.x < 0)
                a.position.x += intersection.w;
            a.velocity.x = 0;
        }
        // Vertical Collision
        else
        {
            if (a.velocity.y > 0)
            {
                a.position.y -= intersection.h;
                a.velocity.y = 0;
                Player* p = static_cast<Player*>(&a);
                p->setGrounded(true);
            }
            else if (a.velocity.y < 0)
            {
                a.position.y += intersection.h;
                a.velocity.y = 0;
            }
        }
    }
}
void Level::SetMap(short map[MAP_ROWS][MAP_COLS], short background[MAP_ROWS][MAP_COLS],
                   short foreground[MAP_ROWS][MAP_COLS])
{
    /*
     * 1.Ground
     * 2.Panel
     * 3.Enemy
     * 4.Player
     * 5.Grass
     * 6.Brick
     */

    for (int j = 0; j < MAP_COLS; j++)
    {
        map[4][j] = 1;
        background[4][j] = 6;
        if (j % 5 == 0)
        {
            background[3][j] = 6;
            background[2][j] = 6;
        }
    }

    for (int j = 15; j < 20; j++)
    {
        map[4][j] = 0;
    }

    map[2][16] = 2;
    map[2][18] = 2;
    map[3][30] = 2;
    map[2][31] = 2;

    map[2][32] = map[2][33] = map[2][34] = 2;

    map[3][35] = 2;
    map[3][48] = 1;
    map[2][48] = 1;

    for (int i = 1; i < MAP_ROWS; i++)
    {
        for (int j = 0; j < MAP_COLS; j++)
        {
            if (map[i][j] == 1 || map[i][j] == 2)
            {
                foreground[i - 1][j] = 0;
            }
        }
    }

    map[3][2] = 4;
}

void Level::UpdateGroundState()
{
    if (player->velocity.y < 0)
    {
        player->setGrounded(false);
        return;
    }

    float sensorDist = 2.0f;
    SDL_FRect sensor = {player->position.x + player->collider.x + 1,
                        player->position.y + player->collider.y + player->collider.h,
                        player->collider.w - 2, sensorDist};

    SDL_FRect intersection;
    bool hitGround = false;
    for (auto& tile : layers[LAYER_IDX_LEVEL])
    {
        SDL_FRect tileRect = {tile->position.x + tile->collider.x,
                              tile->position.y + tile->collider.y, tile->collider.w,
                              tile->collider.h};

        if (SDL_GetRectIntersectionFloat(&tileRect, &sensor, &intersection))
        {
            hitGround = true;
            break;
        }
    }

    if (hitGround)
    {
        player->setGrounded(true);
    }
}
