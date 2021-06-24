#pragma once
#ifndef _utils
#define _utils

#include <SFML/Graphics.hpp>
using namespace sf;
class Vector{
public:
	double x = 0, y = 0;

	
	constexpr Vector(){}	
	constexpr Vector(int x, int y) : x(double(x)), y(double(y)) {}
	constexpr Vector(double x, double y) : x(x), y(y){}
	
	inline operator sf::Vector2f() const { return { float(x), float(y) };	}


	double length() {
		return sqrtf(powf(x, 2)+ powf(y, 2));
	}
	Vector normalize() {
		Vector out = *this;
		if (out.x < 0.00001 && out.y < 0.00001)
			return { 1, 0 };
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

	Vector operator/(double f) {
		return { x / f, y / f };
	}
	Vector operator*(double f) {
		return { x * f, y * f };
	}
	

	double angle() {
		const double deg2rad = 180.f / 3.14;
		return atan2f(y, x) *deg2rad;
	}
	void rotate(double angle) {
		const double deg2rad = 180.f / 3.14;
		const double theta = angle/deg2rad;

		double cs = cos(theta);
		double sn = sin(theta);


		double 
			_x = this->x * cs - this->y * sn,
			_y = this->x * sn + this->y * cs;
		x = _x;
		y = _y;
	}
	bool operator==(Vector o) const{
		return false;

	}

	Vector floor() {
		return { int(x), int(y) };
	}

	//static double rotate(angle)
	
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
		/*const double deg2rad = 180 / 3.14159265359;
		double an = a.angle() / deg2rad;
		double le = a.length();

		double curAn = atan2f(a.y, a.x) + an;
		return {
			sinf(curAn) * le,
			cosf(curAn) * le,
		};*/
		const double deg2rad = 180 / 3.14159265359;
		const double theta = angle / deg2rad;
		
		double cs = cosf(theta);
		double sn = sinf(theta);
		
		
		double
			_x = a.x * cs - a.y * sn,
			_y = a.x * sn + a.y * cs;
		return { _x, _y };
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
