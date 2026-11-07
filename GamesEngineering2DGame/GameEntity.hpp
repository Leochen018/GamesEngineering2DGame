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
		return 0;
	}

	int GetImageHeight() const
	{
		if (image_ != nullptr)
		{
			return static_cast<int>(image_->height);
		}
		return 0;
	}

	// x and y are the sprite world position
	//w and h are the sprite image size
	// detect collision by checking if a box overlap the sprite body 
	bool OverlapsBox(float x, float y, int w, int h) const
	{
		Vec2f p = GetPosition();
		if ((x < p.x_ + GetImageWidth()) && (x + w > p.x_) && (y < p.y_ + GetImageHeight()) && (y + h > p.y_))
		{
			return true;
		}
		return false;
	}

	// midpoint of the player image in world position
	Vec2f GetCentre() const
	{
		return { GetPosition().x_ + GetImageWidth() / 2 , GetPosition().y_ + GetImageHeight() / 2 };
	}
protected:
	const GamesEngineeringBase::Image* image_;

};

enum class PickUpType
{
	Health, 
	Special
};
class PickUpEntity : public SpriteEntity
{
public:
	PickUpEntity() : SpriteEntity({0.0f, 0.0f}, nullptr)
	{
	}

	void Place(const Vec2f& position, const GamesEngineeringBase::Image* image,
		PickUpType type, int amount)
	{
		SetPosition(position);
		image_ = image;
		type_ = type;
		amount_ = amount;
		active_ = true;
	}

	void Draw(GamesEngineeringBase::Window& canvas, const Camera& camera) override
	{
		if (active_)
		{
			SpriteEntity::Draw(canvas, camera);
		}
	}


	bool IsActive() const { return active_; }
	void Collect() { active_ = false; }
	PickUpType GetType() const { return type_; }
	int GetAmount() const { return amount_; }
private:
	PickUpType type_ = PickUpType::Health;
	int amount_ = 0;
	bool active_ = false;
};