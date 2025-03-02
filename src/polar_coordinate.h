#ifndef POLARCOORDINATE_H
#define POLARCOORDINATE_H

#include <raylib-cpp.hpp>

class PolarCoordinate {
    public:
        PolarCoordinate(float mag, float the);
        
        // Default constructor
        PolarCoordinate() : magnitude(0), theta(0) {}
	
	Vector2 to_cartesian(Vector2 origin);
        void setMagnitude(float mag);
	void setTheta(float the);
    private:
        float magnitude; //pixels
	float theta; //pixels	
};

#endif // POLARCOORDINATE_H
