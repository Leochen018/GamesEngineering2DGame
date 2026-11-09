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
    int tileAt(int col, int row) const { return tiles_[row * width_ + col]; }

    // gets the width and height pixels of the tile map
    int widthPixels()  const { return width_ * TILE_SIZE; }
    int heightPixels() const { return height_ * TILE_SIZE; }


    // check if the position is passable
    bool isSolidTile(int col, int row)
    {
        if (col < 0 || row < 0 || col >= width_ || row >= height_)
        {
            return true;
        }
       
        return isPassableTile[tileAt(col, row)];
    }
    //int x and y are player left corner position
    //int w and h are size of the character image
    bool isAreaBlocked(int x, int y, int w, int h)
    {
        // anything outside the world is blocked
        if (x < 0 || y < 0 || x + w > widthPixels() || y + h > heightPixels())
        {
            return true;
        }
        int firstCol = x / TILE_SIZE;
        int lastCol = (x + w - 1) / TILE_SIZE;
        int firstRow = y / TILE_SIZE;
        int lastRow = (y + h - 1) / TILE_SIZE;

        for (int r = firstRow; r <= lastRow; r++)
        {
            for (int c = firstCol; c <= lastCol; c++)
            {
                if (isSolidTile(c, r)) return true;
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
        int firstCol = camX / TILE_SIZE;
        int firstRow = camY / TILE_SIZE;

        int lastCol = (camX + (int)canvas.getWidth() - 1) / TILE_SIZE;
        int lastRow = (camY + (int)canvas.getHeight() - 1) / TILE_SIZE;

        if (firstCol < 0) firstCol = 0;
        if (firstRow < 0) firstRow = 0;
        if (lastCol >= width_)  lastCol = width_ - 1;
        if (lastRow >= height_) lastRow = height_ - 1;

        for (int r = firstRow; r <= lastRow; r++)
            for (int c = firstCol; c <= lastCol; c++)
                Renderer_draw_image(canvas,
                    { (float)(c * TILE_SIZE - camX), (float)(r * TILE_SIZE - camY) },
                    images_[tileAt(c, r)]);
    }



private:
	GamesEngineeringBase::Image images_[NUM_TILE_TYPES]; // stores the  number of different type of tiles
	int* tiles_;
	int width_, height_;  // size= of the tile map


    
    // states which of the tile id is passable/ block movement
    bool isPassableTile[NUM_TILE_TYPES] = { false, true, false, true, true, false, false, false, false, true };
   
};