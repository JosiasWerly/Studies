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
#include "CollisionSystem.hpp"
#include "jwindow.hpp"



class Class {
public:
	Class() {}
	virtual ~Class() {}
	virtual void tick() {}
};
class GarbageSystem :
	public Singleton<GarbageSystem> {
public:
	list<Var<Class>> objects, toDelete;
	void tick() {
		for (auto &vo : objects){
			if (!vo && vo.refs() <= 1)
				toDelete.push_back(vo);
			else
				vo->tick();
		}

		for (auto &d : toDelete)
			objects.remove(d);
		toDelete.clear();
	}
};
Var<Class> instantiate(Class *i) {
	Var<Class> out;
	out = i;
	GarbageSystem::instance().objects.push_back(out);
	return out;
}



class Engine :
	public Singleton<Engine> {
public:
	World &world = World::instance();
	GarbageSystem &gs = GarbageSystem::instance();
	Navigation &nav = Navigation::instance();
	Collision &col = Collision::instance();
	Engine() {
		nav.config({ 10, 10 });
		nav.meshByImage(World::instance().getImage());
	}
	void tick() {
		gs.tick();
		col.tick();
		win.tick();
	}
};


static Engine &e = Engine::instance();
#endif // !_engine
