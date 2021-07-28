#include "jEngine.hpp"
class Actor {
public:
	Vector pos;
	CircleShape c;
	Actor() {
		pos = { rand() % 600 + 100, rand() % 500 + 50 };
		c.setRadius(5);
		c.setOrigin({ 5, 5 });
		c.setFillColor(Color::Magenta);
		pushDraw(&c);
	}
	virtual ~Actor() {
		popDraw(&c);
	}

	void tick() {

		c.setPosition(pos);
	}
};
static list<Actor*> acts;

list<Actor *> orderByDistance(list<Actor*> ls, Vector location) {
	list<Actor *> out;	
	while(ls.size()){
		std::pair<Actor *, double> p = {nullptr, 0};	
		for (auto &a : ls){
			double nDis = Vector::distance(location, a->pos);
			if (nDis >= p.second)
				p = { a, nDis };
		}
		out.push_front(p.first);
		ls.remove(p.first);
	}
	return out;
}





namespace Experimental {
	const int links = 2;
	struct Link{
		Actor *a, *b;
	};
	class System{
	public:
		list<Link *> lnks;

		void fix() {
			auto b = *lnks.begin(),
				e = *lnks.end();
			Vector::distance(b->a->pos, b->b->pos);

		}
		System() {
			auto i = acts.begin();
			while (i != acts.end()){
				auto nLink = new Link;				
				if (i != acts.end()) {
					nLink->a = (*i);
					i++;
				}
				if (i != acts.end()) {
					nLink->b = (*i);
					i++;
				}
				lnks.push_back(nLink);
			}
		}
		void tick() {
			for (auto &l : lnks){
				drawDebugLine(l->a->pos, l->b->pos, Color::Red);
			}
		}
	};
};







int main() {
	for (size_t i = 0; i < 10; i++)
		acts.push_back(new Actor);

	Experimental::System s;
	while (true){
		s.tick();
		for (auto &a : acts)
			a->tick();
		e.tick();
	}
	return 0;
}