#include <iostream>
#include "GamesEngineeringBase.h" // Include the GamesEngineeringBase header

int main() {

    // Create a canvas window with dimensions 1024x768 and title “Tiles"
    GamesEngineeringBase::Window canvas;
    canvas.create(1024, 768, "Leo's game");
    bool running = true; // Variable to control the main loop's running state.

    const unsigned int WORLD_WIDTH = 3000;
    const unsigned int WORLD_HEIGHT = 2000;

    // Create a camera on the top-left corner 
    unsigned int camX = 0;
    unsigned int camY = 0;

    // Load an image and place it at coordinates (300, 300)
    GamesEngineeringBase::Image image;
    image.load("Resources/A_Resized.png");
    unsigned int posX = (WORLD_WIDTH/2)- (image.width/2);  // Initial x-coordinate for the plane image. placed in the middle of the world 
    unsigned int posY = (WORLD_HEIGHT/2) - (image.height/2);  // Initial y-coordinate for the plane image.

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

        //keep the player inside the window for the impassable wall
        unsigned int maxX = canvas.getWidth() - image.width;
        unsigned int maxY = canvas.getHeight() - image.height;

       

       
        // Control the plane's movement using W, S, A, D keys
        if (canvas.keyPressed('W')) // Move plane up
        {
            if (posY < move_amount)
            {\
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
        }


        if (posX > maxX) posX = maxX;   // stop at right edge
        if (posY > maxY) posY = maxY;   // stop at bottom edge

        // Check if the plane goes out of bounds (off the screen)
        if (posX > (canvas.getWidth() - image.width) || posX < 0 || posY >(canvas.getHeight() - image.height) || posY < 0)
        {
            // Play the explosion sound when plane hits the edge
            sounds.play("Resources/explosion.wav");

        }

        // Clear the entire canvas with a blue background (RGB: 0, 0, 255)
        for (unsigned int i = 0; i < (canvas.getWidth() * canvas.getHeight()); i++)
        {
            canvas.draw(i, 0, 0, 255); // Drawing the blue background pixel by pixel.
        }


        //  Draw the image every frame (even though position doesn't change)
        for (unsigned int i = 0; i < image.height; i++)
        {
            for (unsigned int n = 0; n < image.width; n++)
            {
                if ((posX + n) >= 0 && (posX + n) < canvas.getWidth() &&
                    (posY + i) >= 0 && (posY + i) < canvas.getHeight())
                {
                    if (image.alphaAt(n, i) > 210)
                    {
                        canvas.draw(posX + n, posY + i, image.at(n, i));
                    }
                }
            }
        }


        // Display the frame on the screen. This must be called once the frame is finished in order to display the frame.
        canvas.present();
    }
    return 0;
}