#include <raylib-cpp.hpp>
#include "asteroid.h"
#include "player.h"
#include "game_constants.h"

Player::Player() {
	
    // Main line down the middle of the ship
    Vector2 point0 = {GC::SCREEN_WIDTH/2, GC::SCREEN_HEIGHT/2 - length/2};
    Vector2 point1 = {GC::SCREEN_WIDTH/2, GC::SCREEN_HEIGHT/2 + length/2};

    // Perpendicular bar at the back of the ship
    Vector2 point2 = {GC::SCREEN_WIDTH/2 - width/2, GC::SCREEN_HEIGHT/2 + length/2};
    Vector2 point3 = {GC::SCREEN_WIDTH/2 + width/2, GC::SCREEN_HEIGHT/2 + length/2};

    // Side panels
    Vector2 point4 = point0;
    Vector2 point5 = {GC::SCREEN_WIDTH/2 - width/2 - (width * 0.1), GC::SCREEN_HEIGHT/2 + length/2 + (length * 0.1)};
    Vector2 point6 = point0;
    Vector2 point7 = {GC::SCREEN_WIDTH/2 + width/2 + (width * 0.1), GC::SCREEN_HEIGHT/2 + length/2 + (length * 0.1)};
    
    // Thruster
    Vector2 point8 = {GC::SCREEN_WIDTH/2 - width/4, GC::SCREEN_HEIGHT/2 + length/2};
    Vector2 point9 = {GC::SCREEN_WIDTH/2 + width/4, GC::SCREEN_HEIGHT/2 + length/2};
    Vector2 point10 = {GC::SCREEN_WIDTH/2, GC::SCREEN_HEIGHT/2 + 3*length/4};

    points = {point0, point1, point2, point3, point4, point5, point6, point7, point8, point9, point10};

    // Take the centre of rotation as the centre of the line running down the centre of the ship
    midpoint = {(point0.x + point1.x)/2 - (point0.x - point1.x)/4, (point0.y + point1.y)/2 - (point0.y - point1.y) / 4}; // Shift back to 1/4 along midline

}

bool Player::CollidedWithAsteroid(Asteroid& asteroid) {
	     
    for (const auto& point : points) {
        // Calculate distance from point to midpoint of asteroid
	float distance = std::sqrt(std::pow((point.x - asteroid.getPosition().x), 2) + std::pow((point.y - asteroid.getPosition().y), 2));
	    if (distance <= asteroid.getRadius()) return true;
	}
            return false;
	}
        
void Player::Draw() {
    // Update variables
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_LEFT)) {
        // Rotate player by chosen number of radians
	// Translate points so that midpoint is at origin
	Vector2 pointsOrigin[numPoints];

	for (int i = 0; i < numPoints; i++){
	    pointsOrigin[i] = {points[i].x - midpoint.x, points[i].y - midpoint.y};
	}
	
	//Vector2 point1Origin = {point1.x - midpoint.x, point1.y - midpoint.y};
	//Vector2 point2Origin = {point2.x - midpoint.x, point2.y - midpoint.y};

	Vector2 pointsOriginRot[numPoints];
	// Rotate points using appropriate rotation matrix relative to the origin
	if (IsKeyDown(KEY_RIGHT)) {
	    // Clockwise rotation
	    for (int i = 0; i < numPoints; i++) {
	        pointsOriginRot[i] = {cos(theta)*pointsOrigin[i].x -sin(theta)*pointsOrigin[i].y, sin(theta)*pointsOrigin[i].x + cos(theta)*pointsOrigin[i].y};
	    }
	} else {
	    // Anticlockwise rotation
	    for (int i = 0; i < numPoints; i++) {
	        pointsOriginRot[i] = {cos(theta)*pointsOrigin[i].x + sin(theta)*pointsOrigin[i].y, -sin(theta)*pointsOrigin[i].x + cos(theta)*pointsOrigin[i].y};
	    }
	}

	// Translate points back to absolute locations
	for (int i = 0; i < numPoints; i++) {
	    points[i] = {pointsOriginRot[i].x + midpoint.x, pointsOriginRot[i].y + midpoint.y};
	}
    }
    
    // Calculate the direction of travel (the direction from point1 to point0
    deltaXShip = points[0].x - points[1].x;
    deltaYShip = points[0].y - points[1].y;
	    
    // To avoid extra computations, just loop back if midpoint is at least the ship's length off the screen
    // TODO: Avoid repetition here
    if (midpoint.x > GC::SCREEN_WIDTH + length) {
        for (int i = 0; i < numPoints; i++) {
	    points[i].x -= GC::SCREEN_WIDTH + length*2;
	}
    } else if (midpoint.x < -length) {
        for (int i = 0; i < numPoints; i++) {
	    points[i].x += GC::SCREEN_WIDTH + length*2;
	}
    }

    if (midpoint.y > GC::SCREEN_HEIGHT + length) {
	for (int i = 0; i < numPoints; i++) {
	    points[i].y -= GC::SCREEN_HEIGHT + length*2;
	}
    } else if (midpoint.y < -length) {
	for (int i = 0; i < numPoints; i++) {
	    points[i].y += GC::SCREEN_HEIGHT + length*2;
        }
    }

    // Move ship forwards
    if (IsKeyDown(KEY_UP)) {
                
	float accelByShipLength = accel / length;

	//float deltaX;
	//float deltaY;

	if (deltaXShip != 0.0) {
	    velocX += deltaXShip * accelByShipLength;
	    velocY += deltaYShip * accelByShipLength;
	} else {
	    // Infinite gradient. No x delta
	    velocY -= accel;
	    velocX = 0;
	}
    }
    
    // Given a known velocity with speed component s and direction g (dy/dx), the x component
    // delta x = sqrt(s - (1 + (dy/dx)) and the y component delta y = sqrt(s - (delta x)^2)    // So over a discrete time replace s with the distance travelled (s*deltat) in the direction of
    // velocity and you can use this to work out how far you should shift in the x and
    // y direction, which makes intuitive sense since deltaX is sqrt(s) for zero gradient
    // and deltaY = sqrt(s) for infinite gradient (X*2 + y*2 = s * deltat)
    //float deltaX = sqrt(pSpeed - (1 + gradient));
    //float deltaY = sqrt(pSpeed - deltaX*2);

    // Alternatively, the ratio of speed to ship length will let you map deltay/delta x between
    // points 0 and 1 to the x and y shift for the time step

    for (int i = 0; i < numPoints; i++) {
        points[i].x += velocX;
	points[i].y += velocY;
    }

    // Recalculate the midpoint	    
    midpoint = {(points[0].x + points[1].x)/2 - (points[0].x - points[1].x)/4, (points[0].y + points[1].y)/2 - (points[0].y - points[1].y) / 4}; // Shift back to 1/4 along midline

    // Decay the speed
    velocX *= dragCoeff;
    velocY *= dragCoeff;	

    DrawTriangle(points[10], points[9], points[8], IsKeyDown(KEY_UP) ? ORANGE : BLACK);
    DrawLine(points[0].x, points[0].y, points[1].x, points[1].y, BLACK);
    DrawLine(points[2].x, points[2].y, points[3].x, points[3].y, WHITE);
    DrawLine(points[4].x, points[4].y, points[5].x, points[5].y, WHITE);
    DrawLine(points[6].x, points[6].y, points[7].x, points[7].y, WHITE);
}

// Getters and setters

float Player::getDeltaXShip() { return deltaXShip; };
float Player::getDeltaYShip() { return deltaYShip; };
float Player::getLength() { return length; };
std::vector<Vector2> Player::getPoints() { return points; };

