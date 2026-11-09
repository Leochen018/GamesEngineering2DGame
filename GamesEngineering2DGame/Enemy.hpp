#pragma once

#include "GameEntity.hpp"
#include "TileMap.hpp"
#include "Projectile.hpp"
#include "GameStatistics.hpp"
#include <random>

class EnemyEntity : public CharacterEntity
{
public:
	EnemyEntity(const Vec2f& position, const GamesEngineeringBase::Image* image, TileMap* map)
		: CharacterEntity(position, image,6), map_(map)
	{

	}

	void SetTarget(const Vec2f& target)
	{
		target_ = target;
	}
	
	void SetBlocker(const SpriteEntity* blocker)
	{
		blocker_ = blocker;
	}

	bool CanShoot() const
	{
		if (shootTimer_ <= 0.0f)
		{
			return true;
		}
		return false;
	}
	void SetShootTimer(float seconds)
	{
		shootTimer_ = seconds;
	}
	void ResetShootTimer()
	{
		shootTimer_ = shootCoolDown_;
	}
	virtual void Update(float delta_time) override
	{
		if (shootTimer_ > 0.0f)
		{
			shootTimer_ -= delta_time;
		}
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

		if ((map_ == nullptr || !map_->isAreaBlocked(static_cast<int>(pos.x_ + movement.x_), static_cast<int>(pos.y_),w,h))
			&& !(blocker_ != nullptr && blocker_->OverlapsBox(pos.x_ + movement.x_, pos.y_, w, h)))
		{
			pos.x_ += movement.x_;
		}
		if ((map_ == nullptr || !map_->isAreaBlocked(static_cast<int>(pos.x_), static_cast<int>(pos.y_ + movement.y_), w, h))
			&& !(blocker_ != nullptr && blocker_->OverlapsBox(pos.x_, pos.y_ + movement.y_, w, h)))
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
	float shootCoolDown_ = 2.0f;
	float shootTimer_ = 0.0f;
private:
	const SpriteEntity* blocker_ = nullptr;
};

class EnemyManager
{
public:
	static const int MAX_ENEMIES = 30;

	EnemyManager(TileMap* map, const GamesEngineeringBase::Image* image, const GamesEngineeringBase::Image* projectileImage,
		const SpriteEntity* player, GameStatistics* stats)
		: map_(map), image_(image), projectileImage_(projectileImage), player_(player), stats_(stats)
	{
	
	}
	~EnemyManager()
	{
		for (auto* enemy : enemies_) {
			delete enemy;
		}
	}
	EnemyManager(const EnemyManager&) = delete;
	EnemyManager& operator=(const EnemyManager&) = delete;


	// count is the amount of enemies spawned when passed into the function
	void SpawnRandom(int count, std::mt19937& gen)
	{
		for (int i = 0; i < count; i++)
		{
			Vec2f spot;
			if (map_->FindRandomSpot(gen, image_->width, image_->height, player_->GetCentre(), minSpawnDistance_, spot))
			{
				for (int i = 0; i < MAX_ENEMIES; i++)
				{
					if (enemies_[i] == nullptr)
					{
						EnemyEntity* enemy = new EnemyEntity(spot, image_, map_);
						enemy->SetBlocker(player_);
						obstacles_[i] = enemy;
						enemies_[i] = enemy;
						break;
							
					}
				}
				
			}
		}
	}
	void Shoot(ProjectileManager& projectiles)
	{
		for (int i = 0; i < MAX_ENEMIES; i++)
		{
			EnemyEntity* enemy = enemies_[i];
			if (enemy == nullptr || !enemy->CanShoot())
			{
				continue;
			}
			Vec2f playerDirection = player_->GetCentre() - enemy->GetCentre();
			float distance = playerDirection.length();
			if (distance > projectileRange_ || distance == 0.0f)
			{
				continue;
			}
			//normalise
			Vec2f direction = playerDirection * (1.0f / distance); 
			projectiles.SpawnProjectile(enemy->GetCentre(), direction, projectileSpeed_,
				projectileImage_, map_, false, projectileDamage_);
			enemy->ResetShootTimer();

		}

	}

