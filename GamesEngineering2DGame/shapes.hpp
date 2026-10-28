#pragma once


// -----------------------------------------------------------

class Shape
{
public:
    Shape() = default;
    Shape(const Shape&) = default;
    Shape& operator=(const Shape&) = default;

    virtual float getArea() const = 0;
    virtual float getPerimeter() const = 0;
    virtual bool isInside(float x, float y) const = 0;

    virtual ~Shape() = default;

private:
};

// -----------------------------------------------------------

class Circle : public Shape
{
public:
    Circle(float radius) : radius_(radius) {}
    float getRadius() const { return radius_; }

    float getArea() const override { return 3.14159f * radius_ * radius_; }
    float getPerimeter() const override { return (2.0f * 3.14159f) * radius_; }
    bool isInside(float x, float y) const override { return (x * x + y * y) <= (radius_ * radius_); }

private:
    float radius_;
};

class Square : public Shape
{
public:
    Square(float side) : side_(side) {}
    float getSide() const { return side_; }

    float getArea() const override { return side_ * side_; }
    float getPerimeter() const override { return 4.0f * side_; }
    bool isInside(float x, float y) const override { return x >= 0 && x <= side_ && y >= 0 && y <= side_; }

private:
    float side_;
};

class Rect : public Shape
{
public:
    Rect(float width, float height) : width_(width), height_(height) {}
    float getWidth() const { return width_; }
    float getHeight() const { return height_; }

    float getArea() const override { return width_ * height_; }
    float getPerimeter() const override { return 2.0f * (width_ + height_); }
    bool isInside(float x, float y) const override { return x >= 0 && x <= width_ && y >= 0 && y <= height_; }


private:
    float width_;
    float height_;
};

// -----------------------------------------------------------
