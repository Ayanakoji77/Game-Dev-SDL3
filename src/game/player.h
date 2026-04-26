#pragma once
#include <functional>
#include <glm/fwd.hpp>
#include <vector>

#include "core/animation.h"
#include "core/timer.h"
#include "gameobject.h"

enum class PlayerState
{
    Idle,
    Running,
    Jumping,
};

class Player : public GameObject
{
   private:
    PlayerState state = PlayerState::Idle;

    // constants for the player
    float maxSpeedX = 80.0f;
    float direction = 1.0f;
    glm::vec2 acceleration{250.0f, 0.0f};
    float jump_power = -300.0f;
    std::vector<Animation> animations;
    int currentAnim = 0;
    bool grounded = true;
    Timer weaponTimer{0.2f};
    float gravity = 500.0f;

   public:
    std::function<void(glm::vec2, float)> onShoot;
    Player(SDL_Texture* atlasTexture);
    PlayerState getState() { return state; }
    void update(float deltaTime, const bool* keys) override;
    void Render(SDL_Renderer* renderer, glm::vec2 offset) override;
    void setGrounded(bool val) { grounded = val; }
    bool isGrounded() { return grounded; }
};
