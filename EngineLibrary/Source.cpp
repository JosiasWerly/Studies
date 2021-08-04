#include "jEngine.hpp"
#include "CollisionSystem.hpp"







class Actor : 
	public Class,
	public OverlapInstance {
public:
	Vector pos, dir;
	CircleShape c;
	Color w;
	Actor() {
		pos = { rand() % 600 + 100, rand() % 500 + 50 };
		dir = { rand() % 3 + 1, rand() % 3 + 1 };
		dir = dir.rotate(rand() % 180);
		c.setRadius(rand()%10+5);
		c.setFillColor(Color::Magenta);
		pushDraw(&c);
		
		OverlapInstance::config(Boundbox({}, { c.getRadius() * 2, c.getRadius() * 2 }),
			&pos);
		
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

		c.setFillColor(w);
		c.setPosition(pos);
	}
	void collision(OverlapInstance *o) {
		c.setFillColor(Color::Red);
	}
};


int main() {
	e.col.addQuadTree("a", { 0, 0 }, { 400, 600 }, 2);
	e.col.addQuadTree("b", { 200, 0 }, { 400, 600 }, 2);

	Actor *a;
	for (size_t i = 0; i < 100; i++) {
		 a = (Actor*)(Class*)instantiate(new Actor);
		 a->setLayer(true, i % 2 ? "a" : "b");
		 a->w = i % 2 ? Color::Magenta : Color::Cyan;
	}

	
	while (true) {
		a->pos = mousePos;
		e.tick();
	}
	return 0;
}