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



class Class;

class GarbageSystem :
	public Singleton<GarbageSystem> {
public:
	list<Var<Class>> objects, toDelete;
	void tick() {
		for (auto &vo : objects){
			if (vo.refs() <= 1)
				toDelete.push_back(vo);
		}

		for (auto &d : toDelete)
			objects.remove(d);
		toDelete.clear();
	}
};
class Class {
public:
	Class() {}
	virtual ~Class() {}
};

class Engine :
	public Singleton<Engine> {
public:
	World &world = World::instance();
	GarbageSystem &gs = GarbageSystem::instance();
	Navigation &nav = Navigation::instance();
	CollisionSystem &colSys = CollisionSystem::instance();
	Engine() {
		nav.config({ 10, 10 });
		nav.meshByImage(World::instance().getImage());
	}
	void tick() {
		gs.tick();
		colSys.tick();
		win.tick();
	}
};


static Engine &e = Engine::instance();
#endif // !_engine
