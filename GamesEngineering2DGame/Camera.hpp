#pragma once
#include "GamesEngineeringBase.hpp"
#include "Vec2f.hpp"

class Camera
{
public:
	Camera(int screenW, int screenH, int worldW, int worldH) 
		: camX(0), camY(0), screenW(screenW), screenH(screenH), worldW(worldW), worldH(worldH){}

	// make the camera follow the centre of the object as world points
	// targetX, targetY are 
	void follow(Vec2f target)
	{
		camX = target.x_ - (screenW / 2);
		camY = target.y_ - (screenH / 2);

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
	// world -> screen
	int toScreenX(int worldX) const { return worldX - camX; }
	int toScreenY(int worldY) const { return worldY - camY; }

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
	int screenW, screenH;
	int worldW, worldH;
};