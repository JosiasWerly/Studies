#pragma once
#ifndef _utils
#define _utils

#include <SFML/Graphics.hpp>
using namespace sf;
class Vector{
public:
	double x = 0, y = 0;


	
	constexpr Vector(){}	
	constexpr Vector(int x, int y) : x(double(x)), y(double(y)) {
		
	}
	constexpr Vector(double x, double y) : x(x), y(y){
	}
	
	inline operator sf::Vector2f() const { return { float(x), float(y) };	}

	void sanitaze() {
		x = ::round(x * 1000.0) / 1000.0;
		y = ::round(y * 1000.0) / 1000.0;
	}
	double length() {
		return sqrtf(powf(x, 2)+ powf(y, 2));
	}
	Vector normalize() {
		Vector out = *this;
		out = out / out.length();
		sanitaze();
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

	Vector operator/(double f) {
		return { x / f, y / f };
	}
	Vector operator*(double f) {
		return { x * f, y * f };
	}
	

	inline double angle() {
		const double deg2rad = 180.f / 3.1415926;
		double out = atan2f(y, x) * deg2rad;
		out = out <= 0 ? out + 360 : out;
		//return atan2f(y, x) * deg2rad;
		return out;
		
	}
	inline Vector rotate(double angle) {
		const double deg2rad = 180.f / 3.1415926;
		double d = this->angle();
		double _alpha = this->angle() + angle;
		_alpha /= deg2rad;
		double l = length();

		Vector v { cos(_alpha) * l, sin(_alpha) * l };
		v.sanitaze();
		return v;
	}
	bool operator==(Vector o) const{
		return x == o.x && y == o.y;
	}
	bool operator<(Vector o) const{
		return x < o.x && y < o.y;
	}




	Vector floor() {
		return { int(x), int(y) };
	}
	Vector round() {
		return { ::round(x), ::round(y) };
	}

	static double dot(Vector a, Vector b) {
		double len = (a * b).length();
		if (len < 0.0001)
			return 0;
		return (a.x * b.x + a.y * b.y) / len;
	}
	static double distance(Vector a, Vector b) {
		Vector out = b - a;
		return out.length();
	}
	static Vector direction(Vector a, Vector b) {
		Vector out = b - a;
		return out.normalize();
	}
	static Vector lerp(Vector a, Vector b, double alpha) {
		return a + ((b - a) * alpha);
	}
	static Vector rotate(Vector a, double angle) {
		a.rotate(angle);
		return a;
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
