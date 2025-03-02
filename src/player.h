#ifndef PLAYER_H
#define PLAYER_H

#include "game_constants.h"

class Player {
    public:
	Player();
	bool CollidedWithAsteroid(Asteroid& asteroid);
	void Draw();
	float getDeltaXShip();
	float getDeltaYShip();
	float getLength();
	std::vector<Vector2> getPoints();
    private:
	//int SCREEN_WIDTH;
	//int SCREEN_HEIGHT;
        float accel = 0.2; // pixels per frame^2
        float dragCoeff = 0.99;
	int length = 50;
        int width = 20;
        int numPoints = 11;
        // x and y components of player velocity
        float velocX = 0;
        float velocY = 0;

	float deltaXShip;
	float deltaYShip;
         
	// Radians to rotate by per frame (0.02 radians)
        float theta = (2 * GC::pi / 50);

	std::vector<Vector2> points;
	Vector2 midpoint;
};

#endif // PLAYER_H
