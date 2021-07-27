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




class Object {
public:
	virtual void tick() {}
};
class GarbageSystem :
	public Singleton<GarbageSystem> {
public:
	list<Var<Object>> objects;
	list<Var<Object> *> toDelete;

	void tick() {
		for (auto &o : objects) {
			if (o.refs() <= 1)
				toDelete.push_back(&o);
			else
				o->tick();
		}
		for (auto d : toDelete) {
			delete *d;
			objects.remove(*d);
		}
		toDelete.clear();
	}
};
template<class T> Var<Object> instantiate(T *init = new T) {
	auto &gs = GarbageSystem::instance();
	Object *newObj = init;
	gs.objects.emplace_back(newObj);
	return gs.objects.back();
}
void destroy(Var<Object> &target) {
	auto &gs = GarbageSystem::instance();
	gs.toDelete.emplace_back(&target);
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
