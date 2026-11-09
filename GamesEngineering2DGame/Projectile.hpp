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
	void Disable() { alive_ = false; }
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

class ProjectileManager
{
public:
	static const int MAX_AMOUNT_PROJECTILE = 200; // 200 is maximum amount of projectiles in the world at given time

	bool SpawnProjectile(const Vec2f& centre, const Vec2f& direction, float speed,
		const GamesEngineeringBase::Image* image, TileMap* map, bool fromPlayer, int damage)
	{
		for (int i = 0; i < MAX_AMOUNT_PROJECTILE; i++)
		{
			if (!projectiles_[i].IsAlive()) // checks for a free slot to fire a projectile
			{
				projectiles_[i].Fire(centre, direction, speed, image, map, fromPlayer, damage);
				return true;
			}
		}
		return false;
	}
	void Update(float dt)
	{
		for (int i = 0; i < MAX_AMOUNT_PROJECTILE; i++)
		{
			projectiles_[i].Update(dt);
		}
	}

	void Draw(GamesEngineeringBase::Window& canvas, const Camera& camera)
	{
		for (int i = 0; i < MAX_AMOUNT_PROJECTILE; i++)
		{
			projectiles_[i].Draw(canvas, camera);
		}
	}


	int Count() const { return MAX_AMOUNT_PROJECTILE; }
	ProjectileEntity& Get(int i) { return projectiles_[i]; }
private:
	ProjectileEntity projectiles_[MAX_AMOUNT_PROJECTILE];
};