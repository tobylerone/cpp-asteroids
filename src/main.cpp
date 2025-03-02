#include <iostream>
#include <raylib-cpp.hpp>
#include <cmath>
#include <algorithm>
#include <random>
#include "asteroid.h"
#include "player.h"
#include "bullet.h"
#include "game_constants.h"

enum GameStatus {
    MENU,
    PLAYING,
    NEXT_LEVEL,
    GAME_OVER
};
    
struct GameState {
    
    int level = 1;
    int bulletFramesUntilNextSpawn = 0;

    GameStatus status;

    // Create random number generators for uniform real distribution and gaussian distribution
    // TODO: Move this somewhere else
    std::random_device rd; // Create seed
    std::mt19937 gen;
    std::uniform_real_distribution<> uniformDis;

    //std::vector<Bullet> bullets = {};
    //Player player;
    //std::vector<Asteroid> asteroids = {}; // TODO: Length is already known so no need to declare with dynamic length
    
    GameState(): status(MENU), rd(), gen(rd()), uniformDis(0.0, 1.0) {}
};

std::vector<Asteroid> create_asteroids(GameState& state, int numAsteroids) {
    
    std::vector<Asteroid> asteroids {};

    Vector2 position;
    float xSpeed;
    float ySpeed;
    // TODO: This should not be hardcoded here
    int size = 3;
    int numVertices;
    
    for (int i = 0; i < numAsteroids; i++) {
	position = {state.uniformDis(state.gen) * GC::SCREEN_WIDTH, state.uniformDis(state.gen) * GC::SCREEN_HEIGHT};
	// Select speed from uniform random distribution between -3 and 3
	xSpeed = (state.uniformDis(state.gen) * 6 - 3);
        ySpeed = (state.uniformDis(state.gen) * 6 - 3);
	Asteroid ast = Asteroid(position, xSpeed, ySpeed, 3, 12, WHITE, GC::SCREEN_WIDTH, GC::SCREEN_HEIGHT);
        asteroids.push_back(ast);
    }

    return asteroids;    

}

void menu_screen(GameState& state) {

    if(IsKeyDown(KEY_S)) {
        state.status = PLAYING;
    }
    
    BeginDrawing();
    ClearBackground(BLACK);

    const char* text = "ASTEROIDS";
    const char* subText = "PRESS 'S' TO PLAY";
  
    int textFontSize = 60;
    int subTextFontSize = 20;
  
    int textWidth = MeasureText(text, textFontSize);
    int subTextWidth = MeasureText(subText, subTextFontSize);
   
    DrawText(text, (GC::SCREEN_WIDTH/2) - (textWidth/2), GC::SCREEN_HEIGHT/2 - 70, textFontSize, WHITE);
    DrawText(subText, (GC::SCREEN_WIDTH/2) - (subTextWidth/2), GC::SCREEN_HEIGHT/2, subTextFontSize, WHITE);
    
    EndDrawing();
}

void next_level_screen(GameState& state) {
    
    if(IsKeyDown(KEY_N)) {
        state.status = PLAYING;
    }

    BeginDrawing();
    ClearBackground(BLACK);

    std::string textStr = "LEVEL " + std::to_string(state.level - 1) + " COMPLETE!";
    const char* text = textStr.c_str();
    const char* subText = "PRESS 'N' TO BEGIN NEXT LEVEL";
  
    int textFontSize = 40;
    int subTextFontSize = 20;
  
    int textWidth = MeasureText(text, textFontSize);
    int subTextWidth = MeasureText(subText, subTextFontSize);
   
    DrawText(text, (GC::SCREEN_WIDTH/2) - (textWidth/2), GC::SCREEN_HEIGHT/2 - 50, textFontSize, WHITE);
    DrawText(subText, (GC::SCREEN_WIDTH/2) - (subTextWidth/2), GC::SCREEN_HEIGHT/2, subTextFontSize, WHITE);

    EndDrawing();
}

bool game_over_screen(GameState& state) {
    
    if(IsKeyDown(KEY_R)) {
        state.status = PLAYING;
    	// New game started. Indicates that game variables must be reset at next game loop iteration
	return true;
    }

    BeginDrawing();
    ClearBackground(BLACK);

    const char* text = "GAME OVER!";
    std::string subTextStr = "YOU REACHED LEVEL " + std::to_string(state.level - 1) + ". PRESS 'R' TO RESTART";
    const char* subText = subTextStr.c_str();
  
    int textFontSize = 40;
    int subTextFontSize = 20;
  
    int textWidth = MeasureText(text, textFontSize);
    int subTextWidth = MeasureText(subText, subTextFontSize);
   
    DrawText(text, (GC::SCREEN_WIDTH/2) - (textWidth/2), GC::SCREEN_HEIGHT/2 - 50, textFontSize, WHITE);
    DrawText(subText, (GC::SCREEN_WIDTH/2) - (subTextWidth/2), GC::SCREEN_HEIGHT/2, subTextFontSize, WHITE);

    EndDrawing();

    // New game not started
    return false;
}

