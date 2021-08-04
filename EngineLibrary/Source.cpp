#include "jEngine.hpp"
#include "CollisionSystem.hpp"







class Actor : 
	public Class,
	public Overlap {
public:
	//Boundbox b;
	Vector pos, dir;
	CircleShape c;
	Actor() {
		pos = { rand() % 600 + 100, rand() % 500 + 50 };
		dir = { rand() % 3 + 1, rand() % 3 + 1 };
		dir = dir.rotate(rand() % 180);
		c.setRadius(rand()%10+5);
		c.setFillColor(Color::Magenta);
		pushDraw(&c);
		
		Overlap::bb.size = { c.getRadius()*2,  c.getRadius() *2};
		Overlap::targetPos = &pos;
	}
	virtual ~Actor() {
		popDraw(&c);
	}
	void tick() {
		pos = pos + dir;
		if (pos.x < 0)
			pos.x = 800;
		else if (pos.x > 800)
			pos.x = 0;

		if (pos.y < 0)
			pos.y = 600;
		else if (pos.y > 600)
			pos.y = 0;

		c.setFillColor(Color::Green);
		c.setPosition(pos);
	}
	void collision(Overlap *o) {
		c.setFillColor(Color::Red);
	}

};
int main() {
	
	Actor *a;
	for (size_t i = 0; i < 800; i++) {
		//Vector initPos;
		//if (it != e.col.tQuads.end()) {
		//	initPos = (*it)->bb.center;
		//	it++;
		//}
		//else {
		//	it = begIt;
		//}
		 a = (Actor*)(Class*)instantiate(new Actor);
		 //a->pos = initPos - a->bb.size/2.f;
	}
	while (true) {
		a->pos = mousePos;
		e.tick();
	}
	return 0;
}