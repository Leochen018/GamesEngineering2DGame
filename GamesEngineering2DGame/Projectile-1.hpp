#pragma once

#include "GameEntity.hpp"
#include "TileMap.hpp"

// ------------------------------------------------------------
// A projectile: aimed ONCE when fired, then flies in a straight line.
// Dies when it hits a wall, leaves the world, hits something,
// or its lifetime runs out.
//
// Kept in a fixed array (a "pool") in main and reused - no STL,
// no new/delete during play. alive_ says whether a slot is in use.
// ------------------------------------------------------------

class ProjectileEntity : public SpriteEntity
{
public:
	ProjectileEntity() : SpriteEntity({ 0.0f, 0.0f }, nullptr)
	{
	}

	// centre:    where to start (usually the shooter's centre), world pixels
	// direction: which way to fly - must already be length 1
	// speed:     pixels per second
	// piercing:  true = keeps going after hitting an enemy (for the special later)
	void Fire(const Vec2f& centre, const Vec2f& direction, float speed,
		const GamesEngineeringBase::Image* image, TileMap* map,
		bool fromPlayer, int damage, float lifetime = 2.0f, bool piercing = false)
	{
		image_ = image;
		map_ = map;

		// centre the bullet image on the start point
		SetPosition({ centre.x_ - GetImageWidth() / 2.0f, centre.y_ - GetImageHeight() / 2.0f });

		velocity_ = direction * speed;
		fromPlayer_ = fromPlayer;
		damage_ = damage;
		lifetime_ = lifetime;
		piercing_ = piercing;
		alive_ = true;
	}

	virtual void Update(float delta_time) override
	{
		if (!alive_)
		{
			return;
		}

		// fly in a straight line (Move comes from PositionEntity)
		Move(velocity_ * delta_time);

		// run out of time?
		lifetime_ -= delta_time;
		if (lifetime_ <= 0.0f)
		{
			alive_ = false;
			return;
		}

		// hit a wall or left the world? (isAreaBlocked is true outside the map)
		Vec2f p = GetPosition();
		if (map_ != nullptr &&
			map_->isAreaBlocked(static_cast<int>(p.x_), static_cast<int>(p.y_), GetImageWidth(), GetImageHeight()))
		{
			alive_ = false;
		}
	}

	virtual void Draw(GamesEngineeringBase::Window& canvas, const Camera& camera) override
	{
		if (alive_)
		{
			SpriteEntity::Draw(canvas, camera);
		}
	}

	// called by main when this projectile hits a target
	void OnHit()
	{
		if (!piercing_)
		{
			alive_ = false;
		}
	}

	bool IsAlive() const { return alive_; }
	void Kill() { alive_ = false; }
	bool IsFromPlayer() const { return fromPlayer_; }
	int GetDamage() const { return damage_; }

private:
	TileMap* map_ = nullptr;
	Vec2f velocity_{ 0.0f, 0.0f };   // direction * speed, fixed when fired
	float lifetime_ = 0.0f;          // seconds left
	bool alive_ = false;             // is this pool slot in use?
	bool fromPlayer_ = true;         // player bullets hit enemies, enemy bullets hit the player
	bool piercing_ = false;          // special shots can pass through enemies
	int damage_ = 1;
};

// ------------------------------------------------------------
// Fire a projectile from the first free slot in the pool.
// Returns false if every slot is busy (that shot is skipped).
// ------------------------------------------------------------
inline bool SpawnProjectile(ProjectileEntity* pool, int count,
	const Vec2f& centre, const Vec2f& direction, float speed,
	const GamesEngineeringBase::Image* image, TileMap* map,
	bool fromPlayer, int damage, float lifetime = 2.0f, bool piercing = false)
{
	for (int i = 0; i < count; i++)
	{
		if (!pool[i].IsAlive())
		{
			pool[i].Fire(centre, direction, speed, image, map, fromPlayer, damage, lifetime, piercing);
			return true;
		}
	}
	return false;
}
