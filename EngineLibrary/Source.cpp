#include "jEngine.hpp"
#include "CollisionSystem.hpp"






class Perception : 
	public IComponent, 
	public OverlapInstance{
public:
	CircleShape shape;
	~Perception() {
		popDraw(&shape);
	}
	void setup() {
		setLayer(0xF);
		setTarget(&owner->pos);
		setRadius(50);
		shape.setRadius(50);
		shape.setOrigin(50, 50);
		shape.setFillColor(Color(255, 255, 255, 50));
		pushDraw(&shape);
	}
	template<class T>void bind(T *trg) {
		if (trg) {
			onBeginOverlap.bind(new MFnx<T, const TLayer &, OverlapInstance *>(trg, &T::OnGainPerception));
			onEndOverlap.bind(new MFnx<T, const TLayer &, OverlapInstance *>(trg, &T::OnLosePerception));
		}
	}
	void tick() {
		shape.setPosition(owner->pos);
	}
};
class Body :
	public IComponent {
public:
	RectangleShape shape;
	~Body() {
		popDraw(&shape);
	}
	void setup() {
		shape.setSize({ 10, 10 });
		shape.setOrigin(5, 5);
		pushDraw(&shape);
	}
	void tick() {
		shape.setPosition(owner->pos);
	}
};
class Agent : 
	public Actor{
public:
	Perception *p;
	Body *b;
	
	Agent() {
		p = addComponent<Perception>("perception");
		p->bind<Agent>(this);
		b = addComponent<Body>("body");

		pos = { rand() % 600 + 100, rand() % 500 + 50 };
		//c.restart();
	}
	void OnGainPerception(const TLayer &l, OverlapInstance *o) {
		b->shape.setFillColor(Color::Red);
	}
	void OnLosePerception(const TLayer &l, OverlapInstance *o) {
		b->shape.setFillColor(Color::Blue);
	}
	void tick() {
		Actor::tick();
	}
};


int main() {
	Var<Agent> a;	
	for (size_t i = 0; i < 10; i++) {
		 a = instantiate<Agent>(new Agent);
	}
	Clock c;
	c.restart();

	while (true) {
		if (c.getElapsedTime().asSeconds() > 5) {
			destroy(a);
		}
		a->pos = mousePos;
		e.tick();
	}
	return 0;
}