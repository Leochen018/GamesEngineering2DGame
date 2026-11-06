#pragma once

#include "MovingEntity.hpp"

// ------------------------------------------------------------
// An enemy that walks towards a target (the player).
// Now derives from MovingEntity, so it shares the player's
// wall-collision code instead of having its own copy.
// ------------------------------------------------------------

class EnemyEntity : public MovingEntity {
public:
    EnemyEntity(const Vec2f& position, const GamesEngineeringBase::Image* image,
                TileMap* map, float speed = 100.0f)
        : MovingEntity(position, image, map, speed)
    {
    }

    void SetTarget(const Vec2f& target) { target_ = target; }

    virtual void Update(float delta_time) override
    {
        // direction from enemy centre to the target
        Vec2f toTarget = target_ - GetCentre();
        float distance = toTarget.length();
        if (distance < stopDistance_) return;

        Vec2f direction = toTarget * (1.0f / distance);   // normalise
        MoveWithCollision(direction * (speed_ * delta_time));
    }

protected:
    Vec2f target_{ 0.0f, 0.0f };
    float stopDistance_ = 40.0f;   // stop this close to the target's centre
};
