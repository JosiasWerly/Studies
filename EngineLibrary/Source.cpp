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
};
class GarbageSystem : 
	public Singleton<GarbageSystem> {
public:
	list<Var<Object>> objects;
};

template<class T> T *instantiate(T *init) {
	//auto &i = ObjectSystem::instance();
	//Object *o = init;
	//i.objects.push_back(o);
	return init;
}
void destroy(Object* obj) {
	//auto &i =ObjectSystem::instance();
	//i.toDestroy.push_back(obj);
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
	{
		Var<int> a, b;
		Var<float> c;
		a = b;
		a = new int(123);
		a.clear();

		c = a.as<float>();

		
	}

	Engine::instance();

	while (true){
		win.tick();
		system("pause");
	}
	return 0;
}