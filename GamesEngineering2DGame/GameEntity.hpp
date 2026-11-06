#pragma once

#include "Entity.hpp"
#include "Renderer.hpp"
#include "Vector.hpp"


class SpriteEntity : public VisualEntity {

public:
	SpriteEntity(const Vec2f& position, const GamesEngineeringBase::Image* image)
		: VisualEntity(position), image_(image)
	{

	}
	virtual void Update(float delta_time) override
	{
		// a plain sprite doesn't do anything on its own
	}

	virtual void Draw(GamesEngineeringBase::Window& canvas, const Camera& camera) override
	{
		if (image_ == nullptr)
		{
			return;
		}
		Vec2f screen = camera.toScreen(GetPosition()).convertTo<float>();
		Renderer_draw_image(canvas, screen, *image_);
	}

	int GetImageWidth() const
	{
		if (image_ != nullptr)
		{
			return static_cast<int>(image_->width);
		}
	}

	int GetImageHeight() const
	{
		if (image_ != nullptr)
		{
			return static_cast<int>(image_->height);
		}
	}

	// midpoint of the player image in world position
	Vec2f GetCentre() const
	{
		return { GetPosition().x_ + GetImageWidth() / 2 , GetPosition().y_ + GetImageHeight() / 2 };
	}
protected:
	const GamesEngineeringBase::Image* image_;

};