void playing_screen(GameState& state, Player& p, std::vector<Bullet>& bullets, std::vector<Asteroid>& asteroids, raylib::Color& textColor) {

	if (IsKeyDown(KEY_SPACE)) {
	            //Create a new bullet if enough frames have passed since the last spawn
                    if (state.bulletFramesUntilNextSpawn <= 0) {
	    
                        // Quick way to get the bullet x & y deltas
	                float bVelocX = p.getDeltaXShip() * (GC::BULLET_SPEED/p.getLength());
	                float bVelocY = p.getDeltaYShip() * (GC::BULLET_SPEED/p.getLength());
	    
	                bullets.push_back(Bullet({p.getPoints()[0].x, p.getPoints()[0].y}, bVelocX, bVelocY));
	                state.bulletFramesUntilNextSpawn = GC::BULLET_FRAMES_PER_SPAWN;
	             }
	        }

	        // Decrement the frame count between bullet spawning
	        if (state.bulletFramesUntilNextSpawn > 0) state.bulletFramesUntilNextSpawn--;

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
	                if (asteroid_it->ContainsBullet(it->getPosition())) {
		            // Remove the asteroid, spawn new smaller asteroids to resemble the
		            // breaking up of the old, larger one
		            if (asteroid_it->getSize() > 1) {
		         
			        int newSize = asteroid_it->getSize() - 1;
			
			        for (int i=0; i<GC::ASTEROID_SPAWN_FACTOR; i++){
			     
			            // Vary the position to within +- 1% of screen dimensions compared with original asteroid
			            Vector2 newPosition = {asteroid_it->getPosition().x + (GC::SCREEN_WIDTH/100) * state.uniformDis(state.gen) , asteroid_it->getPosition().y + (GC::SCREEN_HEIGHT/100) * state.uniformDis(state.gen)};
			            // Choose new x and y components of velocity within +-10% of the original asteroid's values
			            float newVelocX = asteroid_it->getVelocX() + asteroid_it->getVelocX() * state.uniformDis(state.gen) * 0.1;
			            float newVelocY = asteroid_it->getVelocY() + asteroid_it->getVelocY() * state.uniformDis(state.gen) * 0.1;
			            int newNumVertices = asteroid_it->getNumVertices();
			      
                                    Asteroid newAsteroid = Asteroid(newPosition, newVelocX, newVelocY, newSize, newNumVertices, WHITE, GC::SCREEN_WIDTH, GC::SCREEN_HEIGHT);
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
	            [](Bullet& bullet) {
	                return bullet.IsOffScreen(GC::SCREEN_WIDTH, GC::SCREEN_HEIGHT);
	            }), bullets.end());

                // Check if any asteroids have hit the player

	        // Draw all asteroids and check if any are hitting the player
                for (auto& asteroid : asteroids) {
	            if (p.CollidedWithAsteroid(asteroid)) {
		        state.status = GAME_OVER;		
		    }
	            asteroid.Draw();
	        }

	        p.Draw();

	        textColor.DrawText("Level: " + std::to_string(state.level) + "", 10, 10, 20);	
	        EndDrawing();

	        // Check if asteroids vector is empty and move to next level if so
	        if (asteroids.empty()) {
	            state.level++;
                    // Load in the next set of asteroids
                    asteroids = create_asteroids(state, 3);
		    state.status = NEXT_LEVEL;
	        }
}

int main() {

    GameState state;

    raylib::Color textColor(GREEN);
    raylib::Window w(GC::SCREEN_WIDTH, GC::SCREEN_HEIGHT, "Asteroids");
    
    SetTargetFPS(GC::FPS);
   
    // TODO: Can't find how to make Player p; work. Think something to do with how class is defined
    Player p = Player();
    std::vector<Bullet> bullets;// = {};
    
    std::vector<Asteroid> asteroids;// = create_asteroids(state, 3);
   
    bool isNewGame = true;

    // Main game loop
    while (!w.ShouldClose()) // Detect window close button or ESC key
    {
	if (isNewGame == true) {
	    p = Player();
    	    bullets = {};
	    asteroids = create_asteroids(state, 3);
	    state.level = 1;

	    isNewGame = false;	    
	}

	switch (state.status)
	{
	    case MENU:
	        menu_screen(state);
		break;
	    case NEXT_LEVEL:
	        next_level_screen(state);
                break;
	    case GAME_OVER:
	        isNewGame = game_over_screen(state);
	        break;
	    case PLAYING:
		playing_screen(state, p, bullets, asteroids, textColor);
		break;
        }
    }

    return 0;
}
