#pragma once

#include <random>
#include "GamesEngineeringBase.hpp"
#include "Player.hpp"
#include "TileMap.hpp"
#include "Enemy.hpp"
#include "GameStatistics.hpp"
#include "GameEntity.hpp"
#include "Camera.hpp"
#include "Projectile.hpp"

//Manager class handles everything that related to one play through
//when game starts, main creates manager object and set up constructors
//links player and enmemy to shared stats, spawns the starting enemies

class Manager
{
public:

	static const int MAX_PICKUPS = 20; // maximum potions slots for each game
	static const int HEAL_AMOUNT = 5; // how much the health potion heals the player
	static const int MANA_AMOUNT = 15;
	
	static const int HEAL_POTION_CHANCE = 20; //20 percent chance of heal potion to drop when enemy is killed
	static const int MANA_POTION_CHANCE = 20; // 20 percent chance of mana potion to drop when enemy is killed

	Manager(GamesEngineeringBase::Window& canvas, TileMap& map,
		std::mt19937& gen,
		const GamesEngineeringBase::Image& playerImage,
		const GamesEngineeringBase::Image& enemyImage,
		const GamesEngineeringBase::Image& projectileImage,
		const GamesEngineeringBase::Image& healthPotionImage,
		const GamesEngineeringBase::Image& manaPotionImage,
		const GamesEngineeringBase::Image& keyImage)
		: camera_(canvas.getWidth(), canvas.getHeight(), map.widthPixels(), map.heightPixels())
		, player_({ 300.0f, 400.0f }, & playerImage, & map, 700.0f, & stats_)
		, enemies_(&map, &enemyImage, &projectileImage, &player_, &stats_)
		, healthPotionImage_(&healthPotionImage)
		, manaPotionImage_(&manaPotionImage)
	{
		camera_.SetBounded(!map.isInfiniteWorld());
		player_.SetObstacles(enemies_.GetObstacles(), enemies_.GetMaxEnemies());	
		enemies_.SpawnRandom(10, gen);


		if (!map.isInfiniteWorld())
		{
			pickups_[4].Place({ 1900.0f, 300.0f }, &keyImage, ItemType::Key, 0);
		}
	}
	void SpawnPotion(ItemType type, const Vec2f& centre)
	{
	
		int amount = 0;

		const GamesEngineeringBase::Image* image = nullptr;
		if (type == ItemType::Health)
		{ 
			image = healthPotionImage_;
			amount = HEAL_AMOUNT;
		}
		else if (type == ItemType::Mana)
		{
			image = manaPotionImage_;
			amount = MANA_AMOUNT;
		}
		for (int i = 0; i < MAX_PICKUPS; i++)
		{
			if (pickups_[i].IsActive()) continue;

			// gets the top left corner of the dead enemy position
			Vec2f topLeft = { (centre.x_ - (image->width / 2.0f)), (centre.y_ - (image->height / 2.0f)) };
			pickups_[i].Place(topLeft, image, type, amount);
			return;
		}

	}

	void DropPotion(std::mt19937& gen)
	{
		std::uniform_int_distribution<int> distribution (0,99);
		for (int i = 0; i < enemies_.GetDeathCount(); i++)
		{
			int random = distribution(gen);
			const Vec2f& enemPos = enemies_.GetDeathPosition(i);
			if (random < HEAL_POTION_CHANCE)
			{
				SpawnPotion(ItemType::Health, enemPos);
			}
			else if (random < HEAL_POTION_CHANCE + MANA_POTION_CHANCE)
			{
				SpawnPotion(ItemType::Mana, enemPos);
			}
		}

	}

	
	// stats_ is used by player and enemy 
	// player is used by enemy
	GameStatistics stats_;
	Camera camera_;
	Player player_;
	EnemyManager enemies_;
	const GamesEngineeringBase::Image* healthPotionImage_ = nullptr;
	const GamesEngineeringBase::Image* manaPotionImage_ = nullptr;
	ProjectileManager projectiles_;
	PickUpEntity pickups_[MAX_PICKUPS];
	



};