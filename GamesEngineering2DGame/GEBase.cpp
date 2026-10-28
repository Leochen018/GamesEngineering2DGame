#include <iostream>
#include "GamesEngineeringBase.hpp" // Include the GamesEngineeringBase header
#include "Camera.hpp"
#include "Vec2f.hpp" // Include the Vector header for addition and multiplication on 2D vectors
#include "Renderer.hpp"

int main() {

    // Create a canvas window with dimensions 1024x768 and title “Tiles"
    GamesEngineeringBase::Window canvas;
    canvas.create(1024, 768, "Leo's game");
    bool running = true; // Variable to control the main loop's running state.

    const Vec2f WORLDSIZE = { 2000,2000 };

    // Create a camera on the top-left corner 
    unsigned int camX = 0;
    unsigned int camY = 0;

    // Load an image and place it at coordinates (300, 300)
   
    GamesEngineeringBase::Image image;


    image.load("Resources/A_Resized.png");
    Renderer_draw_image(canvas, {100,220}, image);
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


        
       

        // Check if the plane goes out of bounds (off the screen)
        if (posX > (canvas.getWidth() - image.width) || posX < 0 || posY >(canvas.getHeight() - image.height) || posY < 0)
        {
            // Play the explosion sound when plane hits the edge
            //sounds.play("Resources/explosion.wav");

        }


        // get the mid point (X,Y) of the character in the world
        unsigned int characterCentreX = posX + (image.width / 2);
        unsigned int characterCentreY = posY + (image.height / 2);

        unsigned int halfScreenW = canvas.getWidth() / 2;
        unsigned int halfScreenH = canvas.getHeight() / 2;

        if (characterCentreX < halfScreenW) {

            camX = 0;
        }
        else {

            camX = characterCentreX - halfScreenW;
        }

        if (characterCentreY < halfScreenH)
        {
            camY = 0;
        }
        else
        {
            camY = characterCentreY - halfScreenH;
        }


        // Don't let the camera go past the right or bottom of the map
        if (camX > WORLDSIZE.x_ - canvas.getWidth())  camX = WORLDSIZE.x_ - canvas.getWidth();
        if (camY > WORLDSIZE.y_ - canvas.getHeight()) camY = WORLDSIZE.y_ - canvas.getHeight();













        // Clear the entire canvas with a blue background (RGB: 0, 0, 255)
        for (unsigned int i = 0; i < (canvas.getWidth() * canvas.getHeight()); i++)
        {
            //canvas.draw(i, 0, 0, 0); // Drawing the blue background pixel by pixel.
        }



        // Draw the visible part of the world using the background image
        for (unsigned int sy = 0; sy < canvas.getHeight(); sy++)
        {
            for (unsigned int sx = 0; sx < canvas.getWidth(); sx++)
            {
                // Which world pixel is at this screen pixel?
                unsigned int worldX = sx + camX;
                unsigned int worldY = sy + camY;

                // Which pixel of the background image is that? (repeats the image)
                unsigned int imgX = worldX % background.width;
                unsigned int imgY = worldY % background.height;

                canvas.draw(sx, sy, background.atUnchecked(imgX, imgY));
            }
        }



        // Step 1: world position -> screen position
        unsigned int screenX = posX - camX;
        unsigned int screenY = posY - camY;

        // Step 2: go through every pixel of the image
        for (unsigned int i = 0; i < image.height; i++)          // each row
        {
            for (unsigned int n = 0; n < image.width; n++)       // each pixel in the row
            {
                // Step 3: only if this pixel lands inside the window
                if ((screenX + n) < canvas.getWidth() && (screenY + i) < canvas.getHeight())
                {
                    // Step 4: only if the pixel isn't see-through
                    if (image.alphaAt(n, i) > 210)
                    {
                        // Step 5: copy the pixel's colour onto the screen
                        canvas.draw(screenX + n, screenY + i, image.at(n, i));
                    }
                }
            }
        }


        // Display the frame on the screen. This must be called once the frame is finished in order to display the frame.
        canvas.present();
    }
    return 0;
}