#pragma once
#ifndef _utils
#define _utils

#include <SFML/Graphics.hpp>
using namespace sf;
class Vector{
public:
	double x = 0, y = 0;


	
	Vector(){}
	Vector(int x, int y) : x(double(x)), y(double(y)) {
		
	}
	Vector(double x, double y) : x(x), y(y){
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
	Vector operator+(double f) {
		return { x + f, y + f };
	}
	Vector operator-(double f) {
		return { x - f, y - f };
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


template<class T>class Var {
protected:
	friend class Var;
	template<class t>struct TData {
	public:
		unsigned int refCount = 1;
		t *val = nullptr;
		~TData() {
			delete val;
			val = 0;
		}
	};
	typename typedef TData<T> Data;
	
	Data *dt = nullptr;

	inline void _unRef() {
		if (--dt->refCount == 0) {
			delete dt;
			dt = nullptr;
		}
	}
	inline void _assing(Data *o) {
		_unRef();
		dt = o;
		dt->refCount++;
	}
public:
	inline unsigned int refs() {
		return dt ? dt->refCount : 0;
	}
	~Var() {
		_unRef();
	}
	Var(const Var &cpy) {
		this->dt = cpy.dt;
		this->dt->refCount++;
	}
	Var &operator=(Var &o) {
		_assing(o.dt);
		return *this;
	}
	bool operator==(const Var &v) const {
		return dt == v.dt;
	}

	
	Var(T *init = nullptr) {
		dt = new Data;
		dt->val = init;
	}

	T *operator=(T *v) {		
		return dt->val = v;
	}
	operator T *() {
		return dt->val;
	}
	T *operator->() {
		return dt->val;
	}

};


#endif // !_utils
