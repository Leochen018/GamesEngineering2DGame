#include <iostream>
#include "GamesEngineeringBase.hpp" // Include the GamesEngineeringBase header
#include "Camera.hpp"
#include "Vector.hpp" // Include the Vector header for addition and multiplication on 2D vectors
#include "Renderer.hpp"
#include "TileMap.hpp" // Include the world map background
#include "Enemy.hpp"
#include "Player.hpp"
#include "Projectile.hpp"


const int MAX_PICKUPS = 20;

int main() {

    
    GamesEngineeringBase::Window canvas;
    canvas.create(1024, 768, "test game");
    bool running = true; // Variable to control the main loop's running state.




    // Load the map ONCE, before the loop
    TileMap map;
    if (!map.loadImages()) {
        std::cout << "Couldn't load tile images\n";
        return 1;
    }
    if (!map.load("Resources/level1.txt")) {
        std::cout << "Couldn't load level1.txt\n";
        return 1;
    }

    // gets world size from the tilemap 
    const Vec2f WORLDSIZE = { (float)map.widthPixels(), (float)map.heightPixels() };

    // Create a camera object 
    Camera camera(canvas.getWidth(), canvas.getHeight(),
        static_cast<int>(WORLDSIZE.x_), static_cast<int>(WORLDSIZE.y_));

    GamesEngineeringBase::Image projectile1Image;
    projectile1Image.load("Resources/projectile1.png");
    ProjectileEntity projectiles[200];

    
    GamesEngineeringBase::Image playerImage;
    playerImage.load("Resources/player.png");

    Player player({ 300.0f,400.0f }, &playerImage, &map, 700.0f);

    // Load an enemy 
    GamesEngineeringBase::Image enemyImage;
    enemyImage.load("Resources/enemy.png");

    EnemyEntity enemy1({ 50.0f, 50.0f }, &enemyImage, &map);

    SpriteEntity* playerObstacles[1] = { &enemy1 };
    player.SetObstacles(playerObstacles, 1);

    enemy1.SetBlocker(&player);


    // load health potion
    GamesEngineeringBase::Image potionHealthImage;
    potionHealthImage.load("Resources/potion_health.png");
    PickUpEntity pickups[MAX_PICKUPS];

    pickups[0].Place({ 500.0f, 300.0f }, &potionHealthImage, PickUpType::Health, 3);
    pickups[1].Place({ 900.0f, 1200.0f }, &potionHealthImage, PickUpType::Health, 3);

    GamesEngineeringBase::SoundManager sounds;
    sounds.loadMusic("Resources/music.wav");
    //sounds.playMusic();
    sounds.load("Resources/explosion.wav"); // Preload explosion sound.

    // Timer object to manage time-based events, such as movement speed
    GamesEngineeringBase::Timer timer;



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
        
        if (aim.length() > 0.0f && player.CanShoot())
        {
            Vec2f direction = aim * (1.0f / aim.length());

            SpawnProjectile(projectiles, 200, player.GetCentre(), direction,
                800.0f, &projectile1Image, &map, true, 1);
            player.resetShootTimer();
        }


        player.SetInputAxis(input);
        player.Update(dt);


        player.CollectPickups(pickups, MAX_PICKUPS);
        camera.follow(player.GetCentre());
       

        enemy1.SetTarget(player.GetCentre());
        enemy1.Update(dt);

        for (int i = 0; i < 200; i++)
        {
            ProjectileEntity& p = projectiles[i];
            if (!p.IsAlive()) continue;
            p.Update(dt);
            if (!p.IsAlive()) continue;

            Vec2f q = p.GetPosition();
            if (p.IsFromPlayer() && enemy1.IsAlive() && enemy1.OverlapsBox(q.x_, q.y_, p.GetImageWidth(), p.GetImageHeight()))
            {
                enemy1.TakeDamage(p.GetDamage());
                p.Kill();
            }
        }
        if (!enemy1.IsAlive())
        {
            playerObstacles[0] = nullptr;
        }
        

        map.draw(canvas, camera.getX(), camera.getY());

        for (int i = 0; i < MAX_PICKUPS; i++)
        {
            pickups[i].Draw(canvas, camera);
        }

        if (enemy1.IsAlive())
        {
            enemy1.Draw(canvas, camera);
        }
        for (int i = 0; i < 200; i++)
        {
            projectiles[i].Draw(canvas, camera);
        }
        player.Draw(canvas, camera);
        
       
        

        // Display the frame on the screen. This must be called once the frame is finished in order to display the frame.
        canvas.present();
    }
    return 0;
}