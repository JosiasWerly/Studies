#pragma once
#ifndef _utils
#define _utils

#include <SFML/Graphics.hpp>
using namespace sf;
class Vector{
public:
	float x = 0, y = 0;

	
	constexpr Vector(){}	
	constexpr Vector(int x, int y) : x(float(x)), y(float(y)) {}
	constexpr Vector(float x, float y) : x(x), y(y){}
	
	inline operator sf::Vector2f() const { return { x, y };	}


	float length() {
		return sqrtf(x * x + y * y);
	}
	Vector normalize() {
		Vector out = *this;
		out = out / out.length();
		return out;
	}

	Vector operator+(Vector o) {
		return {x + o.x, y + o.y};
	}
	Vector operator-(Vector o) {
		return { x - o.x, y - o.y };
	}
	Vector operator/(Vector o) {
		return { x / o.x, y / o.y };
	}
	Vector operator*(Vector o) {
		return { x * o.x, y * o.y };
	}

	Vector operator/(float f) {
		return { x / f, y / f };
	}
	bool operator==(Vector o) const{
		return false;

	}


	static float distance(Vector a, Vector b) {
		Vector out = b - a;
		return out.length();
	}
	static Vector direction(Vector a, Vector b) {
		Vector out = b - a;
		return out.normalize();
	}
};


template<class T>class Singleton{
protected:
	Singleton() {}
	virtual ~Singleton(){}
public:
	static T& instance() {
		static T i;
		return i;
	}

};

#endif // !_utils
