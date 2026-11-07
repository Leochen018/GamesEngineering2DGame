#pragma once
#include "GameEntity.hpp"
#include "TileMap.hpp"
class ProjectileEntity : public SpriteEntity
{
public:
	ProjectileEntity() : SpriteEntity({ 0.0f, 0.0f }, nullptr)
	{

	}

	void Fire(const Vec2f& centre, const Vec2f& direction, float speed, const GamesEngineeringBase::Image* image,
		TileMap* map, bool fromPlayer, int damage)
	{
		image_ = image;
		map_ = map;

		SetPosition({ centre.x_ - GetImageWidth() / 2.0f, centre.y_ - GetImageHeight() / 2.0f });
		velocity_ = direction * speed;  
		fromPlayer_ = fromPlayer;
		damage_ = damage;
		lifetime_ = 2.0f;                // projectile lasts for 2 seconds 
		alive_ = true;
	}
	void Draw(GamesEngineeringBase::Window& canvas, const Camera& camera) override {
		if (alive_) 
		{
			SpriteEntity::Draw(canvas, camera);
		}
	}


	virtual void Update(float delta_time) override
	{
		if (!alive_)
		{
			return;
		}
		Move(velocity_ * delta_time);
		lifetime_ -= delta_time;

		if (lifetime_ <= 0.0f)
		{
			alive_ = false;
			return;
		}
		Vec2f p = GetPosition();
		if (map_ != nullptr && map_->isAreaBlocked(static_cast<int>(p.x_),static_cast<int>(p.y_), GetImageWidth(),GetImageHeight()))
		{
			alive_ = false;
		}
	}

	bool IsAlive() const { return alive_; }
	void Kill() { alive_ = false; }
	bool IsFromPlayer() const { return fromPlayer_; }
	int  GetDamage() const { return damage_; }
private:
	TileMap* map_ = nullptr;
	Vec2f velocity_{ 0.0f, 0.0f };
	float lifetime_ = 0.0f;
	bool alive_ = false;
	bool fromPlayer_ = true;
	int damage_ = 1;
};

inline bool SpawnProjectile(ProjectileEntity* pool, int count, const Vec2f& centre, const Vec2f& direction,
	float speed, const GamesEngineeringBase::Image* image, TileMap* map, bool fromPlayer, int damage)
{
	for (int i = 0; i < count; i++)
	{
		if (!pool[i].IsAlive())
		{
			pool[i].Fire(centre, direction, speed, image, map, fromPlayer, damage);
			return true;
		}
	}
	return false;
}