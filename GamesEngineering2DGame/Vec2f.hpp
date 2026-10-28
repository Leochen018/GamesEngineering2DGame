#pragma once

// -------------------------------------------------------------------------

struct Vec2f
{
    float x_;
    float y_;

    Vec2f operator+(const Vec2f& other) const
    {
        return { x_ + other.x_, y_ + other.y_ };
    }

    Vec2f operator*(float scalar) const
    {
        return { x_ * scalar, y_ * scalar };
    }


};

// -------------------------------------------------------------------------