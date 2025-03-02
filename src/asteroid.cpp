#include <raylib-cpp.hpp>
#include <cmath>
#include <algorithm>
#include <random>
#include "asteroid.h"
#include "game_constants.h"

Asteroid::Asteroid(Vector2 pos, float dx, float dy, int siz, int nVert, Color col, int sWidth, int sHeight): position(pos), velocX(dx), velocY(dy), size(siz), numVertices(nVert), colour(col), polarVertices(nVert), cartesianVertices(nVert), SCREEN_WIDTH(sWidth), SCREEN_HEIGHT(sHeight) {
            
	    // Check size is within the bounds 1-3 and set the radius
	    if (size < 1) size = 1;
	    if (size > 3) size = 3;

	    radius = radii[size-1];
	    spikiness = spikinesses[size-1];

	    // Randomly generate euclidian representations for each of the five vertices from the centroid
	    
	    // Create random number generators for uniform real distribution and gaussian distribution
	    // NOTE: Obviously move this somewhere else
	    std::random_device rd; // Create seed
            std::mt19937 gen(rd());

	    std::uniform_real_distribution<> uniformDis(0.0, 1.0);
	    std::normal_distribution<> gaussDis(radius, spikiness);

	    for (int i = 0; i < numVertices; i++) {
	        polarVertices[i].setMagnitude(radius + gaussDis(gen));
	        //polarVertices[i].setTheta(2*GC::pi * state.uniformDis(state.gen));	
	    	// Theta has to be ordered to avoid lines overlapping
		// TODO: Don't evenly distribute theta in a circle. Add some randomness
		polarVertices[i].setTheta(i*(2*GC::pi/numVertices));
		cartesianVertices[i] = polarVertices[i].to_cartesian(position);
            }
	}

void Asteroid::Draw() {

	    // Update the centroid and the vertices' cartesian positions
	    position.x += velocX;
	    position.y += velocY;

	    // Loop asteroid back round if entire circle + n standard deviations of spikiness distribution away from mean
	    // is off the screen
	    // TODO: Just store the largest vertex magnitude and use this as the offset.
	    float offset = (2*radius) + (3*spikiness);
	    // Check if off screen
            if (position.x > SCREEN_WIDTH + offset) position.x = -offset;
	    if (position.x < -offset) position.x = SCREEN_WIDTH + offset;
            if (position.y > SCREEN_HEIGHT + offset) position.y = -offset;
	    if (position.y < -offset) position.y = SCREEN_HEIGHT + offset;
            
	    // TODO: Put this all in a single for loop
	    // TODO: Rather than keeping track of all cartesian vertices, it will be easier to change the origin
	    // and update the cartesian points based off the polarVertices, if a little slower
	    for (int i = 0; i < numVertices; i++) {
		// Update positions based on x and y components of velocity
	        //cartesianVertices[i].x += velocX;
		//cartesianVertices[i].y += velocY;
		cartesianVertices[i] = polarVertices[i].to_cartesian(position);
            }

	    for (int i = 0; i < (numVertices - 1); i++) {
	        // Draw lines between asteroid's vertices
		DrawLineV(cartesianVertices[i], cartesianVertices[i + 1], colour);
	    }
	    DrawLineV(cartesianVertices[numVertices - 1], cartesianVertices[0], WHITE);
	    //DrawCircle(position.x, position.y, radius, colour);
	
	}

bool Asteroid::ContainsBullet(Vector2 bulletCoords) {
	    // This won't be too simple. For now, check if falls within the circle with radius
	    // radius from midpoint
	    
	    // Calculate euclidean distance from midpoint to bullet coordinate
	    float distance = std::sqrt(std::pow((bulletCoords.x - position.x), 2) + std::pow((bulletCoords.y - position.y), 2));

	    // Update boolean depending on whether it's in the circle
	    // NOTE: If at least one bullet is in the asteroid for each frame, this should
	    // not be able to be set to false until the next frame. The asteroid should be
	    // destroyed
	    return (distance < radius) ? true : false;
}

// Getters and setters
Vector2 Asteroid::getPosition() { return position; }
float Asteroid::getVelocX() { return velocX; }
float Asteroid::getVelocY() { return velocY; }
int Asteroid::getRadius() { return radius; };
int Asteroid::getSize() { return size;  }
int Asteroid::getNumVertices() { return numVertices; }
