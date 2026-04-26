
#include "game/bullet.h"

#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>

#include <glm/fwd.hpp>

#include "game/gameobject.h"
Bullet::Bullet(SDL_Texture* atlasTexture, glm::vec2 position, float direction)
{
    this->texture = atlasTexture;
    this->tag = Tag::bullet;
    this->dynamic = true;
    this->collider = {4, 4, 10, 8};
    this->state = BulletState::Moving;
    this->position = position + glm::vec2{18.0f, 15.0f};
    this->direction = direction;

    this->velocity = {bullet_velocity * direction, SDL_rand(yVariance) - yVariance};
    animations.emplace_back(4, 0.6f, 0, 16, 16);
    animations.emplace_back(4, 0.6f, 1, 16, 16);
}

void Bullet::reset(glm::vec2 pos, float dir)
{
    this->state = BulletState::Moving;
    this->position = pos + glm::vec2{18.0f, 15.0f};
    this->direction = dir;
    this->velocity = {bullet_velocity * dir, SDL_rand(yVariance) - yVariance};

    if (!animations.empty())
    {
        currentAnim = 0;
        animations[currentAnim].reset();
    }
}
void Bullet::update(float deltaTime, const bool* keys)
{
    (void)keys;

    float min_x_boudary = -100.0f;
    float max_x_boundary = (50 * 32) + 100.0f;
    if (state == BulletState::Moving)
    {
        if ((this->position.x < min_x_boudary || this->position.x > max_x_boundary))
        {
            this->state = BulletState::Inactive;
        }
        GameObject::update(deltaTime, keys);
    }
    else if (state == BulletState::Colliding)
    {
        currentAnim = 1;
        this->velocity *= 0;
        if (animations[currentAnim].isDone())
        {
            this->state = BulletState::Inactive;
        }
    }
    if (!animations.empty())
    {
        animations[currentAnim].step(deltaTime);
    }
}

void Bullet::Render(SDL_Renderer* renderer, glm::vec2 offset)
{
    if (animations.empty())
    {
        return;
    }

    SDL_FRect src = animations[currentAnim].GetCurrentFrameSrc();

    SDL_FRect dst = {position.x + offset.x, position.y + offset.y, src.w, src.h};
    SDL_FlipMode flip = (direction == -1) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
    SDL_RenderTextureRotated(renderer, texture, &src, &dst, 0, nullptr, flip);
}
