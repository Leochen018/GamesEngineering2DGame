#include <iostream>
#include "GamesEngineeringBase.hpp" // Include the GamesEngineeringBase header
#include "Camera.hpp"
#include "Vector.hpp" // Include the Vector header for addition and multiplication on 2D vectors
#include "Renderer.hpp"
#include "TileMap.hpp" // Include the world map background
#include "Enemy.hpp"
#include "Player.hpp"
#include "Projectile.hpp"
#include "GameStatistics.hpp"
#include "GameState.hpp"
#include "Manager.hpp"
#include <random>






int main() {

    
    GamesEngineeringBase::Window canvas;
    canvas.create(1024, 800, "test game");
    bool running = true; // Variable to control the main loop's running state.


    GAME_STATE state = GAME_STATE::MAIN_MENU;
    std::random_device rd;
    std::mt19937 gen(rd());

    // Load the map ONCE, before the loop
    TileMap map;
    if (!map.loadImages()|| !AttributeBar::LoadImages())
    {
        std::cout << "failed to load resources\n";
        
    }


   


    GamesEngineeringBase::Image mainMenuImage;
    mainMenuImage.load("Resources/menu.png");

    GamesEngineeringBase::Image gameOverImage;
    gameOverImage.load("Resources/game_over.png");

    GamesEngineeringBase::Image resultsImage;
    resultsImage.load("Resources/results.png");


    GamesEngineeringBase::Image player_projectileImage;
    player_projectileImage.load("Resources/player_projectile.png");
   

    
    GamesEngineeringBase::Image playerImage;
    playerImage.load("Resources/player.png");


    // Load an enemy 
    GamesEngineeringBase::Image enemyImage;
    enemyImage.load("Resources/enemy1.png");

  
    GamesEngineeringBase::Image enemy_projectileImage;
    enemy_projectileImage.load("Resources/enemy1_projectile.png");


    GamesEngineeringBase::Image keyImage;
    keyImage.load("Resources/key.png");

   



    // load health potion
    GamesEngineeringBase::Image potionHealthImage;
    potionHealthImage.load("Resources/potion_health.png");

    GamesEngineeringBase::Image potionManaImage;
    potionManaImage.load("Resources/potion_mana.png");


    GamesEngineeringBase::SoundManager sounds;
    sounds.loadMusic("Resources/music.wav");
    //sounds.playMusic();
    sounds.load("Resources/explosion.wav"); // Preload explosion sound.

    // Timer object to manage time-based events, such as movement speed
    GamesEngineeringBase::Timer timer;


    // current game 
    Manager* manager = nullptr;


    // Main game loop
    while (running)
    {

        // Check for input (key presses or window events)
        canvas.checkInput();

        // Clear the window for the next frame rendering
        canvas.clear();

        //If the escape key is pressed, exit the loop and close the window 
        if (canvas.keyPressed(VK_ESCAPE)) break;

        // Get the delta time for the frame
        float dt = timer.dt();





        if (state == GAME_STATE::MAIN_MENU)
        {
            Renderer_draw_image(canvas, { 0.0f, 0.0f }, mainMenuImage);

            if (canvas.keyPressed('1'))
            {
                map.load("Resources/level1.txt");
                delete manager;
                manager = new Manager(canvas, map, gen, playerImage, enemyImage, enemy_projectileImage, potionHealthImage, potionManaImage,keyImage);
                timer.dt();
                state = GAME_STATE::PLAYING_GAME;
            }
            else if (canvas.keyPressed('2'))
            {
                map.generateInfiniteWorld(100, 100, gen);  // infinite map
                delete manager;
                manager = new Manager(canvas, map, gen, playerImage, enemyImage, enemy_projectileImage, potionHealthImage, potionManaImage, keyImage);
                timer.dt();
                state = GAME_STATE::PLAYING_GAME;
            }


        }



        else if (state == GAME_STATE::PLAYING_GAME)
        {
            manager->stats_.timeSurvived += dt;
            Vec2f input{ 0, 0 };
            // Control the plane's movement using W, S, A, D keys
            if (canvas.keyPressed('W'))
            {
                input.y_ = -1.0f;
            }
            if (canvas.keyPressed('S'))
            {
                input.y_ = 1.0f;
            }
            if (canvas.keyPressed('A'))
            {
                input.x_ = -1.0f;
            }
            if (canvas.keyPressed('D'))
            {
                input.x_ = 1.0f;
            }

            Vec2f aim{ 0.0f, 0.0f };

            if (canvas.keyPressed(VK_UP))
            {
                aim.y_ = -1.0f;
            }
            if (canvas.keyPressed(VK_DOWN))
            {
                aim.y_ = 1.0f;
            }
            if (canvas.keyPressed(VK_LEFT))
            {
                aim.x_ = -1.0f;
            }
            if (canvas.keyPressed(VK_RIGHT))
            {
                aim.x_ = 1.0f;
            }

            if (aim.length() > 0.0f && manager->player_.CanShoot())
            {
                Vec2f direction = aim * (1.0f / aim.length());

                manager->projectiles_.SpawnProjectile(manager->player_.GetCentre(), direction,
                    800.0f, &player_projectileImage, &map, true, 1);
                manager->player_.resetShootTimer();
                manager->stats_.shotsFired++;
            }


            manager->player_.SetInputAxis(input);
            manager->player_.Update(dt);




            if (canvas.keyPressed(VK_SHIFT)&& manager->player_.GetMana() >= MANA_COST)
            {
                int killed = manager->enemies_.KillFromList(MANA_KILLS);
                if (killed > 0)
                {
                    manager->player_.UseMana(MANA_COST);
                }
            }





            manager->player_.CollectPickups(manager->pickups_, Manager::MAX_PICKUPS);
            manager->camera_.follow(manager->player_.GetCentre());

            manager->enemies_.Update(dt, gen);
            manager->enemies_.Shoot(manager->projectiles_);


            manager->projectiles_.Update(dt);
            manager->enemies_.ResolveHits(manager->projectiles_);
            manager->DropPotion(gen);
            manager->player_.ApplyEnemyProjectile(manager->projectiles_);






            map.draw(canvas, manager->camera_.getX(), manager->camera_.getY());

            for (int i = 0; i < Manager::MAX_PICKUPS; i++)
            {
                manager->pickups_[i].Draw(canvas, manager->camera_);
            }


            manager->enemies_.Draw(canvas, manager->camera_);

            manager->projectiles_.Draw(canvas, manager->camera_);


            manager->player_.Draw(canvas, manager->camera_);

         

            if (!manager->player_.IsAlive())
            {
                manager->stats_.Print();
                state = GAME_STATE::GAME_OVER;
            }


        }
        else if (state == GAME_STATE::GAME_OVER)
        {
            

            Renderer_draw_image(canvas, { 0.0f, 0.0f }, gameOverImage);

            if (canvas.keyPressed('R'))
            {
                delete manager;
                manager = new Manager(canvas, map, gen, playerImage, enemyImage, enemy_projectileImage, potionHealthImage, potionManaImage,keyImage);
                timer.dt();
                state = GAME_STATE::PLAYING_GAME;
            }
            else if (canvas.keyPressed('M'))
            {
                state = GAME_STATE::MAIN_MENU;
            }
            else if (canvas.keyPressed(VK_RETURN))
            {
                state = GAME_STATE::RESULT;
            }
        }
        else if (state == GAME_STATE::RESULT)
        {
            Renderer_draw_image(canvas, { 0.0f,0.0f }, resultsImage);
            if (canvas.keyPressed(VK_RETURN))
            {
                state = GAME_STATE::GAME_OVER;
            }
        }

        
        canvas.present();
        
    }
    delete manager;
    return 0;
}