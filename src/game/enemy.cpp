#include "enemy.h"

#include <SDL3/SDL_render.h>

#include <cmath>

Enemy::Enemy(SDL_Texture* atlasTexture)
{
    this->texture = atlasTexture;
    this->tag = Tag::enemy;
    this->dynamic = true;
    this->collider = {4, 6, 24, 26};
    this->velocity.x = walkSpeed * direction;

    animations.emplace_back(4, 0.8f, 0, 32, 32);
}

void Enemy::update(float deltaTime, const bool* keys)
{
    if (state == EnemyState::Dead)
        return;

    if (!grounded)
        velocity.y += gravity * deltaTime;

    // Keep moving in the current direction
    velocity.x = walkSpeed * direction;

    this->grounded = false;

    if (!animations.empty())
    {
        animations[currentAnim].step(deltaTime);
    }

    GameObject::update(deltaTime, keys);
}

void Enemy::Render(SDL_Renderer* renderer, glm::vec2 offset)
{
    if (state == EnemyState::Dead || animations.empty())
        return;

    SDL_FRect src = animations[currentAnim].GetCurrentFrameSrc();
    SDL_FRect dst = {position.x + offset.x, position.y + offset.y, src.w, src.h};

    SDL_FlipMode flip = (direction == 1) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
    SDL_RenderTextureRotated(renderer, texture, &src, &dst, 0, nullptr, flip);
}

void Enemy::takeDamage()
{
    state = EnemyState::Dead;
    this->collider = {0, 0, 0, 0};
    this->velocity = {0, 0};
}

void Enemy::reverseDirection()
{
    direction *= -1.0f;
    velocity.x = walkSpeed * direction;
}
