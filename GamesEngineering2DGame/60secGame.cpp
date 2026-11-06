#include <iostream>
#include "GamesEngineeringBase.hpp" // Include the GamesEngineeringBase header
#include "Camera.hpp"
#include "Vector.hpp" // Include the Vector header for addition and multiplication on 2D vectors
#include "Renderer.hpp"
#include "TileMap.hpp" // Include the world map background
#include "Enemy.hpp"

int main() {

    // Create a canvas window with dimensions 1024x768 and title “Tiles"
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

    // Load player
    GamesEngineeringBase::Image player;
    player.load("Resources/player.png");

    // Load an enemy 
    GamesEngineeringBase::Image enemy;
    enemy.load("Resources/enemy.png");

    EnemyEntity enemy1({ 50.0f, 50.0f }, &enemy, &map);

    //inital position of player
    int posX = (WORLDSIZE.x_/2)- (player.width/2);  
    int posY = (WORLDSIZE.y_/2) - (player.height/2);  

    // Load and play background music and an explosion sound
    GamesEngineeringBase::SoundManager sounds;
    sounds.loadMusic("Resources/music.wav");
    //sounds.playMusic();
    sounds.load("Resources/explosion.wav"); // Preload explosion sound.

    // Timer object to manage time-based events, such as movement speed
    GamesEngineeringBase::Timer timer;

    // Define a move speed for the plane, in this case it is pixels moved per second
    float MOVE_SPEED = 700.0f;

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

        // Calculate how far the plane should move based on elapsed time (ensuring a minimum movement of 1 pixel)
        unsigned int move_amount = static_cast<unsigned int>(std::max(MOVE_SPEED * dt, 1.0f));
       
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

        //size of the player's character image. ie 64 x 64
        int w = player.width;
        int h = player.height;

        for (unsigned int i = 0; i < move_amount; i++)
        {
            if (input.x_ != 0 && !map.isAreaBlocked(posX + input.x_, posY, w, h))
            {
                posX += input.x_;
            }
            if (input.y_ != 0 && !map.isAreaBlocked(posX, posY + input.y_, w, h))
            {
                posY += input.y_;
            }
        }


        
       

        // posX,posY are the world coordinates of the left corner of the image 

        // get the coordinates of the midpoint of the character in the world

        Vec2i characterCentre = { posX + (player.width / 2), posY + (player.height / 2) };

        camera.follow(characterCentre.convertTo<float>());
       

        enemy1.SetTarget(characterCentre.convertTo<float>());
        enemy1.Update(dt);

        map.draw(canvas, camera.getX(), camera.getY());
        enemy1.Draw(canvas, camera);
        
        // world position -> screen position
        Vec2f playerScreen = camera.toScreen(Vec2i{ posX,posY }.convertTo<float>());
        
        Renderer_draw_image(canvas, playerScreen, player);
        

        // Display the frame on the screen. This must be called once the frame is finished in order to display the frame.
        canvas.present();
    }
    return 0;
}