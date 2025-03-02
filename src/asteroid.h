#ifndef ASTEROID_H
#define ASTEROID_H

#include <raylib-cpp.hpp>
#include "polar_coordinate.h"

class Asteroid {
    public:
	Asteroid(Vector2 pos, float dx, float dy, int siz, int nVert, Color col, int sWidth, int sHeight);
        
	void Draw();
        bool ContainsBullet(Vector2 bulletCoords);
      
       	Vector2 getPosition();	
	float getVelocX();
	float getVelocY();
	int getRadius();
	int getSize();
	int getNumVertices();
    private:
	Vector2 position;
	float velocX;
	float velocY;
	int radii[3] = {10, 15, 30};
	int radius;
	int size;
	int numVertices;
	int spikinesses[3] = {4, 6, 10};
	int spikiness; // Standard deviation of the random gaussian process that chooses the euclidian distance from the asteroid's centroid to each vertex, with the radius being the mean
	Color colour;
	int SCREEN_WIDTH;
	int SCREEN_HEIGHT;

	// Keep both for now. TODO: Know numVertices so don't want these to have dynamic length
	std::vector<PolarCoordinate> polarVertices;
	std::vector<Vector2> cartesianVertices;
};

#endif // ASTEROID_H
