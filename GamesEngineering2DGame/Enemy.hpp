#pragma once

#include "GameEntity.hpp"
#include "TileMap.hpp"

class EnemyEntity : public SpriteEntity
{
public:
	EnemyEntity(const Vec2f& position, const GamesEngineeringBase::Image* image, TileMap* map)
		: SpriteEntity(position, image), map_(map)
	{

	}

	void SetTarget(const Vec2f& target)
	{
		target_ = target;
	}
	
	virtual void Update(float delta_time) override
	{
		// get the direction from enemy centre to the player. target - origin position
		Vec2f targetDirection = target_ - GetCentre();
		float distance = targetDirection.length();
		if (distance < stopDistance_)
		{
			return;
		}
		//normalise
		Vec2f direction = targetDirection * (1.0f / distance);

		Vec2f movement = direction * (speed_ * delta_time);

		int w = GetImageWidth();
		int h = GetImageHeight();
		Vec2f pos = GetPosition();

		if (map_ == nullptr || !map_->isAreaBlocked(static_cast<int>(pos.x_ + movement.x_), static_cast<int>(pos.y_),w,h))
		{
			pos.x_ += movement.x_;
		}
		if (map_ == nullptr || !map_->isAreaBlocked(static_cast<int>(pos.x_), static_cast<int>(pos.y_ + movement.y_), w, h))
		{
			pos.y_ += movement.y_;
		}
		SetPosition(pos);
		


	}
	void SetSpeed(float speed)
	{
		speed_ = speed;
	}
protected:
	TileMap* map_;
	Vec2f target_ = {0.0f, 0.0f};
	float stopDistance_ = 40.0f;   // stop this close to the target's centre
	float speed_ = 200.0f;
};