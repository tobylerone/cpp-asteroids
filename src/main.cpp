#include <iostream>
#include <raylib-cpp.hpp>
#include <cmath>
#include <algorithm>
#include <random>

class Player {
 
     public:
	 int screenWidth;
	 int screenHeight;
         float accel = 0.2; // pixels per frame^2
	 const double pi = 3.141592653589793238;
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
         float theta = (2 * pi / 50);

	 std::vector<Vector2> points;
	 Vector2 midpoint;
    
	 Player(int sw, int sh) : screenWidth(sw), screenHeight(sh) {
	
             // Main line down the middle of the ship
             Vector2 point0 = {screenWidth/2, screenHeight/2 - length/2};
             Vector2 point1 = {screenWidth/2, screenHeight/2 + length/2};

             // Perpendicular bar at the back of the ship
             Vector2 point2 = {screenWidth/2 - width/2, screenHeight/2 + length/2};
             Vector2 point3 = {screenWidth/2 + width/2, screenHeight/2 + length/2};

             // Side panels
             Vector2 point4 = point0;
             Vector2 point5 = {screenWidth/2 - width/2 - (width * 0.1), screenHeight/2 + length/2 + (length * 0.1)};
             Vector2 point6 = point0;
             Vector2 point7 = {screenWidth/2 + width/2 + (width * 0.1), screenHeight/2 + length/2 + (length * 0.1)};
    
             // Thruster
             Vector2 point8 = {screenWidth/2 - width/4, screenHeight/2 + length/2};
             Vector2 point9 = {screenWidth/2 + width/4, screenHeight/2 + length/2};
             Vector2 point10 = {screenWidth/2, screenHeight/2 + 3*length/4};

             points = {point0, point1, point2, point3, point4, point5, point6, point7, point8, point9, point10};

             // Take the centre of rotation as the centre of the line running down the centre of the ship
             midpoint = {(point0.x + point1.x)/2 - (point0.x - point1.x)/4, (point0.y + point1.y)/2 - (point0.y - point1.y) / 4}; // Shift back to 1/4 along midline
	    
	 }
        
