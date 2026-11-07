#pragma once


#include <cmath>
#include <iostream>
#include "TileMap.hpp"
#include "GameEntity.hpp"


const int MAX_HEALTH = 10;
class Player : public SpriteEntity
{
public:
	Player(const Vec2f& position, const GamesEngineeringBase::Image* image, TileMap* map, float speed = 300.0f)
		: SpriteEntity(position, image), map_(map), speed_(speed)
	{
		
	}
	void SetInputAxis(const Vec2f& input)
	{
		inputAxis_ = input;
	}
	float GetSpeed() const
	{
		return speed_;
	}

	void SetObstacles(SpriteEntity** obstacles, int count)
	{
		obstacles_ = obstacles;
		obstacle_count = count;
	}
	bool CanShoot() const
	{
		if (shootTimer_ <= 0.0f)
		{
			return true;
		}
		return false;
	}
	void resetShootTimer()
	{
		shootTimer_ = shootCoolDown_;
	}
	void TakeDamage(int amount)
	{
		health_ -= amount;
		if (health_ < 0)
		{
			health_ = 0;
		}
	}
	void Heal(int amount)
	{
		health_ += amount;
		if (health_ > MAX_HEALTH)
		{
			health_ = MAX_HEALTH;
		}
	}
	void CollectPickups(PickUpEntity* pickups, int count)
	{
		Vec2f pos = GetPosition();
		int w = GetImageWidth();
		int h = GetImageHeight();
		for (int i = 0; i < count; i++)
		{
			PickUpEntity& item = pickups[i];
			if (!item.IsActive()) continue;
			if (!item.OverlapsBox(pos.x_, pos.y_, w, h)) continue;

			switch (item.GetType())
			{
			case PickUpType::Health:
				if (!IsFullHealth())
				{
					Heal(item.GetAmount());
					item.Collect();
					std::cout << "Healed! Health: " << GetHealth() << "\n";

				}
				break;
			case PickUpType::Special:
				// later: AddSpecial(); item.Collect();
				break;
			}
		}
	}

	virtual void Update(float delta_time) override
	{
		int horizontalInput = static_cast<int>(inputAxis_.x_);
		int verticalInput = static_cast<int>(inputAxis_.y_);

		if (shootTimer_ > 0.0f)
		{
			shootTimer_ -= delta_time;
		}
		// check if key was pressed
		
		if (horizontalInput == 0 && verticalInput == 0)
		{
			return;
		}

		float distance = speed_ * delta_time;


		//normalise only for diagonal, straight movement is already normalised
		if (horizontalInput != 0 && verticalInput != 0)
		{
			distance *= 0.7071f;
		}

		unsigned int move_amount = static_cast<unsigned int>(std::max(distance, 1.0f));

		int w = GetImageWidth();
		int h = GetImageHeight();

		//get player world position, top left corner of the sprite, inherited from PositionEntity in Entity.hpp
		Vec2f pos = GetPosition();

		for (unsigned int i = 0; i < move_amount; i++)
		{
			if (horizontalInput != 0 && !map_->isAreaBlocked(static_cast<int>(pos.x_) + horizontalInput,
				static_cast<int>(pos.y_), w, h) && !IsBlockedByObstacle(pos.x_ + horizontalInput, pos.y_, w, h))
			{
				pos.x_ += horizontalInput;
			}
			if (verticalInput != 0 && !map_->isAreaBlocked(static_cast<int>(pos.x_), 
				static_cast<int>(pos.y_ + verticalInput), w, h) && !IsBlockedByObstacle(pos.x_, pos.y_ + verticalInput, w, h))
			{
				pos.y_ += verticalInput;
			}
		}
		SetPosition(pos);
	}


	bool IsAlive() const { return health_ > 0; }
	bool IsFullHealth() const { return health_ >= MAX_HEALTH; }
	int  GetHealth() const { return health_; }
	int  GetMaxHealth() const { return MAX_HEALTH; }
private:

	TileMap* map_;
	float speed_;
	Vec2f inputAxis_{ 0.0f, 0.0f };
	int obstacle_count = 0;
	SpriteEntity** obstacles_ = nullptr;


	float shootCoolDown_ = 0.2f;
	float shootTimer_ = 0.0f;

	
	int health_ = 4;

	bool IsBlockedByObstacle(float x, float y, int w, int h) const
	{
		Vec2f pos = GetPosition();
		for (int i = 0; i < obstacle_count; i++)
		{
			SpriteEntity* o = obstacles_[i];
			if (o == nullptr) continue;

			// check if its overlapping where the object stands. ignores it so the sprite can walk out
			if (o->OverlapsBox(pos.x_, pos.y_, w, h)) continue;

			if (o->OverlapsBox(x, y, w, h)) return true;

		}
		return false;
	}

};