#include <raylib-cpp.hpp>

class Bullet {
    public:
	Bullet(Vector2 pos, float dx, float dy);
	void Draw();
	Vector2 getPosition() const;
	bool IsOffScreen(int SCREEN_WIDTH, int SCREEN_HEIGHT);
    private:
        Vector2 position;
	float velocX;
	float velocY;
	float radius = 2;
	Color color = WHITE;
};