	 void Draw() {
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
	    if (midpoint.x > screenWidth + length) {
	        for (int i = 0; i < numPoints; i++) {
	            points[i].x -= screenWidth + length*2;
	        }
	    } else if (midpoint.x < -length) {
	        for (int i = 0; i < numPoints; i++) {
		    points[i].x += screenWidth + length*2;
		}
	    }

	    if (midpoint.y > screenHeight + length) {
	        for (int i = 0; i < numPoints; i++) {
	            points[i].y -= screenHeight + length*2;
	        }
	    } else if (midpoint.y < -length) {
	        for (int i = 0; i < numPoints; i++) {
		    points[i].y += screenHeight + length*2;
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
	    // delta x = sqrt(s - (1 + (dy/dx)) and the y component delta y = sqrt(s - (delta x)^2) 
	    // So over a discrete time replace s with the distance travelled (s*deltat) in the direction of
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
            //pMidpoint = {(pPoints[0].x + pPoints[1].x)/2, (pPoints[0].y + pPoints[0].y)/2};
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
};

class Bullet {
    public:
        Vector2 position;
	float velocX;
	float velocY;
	float radius;
	Color color;

	Bullet(Vector2 pos, float dx, float dy, float r, Color c): position(pos), velocX(dx), velocY(dy), radius(r), color(c) {}
	
	void Draw() {
	    // Update the positions
            position.x += velocX;
	    position.y += velocY;
	    DrawCircleV(position, radius, color);
	}

        bool IsOffScreen(int screenWidth, int screenHeight) {
	    return (position.x < 0 || position.x > screenWidth || position.y < 0 || position.y > screenHeight);
	}
};

class PolarCoordinate {
    public:
        float magnitude; //pixels
        float theta; //radians

        PolarCoordinate(float mag, float the) : magnitude(mag), theta(the) {}
	
	// Default constructor
	PolarCoordinate() : magnitude(0), theta(0) {}

        Vector2 to_cartesian(Vector2 origin) {
            return {origin.x + magnitude*cos(theta), origin.y + magnitude*sin(theta)};
        }
};

class Asteroid {
    public:
        Vector2 position;
	float velocX;
	float velocY;
	float radius;
	int numVertices;
	int spikiness; // Standard deviation of the random gaussian process that chooses the euclidian distance from the asteroid's centroid to each vertex, with the radius being the mean
        Color colour;
	int screenWidth;
	int screenHeight;

	// Keep both for now. TODO: Know numVertices so don't want these to have dynamic length
	std::vector<PolarCoordinate> polarVertices;
	std::vector<Vector2> cartesianVertices;

	Asteroid(Vector2 pos, float dx, float dy, float rad, int nVert, int spik, Color col, int sWidth, int sHeight): position(pos), velocX(dx), velocY(dy), radius(rad), numVertices(nVert), spikiness(spik), colour(col), polarVertices(nVert), cartesianVertices(nVert), screenWidth(sWidth), screenHeight(sHeight) {
            
            // TODO: Avoid repeating this
	    const double pi = 3.141592653589793238;
            
	    // Randomly generate euclidian representations for each of the five vertices from the centroid
	    
	    // Create random number generators for uniform real distribution and gaussian distribution
	    // NOTE: Obviously move this somewhere else
	    std::random_device rd; // Create seed
            std::mt19937 gen(rd());

	    std::uniform_real_distribution<> uniformDis(0.0, 1.0);
	    std::normal_distribution<> gaussDis(radius, spikiness);

	    for (int i = 0; i < numVertices; i++) {
	        polarVertices[i].magnitude = radius + gaussDis(gen);
	        //polarVertices[i].theta = 2*pi * uniformDis(gen);	
	    	// Theta has to be ordered to avoid lines overlapping
		// TODO: Don't evenly distribute theta in a circle. Add some randomness
		polarVertices[i].theta = i*(2*pi/numVertices);
		cartesianVertices[i] = polarVertices[i].to_cartesian(position);
            }
	}

	void Draw() {

	    // Update the centroid and the vertices' cartesian positions
	    position.x += velocX;
	    position.y += velocY;

	    // Loop asteroid back round if entire circle + n standard deviations of spikiness distribution away from mean
	    // is off the screen
	    // TODO: Just store the largest vertex magnitude and use this as the offset.
	    float offset = (2*radius) + (3*spikiness);
	    // Check if off screen
            if (position.x > screenWidth + offset) position.x = -offset;
	    if (position.x < -offset) position.x = screenWidth + offset;
            if (position.y > screenHeight + offset) position.y = -offset;
	    if (position.y < -offset) position.y = screenHeight + offset;
            
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
	bool ContainsBullet(Vector2 bulletCoords) {
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
};

int main() {
    
    // Initialization
    int screenWidth = 1000;
    int screenHeight = 700;
    int fps = 60;
    const double pi = 3.141592653589793238;

    int score = 0;
    
    std::vector<Bullet> bullets;
    int bRadius = 2;
    float bSpeed = 30.0;
    Color bColor = WHITE;
    // Wait a period between successive bullet object creations
    int bFramesBetweenSpawn = 8;
    int bFramesUntilNextSpawn = 0;

    raylib::Color textColor(GREEN);
    raylib::Window w(screenWidth, screenHeight, "Asteroids");
    
    SetTargetFPS(fps);

    Player p = Player(screenWidth, screenHeight);

    Asteroid roid1 = Asteroid({100, 100}, 3, 4, 20, 10, 10, WHITE, screenWidth, screenHeight);
    Asteroid roid2 = Asteroid({600, 200}, 8, -5, 10, 7, 10, WHITE, screenWidth, screenHeight);
    Asteroid roid3 = Asteroid({200, 500}, -2, -3, 30, 12, 15, WHITE, screenWidth, screenHeight);
    Asteroid roid4 = Asteroid({700, 300}, 3, 5, 14, 12, 6, WHITE, screenWidth, screenHeight);
    
    std::vector<Asteroid> asteroids = {roid1, roid2, roid3, roid4};

    // Main game loop
    while (!w.ShouldClose()) // Detect window close button or ESC key
    {
	if (IsKeyDown(KEY_SPACE)) {
	    //Create a new bullet if enough frames have passed since the last spawn
            if (bFramesUntilNextSpawn <= 0) {
	    
                // Quick way to get the bullet x & y deltas
	        float bVelocX = p.deltaXShip * (bSpeed/p.length);
	        float bVelocY = p.deltaYShip * (bSpeed/p.length);
	    
	        bullets.push_back(Bullet({p.points[0].x, p.points[0].y}, bVelocX, bVelocY, bRadius, bColor));
	        bFramesUntilNextSpawn = bFramesBetweenSpawn;
	    }
	}

	// Decrement the frame count between bullet spawning
	if (bFramesUntilNextSpawn > 0) bFramesUntilNextSpawn--;

        // DRAW------------------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(BLACK);

	// Draw all bullets
	for (auto& bullet : bullets) {
	    bullet.Draw();
	}

	// Remove asteroids that have been hit by a bullet
	for (auto it = bullets.begin(); it != bullets.end(); ++it) {
	    // Check if bullet is inside any asteroid
	    for (auto asteroid_it = asteroids.begin(); asteroid_it != asteroids.end();) {
	        if (asteroid_it->ContainsBullet(it->position)) {
		    // Remove the asteroid and continue iterating
		    asteroid_it = asteroids.erase(asteroid_it);
		} else {
	            ++asteroid_it;
		}
	    }
	}

	// Remove bullets that are off screen
	bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
	    [screenWidth, screenHeight](Bullet& bullet) {
	        return bullet.IsOffScreen(screenWidth, screenHeight);
	    }), bullets.end());

        // Draw all asteroids
        for (auto& asteroid : asteroids) {
	    asteroid.Draw();
	}

	p.Draw();

	textColor.DrawText("Score: " + std::to_string(score) + "", 10, 10, 20);	
	EndDrawing();
    }

    return 0;
}
