#pragma once

#include "GamesEngineeringBase.hpp"
#include "Vector.hpp"
#include "Camera.hpp"

class Entity {

public:
    Entity() = default;
    virtual ~Entity() = default;   // needed because we use Entity through base pointers

    virtual void Update(float delta_time) = 0;

};

class PositionEntity : public Entity {
public:
    PositionEntity(const Vec2f& position)
        : Entity()
        , position_{ position }
    {
    }

    const Vec2f& GetPosition() const { return position_; }
    void SetPosition(const Vec2f& position) { position_ = position; }
    void Move(const Vec2f& delta) { position_ = position_ + delta; }

private:
    Vec2f position_;
};


class VisualEntity : public PositionEntity {
public:
    VisualEntity(const Vec2f& position)
        : PositionEntity(position)
    {
    }

    virtual void Update(float delta_time) override = 0;
    virtual void Draw(GamesEngineeringBase::Window& window, const Camera& camera) = 0;
};
