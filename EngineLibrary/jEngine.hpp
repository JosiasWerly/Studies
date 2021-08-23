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
private:
	Var<Class> internalData;
public:
	virtual bool isValid() {
		return internalData.refs();
	}

	Class() {}
	virtual ~Class() {}
	virtual void tick() {}
};

class Actor;
class IComponent :
	public Class {
	friend class Actor;
protected:
	Actor *owner = nullptr;
	virtual void setup() {}
};

class Actor :
	public Class {
private:
	map<string, Class *> components;
public:
	Vector pos, dir;

	Actor() {
	}
	virtual ~Actor() {
	}
	virtual void tick() {
		for (auto c : components)
			c.second->tick();
	}

	template<class T> T *addComponent(string name, T *comp = new T) {
		if (components.count(name))
			delete components[name];
		components[name] = comp;
		comp->owner = this;
		comp->setup();
		return comp;
	}
	template<class T> T *delComponent(string name) {
		if (components.count(name))
			delete components[name];
	}
	template<class T> T *getComponent(string name) {
		if (components.count(name))
			return components[name];
		return nullptr;
	}

};

class GarbageSystem :
	public Singleton<GarbageSystem> {
public:
	list<Var<Class>> objects, toDelete;
	void tick() {
		for (auto &vo : objects){
			if (!*vo && vo.refs() <= 1)
				toDelete.push_back(vo);
			else
				vo->tick();
		}
		for (auto &d : toDelete)
			objects.remove(d);
		toDelete.clear();
	}
};
template<class T> Var<T> instantiate(T *i) {
	Var<T> out;
	*out = i;
	GarbageSystem::instance().objects.push_back(out.as<Class>());
	return out;
}
void destroy(Var<Class> &trg) {
	GarbageSystem::instance().toDelete.push_back(trg);
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
