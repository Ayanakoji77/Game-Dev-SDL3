#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_rect.h>

#include <glm/fwd.hpp>
#include <glm/glm.hpp>

class Camera
{
   private:
    SDL_FRect viewport;
    int mapW, mapH;

   public:
    Camera(int screenHeignt, int screenWidth, int mapWidth, int mapHeight)
        : viewport{0, 0, float(screenHeignt), float(screenWidth)}, mapW(mapWidth), mapH(mapHeight)
    {
    }

    void Follow(glm::vec2 target_position)
    {
        viewport.x = target_position.x - (viewport.w / 2);
        viewport.y = target_position.y - (viewport.h / 2);
        if (mapH > 0 && mapW > 0)
        {
            if (viewport.x < 0)
                viewport.x = 0;
            if (viewport.y < 0)
                viewport.y = 0;
            if (viewport.x + viewport.w > mapW)
                viewport.x = mapW - viewport.w;
            if (viewport.y + viewport.h > mapH)
                viewport.y = mapH - viewport.h;
        }
    }
    glm::vec2 GetOffset() const { return {-viewport.x, -viewport.y}; }
};
