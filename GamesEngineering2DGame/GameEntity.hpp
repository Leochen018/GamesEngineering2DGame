#pragma once

#include "Entity.hpp"
#include "Renderer.hpp"
#include "Vector.hpp"
#include <string>


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

enum class ItemType
{
	Health, 
	Mana,
	Key
	
};

class PickUpEntity : public SpriteEntity
{
public:
	PickUpEntity() : SpriteEntity({0.0f, 0.0f}, nullptr)
	{
	}

	void Place(const Vec2f& position, const GamesEngineeringBase::Image* image,
		ItemType type, int amount)
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
	ItemType GetType() const { return type_; }
	int GetAmount() const { return amount_; }
private:
	ItemType type_ = ItemType::Health;
	int amount_ = 0;
	bool active_ = false;
};


enum class  AttributeBarType
{
	Health_Bar,
	Mana_Bar
};
// inherit VisualEntity since it doesnt have collision
class AttributeBar : public VisualEntity
{
public:
	static const int NUM_IMAGES = 11; //health_0.png .. health_10.png

	//attribute bar follows the SpriteEntity (player or enemy)
	// gap is the distance between the sprite image and the attribute bar. placed below the sprite
	AttributeBar(const SpriteEntity* target, int maxValue, AttributeBarType type, float gap )
		:VisualEntity({ 0.0f,0.0f }), target_(target), maxValue_(maxValue), gap_(gap)
	{
		if (type == AttributeBarType::Health_Bar)
		{
			images_ =   healthImages_;

		}
		else if (type == AttributeBarType::Mana_Bar)
		{
			images_ = manaImages_;
		}
	}
	void SetValue(int value)
	{
		value_ = value;
	}
	static bool LoadImages()
	{
		for (int i = 0; i < NUM_IMAGES; i++)
		{
			healthImages_[i].load("Resources/health_bar_" + std::to_string(i) + ".png");
			manaImages_[i].load("Resources/mana_bar_" + std::to_string(i) + ".png");
			if (healthImages_[i].data == nullptr || manaImages_[i].data == nullptr)
			{
				return false;
			}
		}
		return true;
	}

	virtual void Update(float delta_time) override
	{
		if (target_ == nullptr)
		{
			return;
		}
		Vec2f targetPosition = target_->GetPosition();
		float barWidth = static_cast<float>(images_[0].width);
		// position of the health bar with a gap
		SetPosition({ targetPosition.x_ + ((target_->GetImageWidth() - barWidth) / 2.0f),
			targetPosition.y_ + target_->GetImageHeight() + gap_ });
	}

	virtual void Draw(GamesEngineeringBase::Window& canvas, const Camera& camera) override
	{
		if (maxValue_ <= 0)
		{
			return;
		}

		//gets the index of the health/mana bar array storing the health_bar.png based on the entities health and max health. same with mana
		//starts off full health, mana starts from empty
		int index = value_ * (NUM_IMAGES - 1) / maxValue_;

		// base case when index is out of range
		if (index < 0)
		{
			index = 0;
		}
		if (index > NUM_IMAGES - 1)
		{
			index = NUM_IMAGES - 1;
		}

		Renderer_draw_image(canvas, camera.toScreen(GetPosition()), images_[index]);

	}

private:
	// who the bar follows
	const SpriteEntity* target_;                         
	inline static GamesEngineeringBase::Image healthImages_ [NUM_IMAGES];
	inline static GamesEngineeringBase::Image manaImages_[NUM_IMAGES];

	//when called, will point to either health or mana iamges
	GamesEngineeringBase::Image* images_ = nullptr;

	int value_ = 0;
	int maxValue_ = 0;
	float gap_ = 4.0f;
};





class CharacterEntity : public SpriteEntity
{
public:
	CharacterEntity(const Vec2f& position, const GamesEngineeringBase::Image* image, int maxHealth)
		: SpriteEntity(position, image), maxHealth_(maxHealth), health_(maxHealth),
		healthBar_(this, maxHealth, AttributeBarType::Health_Bar, 4.0f)  // gap is 4 pixels
	{
		
	}

	virtual void TakeDamage(int amount)
	{
		health_ -= amount;
		if (health_ < 0)
		{
			health_ = 0;
		}
	}

	virtual void Draw(GamesEngineeringBase::Window& canvas, const Camera& camera) override
	{
		SpriteEntity::Draw(canvas, camera);
		healthBar_.SetValue(health_);
		healthBar_.Update(0.0f);
		healthBar_.Draw(canvas, camera);
	}



	bool IsAlive() const
	{
		return health_ > 0;
	}
	int  GetHealth() const
	{
		return health_;
	}
	int  GetMaxHealth() const
	{
		return maxHealth_;
	}
protected:
	int maxHealth_;
	int health_;

private:
	AttributeBar healthBar_;
};

