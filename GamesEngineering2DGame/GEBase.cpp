#include <iostream>
#include "GamesEngineeringBase.hpp" // Include the GamesEngineeringBase header
#include "Camera.hpp"
#include "Vec2f.hpp" // Include the Vector header for addition and multiplication on 2D vectors
#include "Renderer.hpp"

int main() {

    // Create a canvas window with dimensions 1024x768 and title “Tiles"
    GamesEngineeringBase::Window canvas;
    canvas.create(1024, 768, "test game");
    bool running = true; // Variable to control the main loop's running state.

    const Vec2f WORLDSIZE = { 2000,2000 };

    // Create a camera object 
    Camera camera(canvas.getWidth(), canvas.getHeight(),
        static_cast<int>(WORLDSIZE.x_), static_cast<int>(WORLDSIZE.y_));

    // Load an image and place it at coordinates (300, 300)
    GamesEngineeringBase::Image image;
    image.load("Resources/A_Resized.png");

    // Load an enemy 
    GamesEngineeringBase::Image enemy;
    enemy.load("Resources/L2.png");

    // Load background image
    GamesEngineeringBase::Image background;
    background.load("Resources/Background.png");

    unsigned int posX = (WORLDSIZE.x_/2)- (image.width/2);  // Initial x-coordinate for the plane image. placed in the middle of the world 
    unsigned int posY = (WORLDSIZE.y_/2) - (image.height/2);  // Initial y-coordinate for the plane image.

    // Load and play background music and an explosion sound
    GamesEngineeringBase::SoundManager sounds;
    sounds.loadMusic("Resources/music.wav");
    sounds.playMusic();
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

        //keep the player from leaving the screen
        unsigned int maxX = WORLDSIZE.x_ - image.width;
        unsigned int maxY = WORLDSIZE.y_ - image.height;

        
       
        // Control the plane's movement using W, S, A, D keys
        if (canvas.keyPressed('W')) // Move plane up
        {
            if (posY < move_amount)
            {
                posY = 0;                      
            }
            else
            {
                posY = posY - move_amount;      
            }
        }
        if (canvas.keyPressed('S')) // Move plane down
        {
            posY = posY + move_amount;
            if (posY > maxY)
            {
                posY = maxY;   // stop at bottom edge
            }
        }
        if (canvas.keyPressed('A')) // Move plane left
        {
            if (posX < move_amount)
            {
                posX = 0;
            }
            else {

                posX = posX - move_amount;
            }
        }
        if (canvas.keyPressed('D')) // Move plane right
        {
            posX = posX + move_amount;
            if (posX > maxX)
            {
                posX = maxX;   // stop at right edge
            }
        }


        
       

        // posX,posY are the world coordinates of the left corner of the image 

        // get the coordinates of the midpoint of the character in the world
        unsigned int characterCentreX = posX + (image.width / 2);
        unsigned int characterCentreY = posY + (image.height / 2);

        camera.follow({ static_cast<float>(characterCentreX),
                          static_cast<float>(characterCentreY) });
       



    
        
        // Draw the visible part of the world using the background image
        for (unsigned int sy = 0; sy < canvas.getHeight(); sy++)
        {
            for (unsigned int sx = 0; sx < canvas.getWidth(); sx++)
            {
                // Which world pixel is at this screen pixel?
                unsigned int worldX = sx + camera.getX();
                unsigned int worldY = sy + camera.getY();

                // Which pixel of the background image is that? (repeats the image)
                unsigned int imgX = worldX % background.width;
                unsigned int imgY = worldY % background.height;

                canvas.draw(sx, sy, background.atUnchecked(imgX, imgY));
            }
        }
        
        
        // world position -> screen position
        int screenX = camera.toScreenX(posX);
        int screenY = camera.toScreenY(posY);
        
        Renderer_draw_image(canvas,
            { static_cast<float>(screenX), static_cast<float>(screenY) },
            image);
        
        // draw the enemy at position 0,0
        Renderer_draw_image(canvas, {0,0} ,enemy);

      
        // Display the frame on the screen. This must be called once the frame is finished in order to display the frame.
        canvas.present();
    }
    return 0;
}