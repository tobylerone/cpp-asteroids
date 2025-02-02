#include <iostream>
#include <raylib-cpp.hpp>
#include <cmath>
#include <algorithm>
#include <random>


enum GameState {
    MENU,
    PLAYING,
    NEXT_LEVEL,
    GAME_OVER
};

class Bullet {
    public:
        Vector2 position;
	float velocX;
	float velocY;
	float radius = 2;
	Color color = WHITE;

	Bullet(Vector2 pos, float dx, float dy): position(pos), velocX(dx), velocY(dy) {}
	
	void Draw() {
	    // Update the positions
            position.x += velocX;
	    position.y += velocY;
	    DrawCircleV(position, radius, color);
	}

        bool IsOffScreen(int SCREEN_WIDTH, int SCREEN_HEIGHT) {
	    return (position.x < 0 || position.x > SCREEN_WIDTH || position.y < 0 || position.y > SCREEN_HEIGHT);
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

	Asteroid(Vector2 pos, float dx, float dy, int siz, int nVert, Color col, int sWidth, int sHeight): position(pos), velocX(dx), velocY(dy), size(siz), numVertices(nVert), colour(col), polarVertices(nVert), cartesianVertices(nVert), SCREEN_WIDTH(sWidth), SCREEN_HEIGHT(sHeight) {
            
            // TODO: Avoid repeating this
	    const double pi = 3.141592653589793238;
            
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

class Player {
 
     public:
	 int SCREEN_WIDTH;
	 int SCREEN_HEIGHT;
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
    
	 Player(int sw, int sh) : SCREEN_WIDTH(sw), SCREEN_HEIGHT(sh) {
	
             // Main line down the middle of the ship
             Vector2 point0 = {SCREEN_WIDTH/2, SCREEN_HEIGHT/2 - length/2};
             Vector2 point1 = {SCREEN_WIDTH/2, SCREEN_HEIGHT/2 + length/2};

             // Perpendicular bar at the back of the ship
             Vector2 point2 = {SCREEN_WIDTH/2 - width/2, SCREEN_HEIGHT/2 + length/2};
             Vector2 point3 = {SCREEN_WIDTH/2 + width/2, SCREEN_HEIGHT/2 + length/2};

             // Side panels
             Vector2 point4 = point0;
             Vector2 point5 = {SCREEN_WIDTH/2 - width/2 - (width * 0.1), SCREEN_HEIGHT/2 + length/2 + (length * 0.1)};
             Vector2 point6 = point0;
             Vector2 point7 = {SCREEN_WIDTH/2 + width/2 + (width * 0.1), SCREEN_HEIGHT/2 + length/2 + (length * 0.1)};
    
             // Thruster
             Vector2 point8 = {SCREEN_WIDTH/2 - width/4, SCREEN_HEIGHT/2 + length/2};
             Vector2 point9 = {SCREEN_WIDTH/2 + width/4, SCREEN_HEIGHT/2 + length/2};
             Vector2 point10 = {SCREEN_WIDTH/2, SCREEN_HEIGHT/2 + 3*length/4};

             points = {point0, point1, point2, point3, point4, point5, point6, point7, point8, point9, point10};

             // Take the centre of rotation as the centre of the line running down the centre of the ship
             midpoint = {(point0.x + point1.x)/2 - (point0.x - point1.x)/4, (point0.y + point1.y)/2 - (point0.y - point1.y) / 4}; // Shift back to 1/4 along midline
	    
	 }

	 bool CollidedWithAsteroid(Asteroid& asteroid) {
	     
             for (const auto& point : points) {
	         // Calculate distance from point to midpoint of asteroid
	         float distance = std::sqrt(std::pow((point.x - asteroid.position.x), 2) + std::pow((point.y - asteroid.position.y), 2));
	         if (distance <= asteroid.radius) return true;
	     }

             return false;
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
	    if (midpoint.x > SCREEN_WIDTH + length) {
	        for (int i = 0; i < numPoints; i++) {
	            points[i].x -= SCREEN_WIDTH + length*2;
	        }
	    } else if (midpoint.x < -length) {
	        for (int i = 0; i < numPoints; i++) {
		    points[i].x += SCREEN_WIDTH + length*2;
		}
	    }

	    if (midpoint.y > SCREEN_HEIGHT + length) {
	        for (int i = 0; i < numPoints; i++) {
	            points[i].y -= SCREEN_HEIGHT + length*2;
	        }
	    } else if (midpoint.y < -length) {
	        for (int i = 0; i < numPoints; i++) {
		    points[i].y += SCREEN_HEIGHT + length*2;
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

GameState menu_screen(const int& screenWidthRef, const int& screenHeightRef) {
        
    GameState state = MENU;

    if(IsKeyDown(KEY_SPACE)) {
        state = PLAYING;
    }
    
    BeginDrawing();
    ClearBackground(BLACK);

    const char* text = "ASTEROIDS";
    const char* subText = "PRESS ENTER TO PLAY";
  
    int textFontSize = 60;
    int subTextFontSize = 20;
  
    int textWidth = MeasureText(text, textFontSize);
    int subTextWidth = MeasureText(subText, subTextFontSize);
   
    DrawText(text, (screenWidthRef/2) - (textWidth/2), screenHeightRef/2 - 70, textFontSize, WHITE);
    DrawText(subText, (screenWidthRef/2) - (subTextWidth/2), screenHeightRef/2, subTextFontSize, WHITE);
    
    EndDrawing();

    return state;
}

GameState next_level_screen(int& levelRef, const int& screenWidthRef, const int& screenHeightRef) {
    
    GameState state = NEXT_LEVEL;

    if(IsKeyDown(KEY_N)) {
        state = PLAYING;
    }

    BeginDrawing();
    ClearBackground(BLACK);

    std::string textStr = "LEVEL " + std::to_string(levelRef - 1) + " COMPLETE!";
    const char* text = textStr.c_str();
    const char* subText = "PRESS 'N' TO BEGIN NEXT LEVEL";
  
    int textFontSize = 40;
    int subTextFontSize = 20;
  
    int textWidth = MeasureText(text, textFontSize);
    int subTextWidth = MeasureText(subText, subTextFontSize);
   
    DrawText(text, (screenWidthRef/2) - (textWidth/2), screenHeightRef/2 - 50, textFontSize, WHITE);
    DrawText(subText, (screenWidthRef/2) - (subTextWidth/2), screenHeightRef/2, subTextFontSize, WHITE);

    EndDrawing();

    return state;
}

GameState game_over_screen(int& levelRef, const int& screenWidthRef, const int& screenHeightRef) {
    
    GameState state = GAME_OVER;

    if(IsKeyDown(KEY_R)) {
        state = PLAYING;
    }

    BeginDrawing();
    ClearBackground(BLACK);

    const char* text = "GAME OVER!";
    std::string subTextStr = "YOU REACHED LEVEL " + std::to_string(levelRef - 1) + ". PRESS 'R' TO RESTART";
    const char* subText = subTextStr.c_str();
  
    int textFontSize = 40;
    int subTextFontSize = 20;
  
    int textWidth = MeasureText(text, textFontSize);
    int subTextWidth = MeasureText(subText, subTextFontSize);
   
    DrawText(text, (screenWidthRef/2) - (textWidth/2), screenHeightRef/2 - 50, textFontSize, WHITE);
    DrawText(subText, (screenWidthRef/2) - (subTextWidth/2), screenHeightRef/2, subTextFontSize, WHITE);

    EndDrawing();

    return state;
}

int main() {

    // Define random uniform process
    std::random_device rd; // Create seed
    std::mt19937 gen(rd());
    
    std::uniform_real_distribution<> uniformDis(-1.0, 1.0);

    GameState currentState = MENU;
 
    // Initialization
    const int SCREEN_WIDTH = 1400;
    const int SCREEN_HEIGHT = 800;
    const int FPS = 60;
    const double pi = 3.141592653589793238;
    const double BULLET_SPEED = 20.0;
    
    int level = 1;

    std::vector<Bullet> bullets;
    // Wait a period between successive bullet object creations
    const int bFramesBetweenSpawn = 6;
    int bFramesUntilNextSpawn = 0;

    raylib::Color textColor(GREEN);
    raylib::Window w(SCREEN_WIDTH, SCREEN_HEIGHT, "Asteroids");
    
    SetTargetFPS(FPS);

    Player p = Player(SCREEN_WIDTH, SCREEN_HEIGHT);

    // The number of smaller asteroids created by destroying a larger one
    int asteroidSpawnFactor = 2;

    Asteroid roid1 = Asteroid({100, 100}, 1, 3.5, 3, 10, WHITE, SCREEN_WIDTH, SCREEN_HEIGHT);
    Asteroid roid2 = Asteroid({600, 200}, 2.5, -3, 3, 7, WHITE, SCREEN_WIDTH, SCREEN_HEIGHT);
    Asteroid roid3 = Asteroid({200, 500}, -2, -3, 3, 12, WHITE, SCREEN_WIDTH, SCREEN_HEIGHT);
    
    std::vector<Asteroid> asteroids = {roid1, roid2, roid3};

    // Main game loop
    while (!w.ShouldClose()) // Detect window close button or ESC key
    {
	if (currentState == MENU) {
	    currentState = menu_screen(SCREEN_WIDTH, SCREEN_HEIGHT);
	}

	if (currentState == NEXT_LEVEL) {
	    currentState = next_level_screen(level, SCREEN_WIDTH, SCREEN_HEIGHT);
	}

	if (currentState == GAME_OVER) {
	    currentState = game_over_screen(level, SCREEN_WIDTH, SCREEN_HEIGHT);
	}
        
	else if (currentState == PLAYING) {
	    if (IsKeyDown(KEY_SPACE)) {
	        //Create a new bullet if enough frames have passed since the last spawn
                if (bFramesUntilNextSpawn <= 0) {
	    
                    // Quick way to get the bullet x & y deltas
	            float bVelocX = p.deltaXShip * (BULLET_SPEED/p.length);
	            float bVelocY = p.deltaYShip * (BULLET_SPEED/p.length);
	    
	            bullets.push_back(Bullet({p.points[0].x, p.points[0].y}, bVelocX, bVelocY));
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

	    // Split or remove asteroids that have been hit by a bullet depending on their size
	    std::vector<Asteroid> newAsteroids = {};
	
	    for (auto it = bullets.begin(); it != bullets.end(); ++it) {
	        // Check if bullet is inside any asteroid
	        for (auto asteroid_it = asteroids.begin(); asteroid_it != asteroids.end();) {
	            if (asteroid_it->ContainsBullet(it->position)) {
		        // Remove the asteroid, spawn new smaller asteroids to resemble the
		        // breaking up of the old, larger one
		        if (asteroid_it->size > 1) {
		         
			    int newSize = asteroid_it->size - 1;
			
			    for (int i=0; i<asteroidSpawnFactor; i++){
			     
			        // Vary the position to within +- 1% of screen dimensions compared with original asteroid
			        Vector2 newPosition = {asteroid_it->position.x + (SCREEN_WIDTH/100) * uniformDis(gen) , asteroid_it->position.y + (SCREEN_HEIGHT/100) * uniformDis(gen)};
			        // Choose new x and y components of velocity within +-10% of the original asteroid's values
			        float newVelocX = asteroid_it->velocX + asteroid_it->velocX * uniformDis(gen) * 0.1;
			        float newVelocY = asteroid_it->velocY + asteroid_it->velocY * uniformDis(gen) * 0.1;
			        int newNumVertices = asteroid_it->numVertices;
			      
                                Asteroid newAsteroid = Asteroid(newPosition, newVelocX, newVelocY, newSize, newNumVertices, WHITE, SCREEN_WIDTH, SCREEN_HEIGHT);
			        newAsteroids.push_back(newAsteroid);
			    }
		        } 
		        asteroid_it = asteroids.erase(asteroid_it);
		    } else {
	                ++asteroid_it;
		    }
	        }
	    }

	    // Add new asteroids to the full list
	    asteroids.insert(asteroids.end(), newAsteroids.begin(), newAsteroids.end());

	    // Remove bullets that are off screen
	    bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
	        [SCREEN_WIDTH, SCREEN_HEIGHT](Bullet& bullet) {
	            return bullet.IsOffScreen(SCREEN_WIDTH, SCREEN_HEIGHT);
	        }), bullets.end());

            // Check if any asteroids have hit the player

	    // Draw all asteroids and check if any are hitting the player
            for (auto& asteroid : asteroids) {
	        if (p.CollidedWithAsteroid(asteroid)) {
		    currentState = GAME_OVER;		
		}
	        asteroid.Draw();
	    }

	    p.Draw();

	    textColor.DrawText("Level: " + std::to_string(level) + "", 10, 10, 20);	
	    EndDrawing();

	    // Check if asteroids vector is empty and move to next level if so
	    if (asteroids.empty()) {
	        level++;
                // Load in the next set of asteroids
                asteroids = {roid1, roid2, roid3};
                currentState = NEXT_LEVEL;
	    }
        }
    }

    return 0;
}
