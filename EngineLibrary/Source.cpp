#include "jEngine.hpp"


class Actor : 
	public Object{
public:
	Vector pos;
	RectangleShape rec;
	Actor() {
		pos = { 200, 200 };
		rec.setSize({ 10, 10 });
		rec.setOrigin({ 5, 5 });
		rec.setFillColor(Color::Magenta);
		pushDraw(&rec);
	}
	virtual ~Actor() {
		popDraw(&rec);
	}
	
	void tick() {
		rec.setPosition(pos);
	}
};




int main() {
	auto a = instantiate<Actor>();

	while (true){
		e.tick();
	}
	return 0;
}