	void Update(float dt, std::mt19937& gen)
	{
		for (int i = 0; i < MAX_ENEMIES; i++)
		{
			if (enemies_[i] == nullptr) continue;
			enemies_[i]->SetTarget(player_->GetCentre());
			enemies_[i]->Update(dt);

		}
		spawnTimer_ += dt;

		if (spawnTimer_ >= spawnInterval_)
		{
			spawnTimer_ = 0;
			SpawnRandom(1, gen);
		}
	}
	// players shoot projectiles, damages enemies, delete the dead enemies
	void ResolveHits(ProjectileManager& projectiles)
	{
		deathCount_ = 0;
		for (int i = 0; i < projectiles.Count(); i++)
		{
			ProjectileEntity& p = projectiles.Get(i);
			if (!p.IsFromPlayer() || !p.IsAlive()) continue;

			Vec2f q = p.GetPosition();
			for (int j = 0; j < MAX_ENEMIES; j++)
			{
				if (enemies_[j] != nullptr && enemies_[j]->OverlapsBox(q.x_, q.y_, p.GetImageWidth(), p.GetImageHeight()))	
				{
					enemies_[j]->TakeDamage(p.GetDamage());
					p.Disable();
					break;
				}
			}
		
		}
		for (int i = 0; i < MAX_ENEMIES; i++)
		{
			if (enemies_[i] != nullptr && !enemies_[i]->IsAlive())
			{
				stats_->kills++;
				stats_->score += enemyKilledScore_;
				num_enemies_deaths_[deathCount_] = enemies_[i]->GetCentre();
				deathCount_++;
				delete enemies_[i];
				enemies_[i] = nullptr;
				obstacles_[i] = nullptr;

			}
		}
		

	}

	//kills x amount of enemies when activate mana
	int KillFromList(int num)
	{
		int count = 0;

		for (int i = 0; i < MAX_ENEMIES; i++)
		{
			if (enemies_[i] == nullptr) continue;

			enemies_[i]->TakeDamage(enemies_[i]->GetHealth());
			count++;
			if (count == num)
			{
				break; // killed x amount of enemies
			}
		}
		return count;
	}
	void Draw(GamesEngineeringBase::Window& canvas, const Camera& camera)
	{
		for (int i = 0; i < MAX_ENEMIES; i++)
		{
			if (enemies_[i] != nullptr)
			{
				enemies_[i]->Draw(canvas, camera);
			}
		}
	}

	SpriteEntity** GetObstacles()
	{
		return obstacles_;
	}
	int GetMaxEnemies() 
	{
		return MAX_ENEMIES;
	}
	int GetDeathCount() const { return deathCount_; }
	const Vec2f& GetDeathPosition(int index) const { return num_enemies_deaths_[index]; }
private:


	EnemyEntity* enemies_[MAX_ENEMIES] = {};
	SpriteEntity* obstacles_[MAX_ENEMIES] = {};

	TileMap* map_;
	const GamesEngineeringBase::Image* image_; // enemy image
	const GamesEngineeringBase::Image* projectileImage_;
	const SpriteEntity* player_;

	float projectileRange_ = 500.0f;    // only shoot when the player is this close
	float projectileSpeed_ = 350.0f;
	int projectileDamage_ = 1;

	float spawnTimer_ = 0.0f;
	float spawnInterval_ = 10.0f;      //  spawns enemies every 10 seconds
	float minSpawnDistance_ = 600.0f; // never spawn this close to the player

	GameStatistics* stats_;
	int enemyKilledScore_ = 50;

	Vec2f num_enemies_deaths_[MAX_ENEMIES];
	int deathCount_ = 0;
};