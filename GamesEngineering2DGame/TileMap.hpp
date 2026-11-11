#pragma once
#include <fstream>
#include <string>
#include <iostream>
#include "GamesEngineeringBase.hpp"
#include "Renderer.hpp"
#include <random>

const int TILE_SIZE = 32;
const int NUM_TILE_TYPES = 10;
const int LOCKED_DOOR_TILE = 9;

class TileMap
{
public:

	TileMap() : tiles_(nullptr), width_(0), height_(0) {}
	~TileMap() { delete[] tiles_; }
    TileMap(const TileMap&) = delete;            // stop accidental double-delete
    TileMap& operator=(const TileMap&) = delete;


    // Load each tile image one time
    bool loadImages()
    {
        for (int i = 0; i < NUM_TILE_TYPES; i++) 
        {
            images_[i].load("Resources/" + std::to_string(i) + ".png");
            if (images_[i].data == nullptr)
            {
                return false;
            }
        }
        return true;
    }

    // File format

    bool load(const std::string& filename)
    {
        infinite_ = false;
        std::ifstream infile(filename);
        if (!infile.is_open())
        {
            std::cout << "Failed to open file: " << filename << std::endl;
            return false;

        }

        //checks for the first two numbers in the file. width and height of the world
        int w, h;
        if (!(infile >> w >> h) || w <= 0 || h <= 0)
        {
            return false;
        }
        delete[] tiles_;
        width_ = w;
        height_ = h;
        // turn the 2d tile map into 1d array
        tiles_ = new int[w * h];

        // goes through each tile id
        for (int i = 0; i < w * h; i++)
        {
            int id = 0;
            if (!(infile >> id) || id < 0 || id >= NUM_TILE_TYPES)
            {
                id = 0;
            }
            tiles_[i] = id;
        }
        return true;

    }
    // converts the 2d array tile map position to the 1d array position 
    int tileAt(int column, int row) const 
    {
        if (infinite_)
        {
            // for infinite map, the column and row are wrapped back to the grid first
         
            column = ((column % width_) + width_) % width_;
            row = ((row % height_) + height_) % height_;
        }
        return tiles_[row * width_ + column];
    }

    void generateInfiniteWorld(int width, int height, std::mt19937& gen)
    {
        infinite_ = true;
        delete[] tiles_;
        width_ = width;
        height_ = height;
        tiles_ = new int [width_ * height_];

        std::uniform_int_distribution<int> distribution(0, 99);
        for (int row = 0; row < height_; row++)
        {
            for (int column = 0; column < width_; column++)
            {
                int i = row * width_ + column;
                tiles_[i] = 0;    // ground by default

                // walls only every 4th column and row, so gaps are always 3 tiles (96 px) wide
                if (column % 4 == 0 && row % 4 == 0 && distribution(gen) < 50)
                {
                    tiles_[i] = 3;    // wall
                }
            }
        }

        //player pos is at  300, 400 which is column 9, row 12
        // we want to clear area around the player's start
        //clear 4 tiles from every direction
        
        for (int row = 8; row <= 16; row++)
        {
            for (int column = 5; column <= 13; column++)
            {
                tiles_[row * width_ + column] = 0;
            }
        }

    }

    bool isInfiniteWorld() const 
    {
        return infinite_;
    }

    // gets the width and height pixels of the tile map
    int widthPixels()  const { return width_ * TILE_SIZE; }
    int heightPixels() const { return height_ * TILE_SIZE; }


    // check if the position is passable
    bool isSolidTile(int col, int row)
    {
        if (!infinite_ && (col < 0 || row < 0 || col >= width_ || row >= height_))
        {
            return true;
        }
       
        return isPassableTile[tileAt(col, row)];
    }
    //int x and y are player left corner position
    //int w and h are size of the character image
    bool isAreaBlocked(int x, int y, int w, int h)
    {

        if (infinite_)
        {
           //take the box's top left corner and wrap it back inside the generated grid
           //so that x and y always end up between 0 and 3199 
            x = ( (x %  widthPixels() ) + widthPixels()) % widthPixels();
            y = ((y % heightPixels()) + heightPixels()) % heightPixels();

        }
        // anything outside the world is blocked
        else if (x < 0 || y < 0 || x + w > widthPixels() || y + h > heightPixels())
        {
            return true;
        }
        int firstCol = x / TILE_SIZE;
        int firstRow = y / TILE_SIZE;

        int lastCol = (x + w - 1) / TILE_SIZE;
        int lastRow = (y + h - 1) / TILE_SIZE;

        for (int row = firstRow; row <= lastRow; row++)
        {
            for (int column = firstCol; column <= lastCol; column++)
            {
                if (isSolidTile(column, row)) return true;
            }
        }

        return false;
    }


    bool FindRandomSpot(std::mt19937& gen, int w, int h, const Vec2f& avoidCentre, float minDistance, Vec2f& out)
    {
        std::uniform_int_distribution<int> randomX(0, widthPixels() - w);
        std::uniform_int_distribution<int> randomY(0, heightPixels() - h);
        for (int i = 0; i < 100; i++) // tries 100 random spots in the world for a enemy to spawn
        {
            int x = randomX(gen);
            int y = randomY(gen);

            if (isAreaBlocked(x, y, w, h)) continue; // cant spawn where area is blocked

            //centre of the potential enemy to spawn in at x,y 
            Vec2f centre = { x + (w / 2.0f), y + (h / 2.0f) };
            // check if the random spot is near player centre
            if ((centre - avoidCentre).length() < minDistance) continue;

            out = { static_cast<float>(x), static_cast<float>(y)};
            return true;

        }
        return false;
    } 

    void setPassableTile(int tileId, bool passable)
    {
        if (tileId >= 0 && tileId < NUM_TILE_TYPES)
        {
            isPassableTile[tileId] = passable;
        }
    }


    // camX,camY are the position of the camera. follows top left corner of the player 
    void draw(GamesEngineeringBase::Window& canvas, int camX, int camY)
    {
        if (infinite_)
        {
            camX = ( (camX % widthPixels()) + widthPixels() ) % widthPixels();
            camY = ( (camY % heightPixels()) + heightPixels() ) % heightPixels();
        }

        int firstCol = camX / TILE_SIZE;
        int firstRow = camY / TILE_SIZE;

        int lastCol = (camX + (int)canvas.getWidth() - 1) / TILE_SIZE;
        int lastRow = (camY + (int)canvas.getHeight() - 1) / TILE_SIZE;


        //for fixed map: dont go past the edges
        if (!infinite_)
        {
            if (firstCol < 0)
            {
                firstCol = 0;
            }
            if (firstRow < 0)
            {
                firstRow = 0;
            }
            if (lastCol >= width_)
            {
                lastCol = width_ - 1;
            }
            if (lastRow >= height_)
            {
                lastRow = height_ - 1;
            }
        }
        //draw both maps
        for (int row = firstRow; row <= lastRow; row++)
        {
            for (int column = firstCol; column <= lastCol; column++)
            {
                Renderer_draw_image(canvas,
                    { (float)(column * TILE_SIZE - camX), (float)(row * TILE_SIZE - camY) },
                    images_[tileAt(column, row)]);
            }
        }

    }



private:
	GamesEngineeringBase::Image images_[NUM_TILE_TYPES]; // stores the  number of different type of tiles
	int* tiles_;
	int width_, height_;  // size= of the tile map
    bool infinite_ = false;

    
    // states which of the tile id is passable/ block movement
    bool isPassableTile[NUM_TILE_TYPES] = { false, true, false, true, true, false, false, false, false, true };
   
};