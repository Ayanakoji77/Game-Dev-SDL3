#pragma once

#include <SDL3/SDL_render.h>

#include <vector>

#include "core/animation.h"
#include "game/gameobject.h"
enum class BulletState
{
    Moving,
    Colliding,
    Inactive,
};
class Bullet : public GameObject
{
   private:
    BulletState state;
    std::vector<Animation> animations;
    int currentAnim = 0;
    float direction = 0;
    float yVariance = 30.0f;
    float bullet_velocity = 120.0f;

   public:
    Bullet(SDL_Texture* atlasTexture, glm::vec2 position, float direction);
    void update(float deltaTime, const bool* keys) override;
    void Render(SDL_Renderer* renderer, glm::vec2 offset) override;
    BulletState GetState() { return state; }
    void SetState(BulletState c_state) { state = c_state; }
    void reset(glm::vec2 pos, float dir);
};
