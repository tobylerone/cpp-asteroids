#ifndef GAMECONSTANTS_H
#define GAMECONSTANTS_H

namespace GameConstants {
    // constexpr is compile-time constant to avoid multiple definitions during linkage
    static constexpr int SCREEN_WIDTH = 1400;
    static constexpr int SCREEN_HEIGHT = 800;
    static constexpr int FPS = 60;
    static constexpr double pi = 3.141592653589793238;
    static constexpr double BULLET_SPEED = 20.0;
    static constexpr int BULLET_FRAMES_PER_SPAWN = 6;
    // The number of smaller asteroids created by destroying a larger one
    static constexpr int ASTEROID_SPAWN_FACTOR = 2;
}

// Create an alias
namespace GC = GameConstants;

#endif // GAMECONSTANTS_H
