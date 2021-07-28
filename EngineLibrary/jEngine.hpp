#pragma once
#ifndef _engine
#define _engine

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <list>
#include <vector>
#include <iostream>
using namespace sf;
using namespace std;

#include "Utils.h"
#include "NavSystem.hpp"
#include "jwindow.hpp"




class Class {
private:
	template<class T> friend Class *instantiate(T *init);
	friend void destroy(Class *target);
	Var<Class> var;
public:
	Class() {}
	virtual ~Class() {}
};
class GarbageSystem :
	public Singleton<GarbageSystem> {
public:
	list<Var<Class>> objects;
	//list<Var<Class>*> toDelete, toUnroot;

	void tick() {
		//for (auto &o : objects) {
		//	if (o.refs() <= 1)
		//		toUnroot.push_back(&o);
		//}
		//for (auto d : toDelete) {
		//	delete *d;
		//	*d = nullptr;
		//}
		//for (auto r : toUnroot) {
		//	delete *r;
		//	r = nullptr;
		//	objects.remove(*r);
		//}
		//toDelete.clear();
		//toUnroot.clear();
	}
};

template<class T> Class* instantiate(T *init = new T) {
	auto &gs = GarbageSystem::instance();
	Class *newObj = init;
	newObj->var = newObj;
	gs.objects.emplace_back(newObj->var);
	return newObj;
}
void destroy(Class *target) {
	auto &gs = GarbageSystem::instance();
	//gs.toDelete.emplace_back(&target);
}

class Engine :
	public Singleton<Engine> {
public:
	GarbageSystem &gs = GarbageSystem::instance();
	Navigation &nav = Navigation::instance();
	World &world = World::instance();
	Engine() {
		nav.config({ 10, 10 });
		nav.meshByImage(World::instance().getImage());
	}
	void tick() {
		gs.tick();
		win.tick();
	}
};


static Engine &e = Engine::instance();
#endif // !_engine
