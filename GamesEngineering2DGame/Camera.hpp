#pragma once
#include "GamesEngineeringBase.hpp"
#include "Vector.hpp"

class Camera
{
public:
	Camera(int screenW, int screenH, int worldW, int worldH) 
		: camX(0), camY(0), screenW(screenW), screenH(screenH), worldW(worldW), worldH(worldH){}

	
	// make the camera follow the centre of the object as world points
	// target.x_, target.y_ are the world coordinates of the midpoint of the sprint image
	// camX, camY are the  world coordinates of the top left corner of the canvas  

	//edge cases so that the camera doesnt go out of the world
	void follow(Vec2f target)
	{
		camX = target.x_ - (screenW / 2);
		camY = target.y_ - (screenH / 2);
		if (!bounded_)
		{
			return;
		}
		if (camX < 0)
		{
			camX = 0;
		}
		if (camY < 0)
		{
			camY = 0;
		}
		if (camX > (worldW - screenW))
		{
			camX = worldW - screenW;
		}
		if (camY > (worldH - screenH))
		{
			camY = worldH - screenH;
		}
	}
	Vec2f toScreen(const Vec2f& worldPos) const
	{
		return { worldPos.x_ - static_cast<float>(camX),
				 worldPos.y_ - static_cast<float>(camY) };
	}
	void SetBounded(bool bounded)
	{
		bounded_ = bounded;
	}

	int getX() const 
	{
		return camX;
	}
	int getY() const
	{
		return camY;
	}


private:
	int camX, camY;  // world position of the camera top left corner
	int screenW, screenH;  // screen coordinates of the 
	int worldW, worldH;  // size of the world 
	bool bounded_ = true;
};