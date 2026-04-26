#pragma once
#include <vector>

#include "core/animation.h"
#include "gameobject.h"

enum class EnemyState
{
    Patrolling,
    Dead
};

class Enemy : public GameObject
{
   private:
    EnemyState state = EnemyState::Patrolling;
    float walkSpeed = 40.0f;
    float direction = -1.0f;
    std::vector<Animation> animations;
    int currentAnim = 0;
    float gravity = 500.0f;
    bool grounded = false;

   public:
    Enemy(SDL_Texture* atlasTexture);
    void update(float deltaTime, const bool* keys) override;
    void Render(SDL_Renderer* renderer, glm::vec2 offset) override;
    EnemyState getState() { return state; }
    void setGrounded(bool val) { grounded = val; }
    void takeDamage();
    void reverseDirection();
};
