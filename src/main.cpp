#include <iostream>
#include <raylib-cpp.hpp>
#include <cmath>

class Bullet {
    public:
        Vector2 position;
	float deltaX;
	float deltaY;
	float speed;
	float radius;
	Color color;

	Bullet(Vector2 pos, float dx, float dy, float s, float r, Color c): position(pos), deltaX(dx), deltaY(dy), speed(s), radius(r), color(c) {}
	
	void Draw() {
	    // Update the positions
            position.x += deltaX;
	    position.y += deltaY;
	    DrawCircleV(position, radius, color);
	}
};

int main() {
    
    // Initialization
    int screenWidth = 1000;
    int screenHeight = 700;
    int fps = 60;

    const double pi = 3.141592653589793238;

    float pMaxSpeed = 8;
    float pSpeed = 0; // Pixels per frame
    float pDrag = 0.05; // Pixels per frame
    //float pGradient = 99999999;
    int pLength = 50;
    int pWidth = 20;
    int pNumPoints = 11;

    std::vector<Bullet> bullets;
    int bRadius = 2;
    float bSpeed = 10.0;
    Color bColor = RED;
    // Wait a period between successive bullet object creations
    int bFramesBetweenSpawn = 10;
    int bFramesUntilNextSpawn = 0;

    // Main line down the middle of the ship
    Vector2 pPoint0 = {screenWidth/2, screenHeight/2 - pLength/2};
    Vector2 pPoint1 = {screenWidth/2, screenHeight/2 + pLength/2};

    // Perpendicular bar at the back of the ship
    Vector2 pPoint2 = {screenWidth/2 - pWidth/2, screenHeight/2 + pLength/2};
    Vector2 pPoint3 = {screenWidth/2 + pWidth/2, screenHeight/2 + pLength/2};

    // Side panels
    Vector2 pPoint4 = pPoint0;
    Vector2 pPoint5 = {screenWidth/2 - pWidth/2 - (pWidth * 0.1), screenHeight/2 + pLength/2 + (pLength * 0.1)};
    Vector2 pPoint6 = pPoint0;
    Vector2 pPoint7 = {screenWidth/2 + pWidth/2 + (pWidth * 0.1), screenHeight/2 + pLength/2 + (pLength * 0.1)};
    
    // Thruster
    Vector2 pPoint8 = {screenWidth/2 - pWidth/4, screenHeight/2 + pLength/2};
    Vector2 pPoint9 = {screenWidth/2 + pWidth/4, screenHeight/2 + pLength/2};
    Vector2 pPoint10 = {screenWidth/2, screenHeight/2 + 3*pLength/4};

    Vector2 pPoints[] = {pPoint0, pPoint1, pPoint2, pPoint3, pPoint4, pPoint5, pPoint6, pPoint7, pPoint8, pPoint9, pPoint10};

    // Take the centre of rotation as the centre of the line running down the centre of the ship
    //Vector2 pMidpoint = {(pPoint0.x + pPoint1.x)/2, (pPoint0.y + pPoint1.y)/2};
    Vector2 pMidpoint = {(pPoint0.x + pPoint1.x)/2 - (pPoint0.x - pPoint1.x)/4, (pPoint0.y + pPoint1.y)/2 - (pPoint0.y - pPoint1.y) / 4}; // Shift back to 1/4 along midline
    // Radians to rotate by per frame (0.02 radians)
    float theta = (2 * pi / 50);

    raylib::Color textColor(LIGHTGRAY);
    raylib::Window w(screenWidth, screenHeight, "Asteroids");
    
    SetTargetFPS(fps);

    // Main game loop
    while (!w.ShouldClose()) // Detect window close button or ESC key
    {
        // Update variables
	if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_LEFT)) {
	    // Rotate p by chosen number of radians
	    // Translate points so that midpoint is at origin
	    Vector2 pPointsOrigin[pNumPoints];

	    for (int i = 0; i < pNumPoints; i++){
	    	pPointsOrigin[i] = {pPoints[i].x - pMidpoint.x, pPoints[i].y - pMidpoint.y};
	    }
	    //Vector2 pPoint1Origin = {pPoint1.x - pMidpoint.x, pPoint1.y - pMidpoint.y};
	    //Vector2 pPoint2Origin = {pPoint2.x - pMidpoint.x, pPoint2.y - pMidpoint.y};

	    Vector2 pPointsOriginRot[pNumPoints];
	    // Rotate points using appropriate rotation matrix relative to the origin
	    if (IsKeyDown(KEY_RIGHT)) {
	        // Clockwise rotation
		for (int i = 0; i < pNumPoints; i++) {
	            pPointsOriginRot[i] = {cos(theta)*pPointsOrigin[i].x -sin(theta)*pPointsOrigin[i].y, sin(theta)*pPointsOrigin[i].x + cos(theta)*pPointsOrigin[i].y};
		}
	        //pPoint1OriginRot = {cos(theta)*pPoint1Origin.x + -sin(theta)*pPoint1Origin.y, sin(theta)*pPoint1Origin.x + cos(theta)*pPoint1Origin.y};
	        //pPoint2OriginRot = {cos(theta)*pPoint2Origin.x + -sin(theta)*pPoint2Origin.y, sin(theta)*pPoint2Origin.x + cos(theta)*pPoint2Origin.y};
	    } else {
	    	// Anticlockwise rotation
		for (int i = 0; i < pNumPoints; i++) {
	            pPointsOriginRot[i] = {cos(theta)*pPointsOrigin[i].x + sin(theta)*pPointsOrigin[i].y, -sin(theta)*pPointsOrigin[i].x + cos(theta)*pPointsOrigin[i].y};
		}
	    }
	    // Translate points back to absolute locations
	    for (int i = 0; i < pNumPoints; i++) {
	        pPoints[i] = {pPointsOriginRot[i].x + pMidpoint.x, pPointsOriginRot[i].y + pMidpoint.y};
	    }
	}

	// Move ship forwards
	if (IsKeyDown(KEY_UP)) {
	    // Instantaneously set speed back to pMaxSpeed (no acceleration)
	    pSpeed = pMaxSpeed;
	}
	
	// Calculate the direction of travel (essentially the gradient from point1 to point0
	float deltaXShip = pPoints[0].x - pPoints[1].x;
	float deltaYShip = pPoints[0].y - pPoints[1].y;
	float speedByShipLength = pSpeed / pLength;	

	float deltaX;
	float deltaY;

	if (deltaXShip != 0.0) {
	    //pGradient = deltaYShip/deltaXShip;
	    deltaX = deltaXShip * speedByShipLength;
	    deltaY = deltaYShip * speedByShipLength;
	} else {
	    // Infinite gradient. No x delta
	    deltaY = -pSpeed;
	    deltaX = 0;
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

	for (int i = 0; i < pNumPoints; i++) {
	    pPoints[i].x += deltaX;
	    pPoints[i].y += deltaY;
	}

	if (IsKeyDown(KEY_SPACE)) {
	    //Create a new bullet if enough frames have passed since the last spawn
            if (bFramesUntilNextSpawn <= 0) {
	    
                // Quick way to get the bullet x & y deltas
	        float bDeltaX = deltaX * (bSpeed/pSpeed);
	        float bDeltaY = deltaY * (bSpeed/pSpeed);
	    
	        bullets.push_back(Bullet({pPoints[0].x, pPoints[0].y}, bDeltaX, bDeltaY, bSpeed, bRadius, bColor));
	        bFramesUntilNextSpawn = bFramesBetweenSpawn;
	    }
	}
	// Recalculate the midpoint	    
        //pMidpoint = {(pPoints[0].x + pPoints[1].x)/2, (pPoints[0].y + pPoints[0].y)/2};
    	pMidpoint = {(pPoints[0].x + pPoints[1].x)/2 - (pPoints[0].x - pPoints[1].x)/4, (pPoints[0].y + pPoints[1].y)/2 - (pPoints[0].y - pPoints[1].y) / 4}; // Shift back to 1/4 along midline

        // Decay the speed
	pSpeed -= (pSpeed > 0 ? pDrag : 0);

	// Decrement the frame count between bullet spawning
	if (bFramesUntilNextSpawn > 0) bFramesUntilNextSpawn--;

        // DRAW------------------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(BLACK);

	// Draw all bullets
	for (auto& bullet : bullets) {
	    bullet.Draw();
	}
        
	//DrawRectangle(pPosX, pPosY, size, size, BLUE);
	DrawTriangle(pPoints[10], pPoints[9], pPoints[8], IsKeyDown(KEY_UP) ? ORANGE : BLACK);
	DrawLine(pPoints[0].x, pPoints[0].y, pPoints[1].x, pPoints[1].y, BLACK);
	DrawLine(pPoints[2].x, pPoints[2].y, pPoints[3].x, pPoints[3].y, WHITE);
	DrawLine(pPoints[4].x, pPoints[4].y, pPoints[5].x, pPoints[5].y, WHITE);
	DrawLine(pPoints[6].x, pPoints[6].y, pPoints[7].x, pPoints[7].y, WHITE);
	
	textColor.DrawText("X: " + std::to_string(pMidpoint.x) + " Y: " + std::to_string(pMidpoint.y) + "", 10, screenHeight-20, 20);	
	EndDrawing();
    }

    return 0;
}
