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


class Object{
public:
	virtual void tick() {
	}
};
class ObjectSystem : 
	public Singleton<ObjectSystem> {
public:
	list<Object*> objects, toDestroy;
	void tick() {
		for (auto o : objects){
			o->tick();
		}
		for (auto d : toDestroy) {
			objects.remove(d);
			delete d;
			d = nullptr;
		}
		toDestroy.clear();
	}
};

template<class T> T *instantiate(T *init) {
	auto &i = ObjectSystem::instance();
	Object *o = init;
	i.objects.push_back(o);
	return init;
}
void destroy(Object* obj) {
	auto &i =ObjectSystem::instance();
	i.toDestroy.push_back(obj);
}



class Engine : 
	public Singleton<Engine>{
public:
	Navigation &nav = Navigation::instance();
	World &world = World::instance();
	Engine() {
		nav.config({ 10, 10 });
		nav.meshByImage(World::instance().getImage());
	}
};





int main() {
	Engine::instance();

	while (true){
		win.tick();
		system("pause");
	}
	return 0;
}