#pragma once

#include <SDL3/SDL_render.h>

#include <glm/ext/vector_float2.hpp>
#include <glm/glm.hpp>
#include <vector>

#include "animation.h"

enum class PlayerState
{
    idle,
    running,
    jumping

};

struct PlayerData
{
    PlayerState state;
    PlayerData() { state = PlayerState::idle; }
};

struct LevelData
{
};

struct EnemyData
{
};

union ObjectData
{
    PlayerData player;
    LevelData level;
    EnemyData enemy;
};

enum class ObjectType
{
    player,
    level,
    enemy
};

struct GameObject
{
    ObjectType type;
    ObjectData data;
    glm::vec2 position, velocity, acceleration;
    float direction;
    float maxSpeedX;
    std::vector<Animation> animations;
    int currentAnimation;
    SDL_Texture* texture;

    GameObject() : data{.level = LevelData()}
    {
        type = ObjectType::level;
        direction = 1;
        maxSpeedX = 0;
        position = velocity = acceleration = glm::vec2(0);
        currentAnimation = -1;
        texture = nullptr;
    }
};
