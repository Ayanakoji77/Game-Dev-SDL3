#include "player.h"

#include <SDL3/SDL_render.h>
#include <SDL3/SDL_scancode.h>

#include <cstdlib>
#include <glm/fwd.hpp>

Player::Player(SDL_Texture* atlasTexture)
{
    // loading texture pointer
    // configure the player
    this->texture = atlasTexture;
    this->tag = Tag::player;
    this->dynamic = true;
    this->collider = {8, 6, 14, 26};
    weaponTimer.step(20.0f);
    animations.emplace_back(4, 0.6f, 2, 32, 32);  // idel
    animations.emplace_back(8, 1.2f, 3, 32, 32);  // run
    animations.emplace_back(8, 1.2f, 5, 32, 32);  // jump
    animations.emplace_back(2, 0.3f, 0, 32, 32);  // slide
}

void Player::update(float deltaTime, const bool* keys)
{
    float dirInput = 0;
    if (keys[SDL_SCANCODE_A])
        dirInput = -1;
    if (keys[SDL_SCANCODE_D])
        dirInput = 1;
    bool jumpPressed = keys[SDL_SCANCODE_SPACE];

    if (keys[SDL_SCANCODE_E])
    {
        if (weaponTimer.isTimeout())
        {
            if (onShoot)
                onShoot(this->position, this->direction);
            weaponTimer.reset();
        }
    }
    weaponTimer.step(deltaTime);
    switch (state)
    {
        case PlayerState::Idle:
            if (jumpPressed && this->isGrounded())
            {
                state = PlayerState::Jumping;
                velocity.y = jump_power;
                this->setGrounded(false);
            }
            else if (dirInput != 0)
            {
                state = PlayerState::Running;
            }
            currentAnim = 0;
            break;
        case PlayerState::Running:
            if (jumpPressed && this->isGrounded())
            {
                state = PlayerState::Jumping;
                velocity.y = jump_power;
                this->setGrounded(false);
            }
            else if (dirInput == 0 && std::abs(velocity.x) < 5.0f)
            {
                state = PlayerState::Idle;
            }
            else if (velocity.x * direction < 0 && this->isGrounded())
            {
                currentAnim = 3;
            }
            else
            {
                currentAnim = 1;
            }

            break;
        case PlayerState::Jumping:
            if (this->isGrounded())
            {
                if (dirInput != 0)
                    state = PlayerState::Running;
                else
                    state = PlayerState::Idle;
            }
            currentAnim = 2;
            break;
    }

    if (dirInput != 0)
        direction = dirInput;
    // gravity
    if (!grounded)
        velocity.y = velocity.y + (gravity * deltaTime);

    velocity.x = velocity.x + (dirInput * acceleration.x * deltaTime);

    this->grounded = false;
    if (std::abs(velocity.x) > maxSpeedX)
    {
        velocity.x = (velocity.x > 0 ? 1.0f : -1.0f) * maxSpeedX;
    }

    if (dirInput == 0 && velocity.x != 0)
    {
        const float factor = velocity.x > 0 ? -1.5f : 1.5f;
        float amount = factor * acceleration.x * deltaTime;

        // If the friction amount is larger than remaining speed, snap to 0
        if (std::abs(velocity.x) < std::abs(amount))
        {
            velocity.x = 0;
        }
        else
        {
            velocity.x += amount;
        }
    }
    if (!animations.empty())
    {
        animations[currentAnim].step(deltaTime);
    }

    GameObject::update(deltaTime, keys);
}

void Player::Render(SDL_Renderer* renderer, glm::vec2 offset)
{
    if (animations.empty())
        return;

    SDL_FRect src = animations[currentAnim].GetCurrentFrameSrc();

    SDL_FRect dst = {position.x + offset.x, position.y + offset.y, src.w, src.h};

    SDL_FlipMode flip = (direction == -1) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
    SDL_RenderTextureRotated(renderer, texture, &src, &dst, 0, nullptr, flip);
}
