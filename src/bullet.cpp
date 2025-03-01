#include "bullet.h"
#include <iostream>

Bullet::Bullet(Vector2 pos, float dx, float dy): position(pos), velocX(dx), velocY(dy) {}
	
void Bullet::Draw() {
    // Update the positions
    position.x += velocX;
    position.y += velocY;
    DrawCircleV(position, radius, color);
}

Vector2 Bullet::getPosition() const {
    return position;
}

bool Bullet::IsOffScreen(int SCREEN_WIDTH, int SCREEN_HEIGHT) {
     return (position.x < 0 || position.x > SCREEN_WIDTH || position.y < 0 || position.y > SCREEN_HEIGHT);
}

