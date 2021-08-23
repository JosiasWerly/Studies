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
		return sqrt(pow(x, 2)+ pow(y, 2));
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
		double out = atan2(y, x) * deg2rad;
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


struct Data{
	void *ptr = nullptr;
	unsigned int refCount = 0;
};
template<class T>class Var {
protected:
	friend class Var;
	
	Data *dt = nullptr;
	T **ptr = nullptr;

	inline void _unRef() {
		if (--dt->refCount == 0) {
			delete dt;
			dt = nullptr;
		}
	}
	inline void _setRef(Data *o) {
		dt = o;
		ptr = (T **)&dt->ptr;
		dt->refCount++;
	}
	inline void _changeRef(Data *o) {
		_unRef();
		_setRef(o);
	}

	Var(Data *d, bool) {
		_setRef(d);
	}
public:
	inline unsigned int refs() {
		return dt ? dt->refCount : 0;
	}
	inline void free() {
		delete ptr;
		dt->ptr = nullptr;
	}

	Var(T *init = nullptr) {
		_setRef(new Data{ init });
	}
	Var(const Var &cpy) {
		_setRef(cpy.dt);
	}
	Var(Var &&cpy) {
		_setRef(cpy.dt);
	}
	~Var() {
		_unRef();
	}
	Var &operator=(Var &o) {
		_changeRef(o.dt);
		return *this;
	}
	Var &operator=(Var o) {
		_changeRef(o.dt);
		return *this;
	}


	bool operator==(const Var &v) const {
		return dt == v.dt;
	}
	template<class t> Var<t> as() {
		return Var<t>(dt, 1);
	}

	T *&operator*() {
		return *ptr;
	}
	T *operator->() {
		return *ptr;
	}


};




template<class ...TArgs>
struct IBind{
protected:
	IBind() {
	}
public:	
	virtual void call(TArgs... args) {
	}
};

template<class ...TArgs>
struct Fnx : IBind <TArgs...> {
public:
	typedef void (*TFnx)(TArgs...);
	TFnx fnx;
	Fnx(TFnx fnx) :
		fnx(fnx) {
	}
	void call(TArgs... args) override {
		fnx(args...);
	}
};

template<class TObj, class ...TArgs>
struct MFnx : public IBind <TArgs...>{
	typedef void (TObj::*TMFnx)(TArgs...);

	TObj *obj;
	TMFnx mfnx;
	MFnx(TObj *obj, TMFnx mfnx) :
		obj(obj),
		mfnx(mfnx){
	}
	void call(TArgs... args) override{
		(obj->*mfnx)(args...);
	}
};

template<class ...TArgs>
class Delegate{
	typedef IBind<TArgs...> TFnx;
	list<TFnx *> binds;
public:
	void bind(TFnx *fn) {
		binds.push_back(fn);
	}
	void broadcast(TArgs ...args) {
		for (auto b : binds)
			b->call(args...);
	}
};



#endif // !_utils
