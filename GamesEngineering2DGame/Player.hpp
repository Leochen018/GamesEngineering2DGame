#pragma once


#include <cmath>
#include <iostream>
#include "TileMap.hpp"
#include "GameEntity.hpp"
#include "Projectile.hpp"
#include "GameStatistics.hpp"


const int MAX_HEALTH = 15;
const int MAX_MANA = 20;
const int MANA_KILLS = 3; //kills 3 enemies when activate mana
const int MANA_COST = 10;

class Player : public CharacterEntity
{
public:
	Player(const Vec2f& position, const GamesEngineeringBase::Image* image, TileMap* map, float speed,GameStatistics* stats)
		: CharacterEntity(position, image, MAX_HEALTH), map_(map), speed_(speed), stats_(stats)
		, manaBar_(this, MAX_MANA, AttributeBarType::Mana_Bar, 20.0f)
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
		CharacterEntity::TakeDamage(amount);
		std::cout << "Hit! Health: " << health_ << "\n";
	}
	void AddHealth(int amount)
	{
		health_ += amount;
		if (health_ > MAX_HEALTH)
		{
			health_ = MAX_HEALTH;
		}
	}
	void AddMana(int amount)
	{
		mana_ += amount;
		if (mana_ > MAX_MANA)
		{
			mana_ = MAX_MANA;
		}
	}
	bool UseMana(int cost)
	{
		// base case: check if player have enough mana to activate 
		if (mana_ < cost)  
		{
			return false;
		}
		mana_ -= cost;
		return true;
	}

	// uses base class draw method and draw mana bar
	virtual void Draw(GamesEngineeringBase::Window& canvas, const Camera& camera) override
	{
		CharacterEntity::Draw(canvas, camera);   
		manaBar_.SetValue(mana_);
		manaBar_.Update(0.0f);                   
		manaBar_.Draw(canvas, camera);
	}


	bool IsFullMana() const { return mana_ >= MAX_MANA; }
	int GetMana() const { return mana_; }



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
			case ItemType::Health:
				if (!IsFullHealth())
				{
					AddHealth(item.GetAmount());
					item.Collect();
					stats_->healthPotionsUsed++;
					std::cout << "used health potion. Health: " << GetHealth() << "\n";

				}
				break;
			case ItemType::Mana:
				if (!IsFullMana())
				{
					AddMana(item.GetAmount());
					item.Collect();
					stats_->manaPotionsUsed++;
					std::cout << "used mana " << GetMana() << " times \n";
				}
				break;
			case ItemType::Key:
				hasKey_ = true;
				item.Collect();                  // key disappears from the map
				map_->setPassableTile(LOCKED_DOOR_TILE,false);
				std::cout << "Got the key\n";
				break;
			}
		}
	}

	void ApplyEnemyProjectile(ProjectileManager& projectiles)
	{
		
		for (int i = 0; i < projectiles.Count(); i++)
		{
			ProjectileEntity& proj = projectiles.Get(i);
			if (!proj.IsAlive() || proj.IsFromPlayer()) continue;   // only live ENEMY bullets

			Vec2f projPos = proj.GetPosition();
			if (OverlapsBox(projPos.x_, projPos.y_, proj.GetImageWidth(), proj.GetImageHeight()))
			{
				TakeDamage(proj.GetDamage());
				proj.Disable();                                        // bullet disappears on hit
				stats_->shotsTaken++;
				stats_->damageTaken+=proj.GetDamage();
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


	bool IsFullHealth() const { return health_ >= MAX_HEALTH; }
	bool HasKey() const { return hasKey_; }
	
	
private:

	TileMap* map_;
	float speed_;
	Vec2f inputAxis_{ 0.0f, 0.0f };
	int obstacle_count = 0;
	SpriteEntity** obstacles_ = nullptr;


	float shootCoolDown_ = 0.2f;
	float shootTimer_ = 0.0f;


	GameStatistics* stats_;

	int mana_ = 0;
	bool hasKey_ = false;
	AttributeBar manaBar_;


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