#pragma once
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>

#include <glm/ext/vector_float2.hpp>
#include <glm/glm.hpp>

class GameObject
{
   public:
    glm::vec2 position{0.0f};
    glm::vec2 velocity{0.0f};

    SDL_FRect collider{0, 0, 32, 32};
    SDL_Texture* texture = nullptr;
    bool dynamic = false;

    enum class Tag
    {
        player,
        level,
        enemy,
        bullet
    } tag = Tag::level;
    virtual ~GameObject() = default;

    virtual void update(float deltaTime, const bool* keys)
    {
        if (dynamic)
        {
            position = position + (velocity * deltaTime);
        }
    }

    virtual void Render(SDL_Renderer* renderer, glm::vec2 offset)
    {
        if (!texture)
            return;

        SDL_FRect dst = {position.x + offset.x, position.y + offset.y, collider.w, collider.h};
        SDL_RenderTexture(renderer, texture, nullptr, &dst);
    }
};
