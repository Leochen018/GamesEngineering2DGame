#pragma once
#include <cmath>


template <typename T>
struct Vec2
{
    T x_;
    T y_;

    Vec2 operator+(const Vec2& other) const
    {
        return { x_ + other.x_, y_ + other.y_ };
    }

    Vec2 operator*(T scalar) const
    {
        return { x_ * scalar, y_ * scalar };
    }

    Vec2 operator-(const Vec2& other) const
    {
        return { x_ - other.x_, y_ - other.y_ };
    }

    float length() const
    {
        return sqrtf((x_ * x_) + (y_ * y_));
    }

    // convert to another type. ie Vec2 float to int
    template<typename U>
    Vec2<U> convertTo() const
    {
        return { static_cast<U>(x_), static_cast<U>(y_) };
    }

   
};
// type allias for shorter names, easy to read
using Vec2f = Vec2<float>;
using Vec2i = Vec2<int>;
using Vec2d = Vec2<double>;