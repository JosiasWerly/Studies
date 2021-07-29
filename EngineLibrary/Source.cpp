#include "jEngine.hpp"

struct Stats {
	unsigned int f = 0, col = 0;
	Clock ms;
};
Stats st;
class Actor {
public:
	Vector pos, dir;
	CircleShape c;
	Actor() {
		pos = { rand() % 600 + 100, rand() % 500 + 50 };
		dir = { rand() % 3+1, rand() % 3 + 1 };
		dir = dir.rotate(rand() % 180);
		c.setRadius(5);
		c.setOrigin({ 5, 5 });
		c.setFillColor(Color::Magenta);
		pushDraw(&c);
	}
	virtual ~Actor() {
		popDraw(&c);
	}
	void tick() {
		//pos = pos + dir;
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
	void onBeginOverlap(Actor* o) {
		c.setFillColor(Color::Red);
	}
};
static vector<Actor*> acts;

vector<Actor *> orderByDistance(vector<Actor*> ls, Vector location) {
	vector<Actor *> out;
	while(ls.size()){
		std::pair<Actor *, double> p = {nullptr, 0};	
		for (auto &a : ls){
			double nDis = Vector::distance(location, a->pos);
			if (nDis >= p.second)
				p = { a, nDis };
		}
		out.push_back(p.first);
		out.erase(out.begin());
	}
	return out;
}





namespace Experimental {	

	
	void distanceBased() {
		for (size_t x = 0; x < acts.size(); x++){
			for (size_t y = x+1; y < acts.size(); y++) {
				double r = acts[x]->c.getRadius();
				if (Vector::distance(acts[x]->pos, acts[y]->pos) <= r) {
					acts[x]->onBeginOverlap(acts[y]);
					acts[y]->onBeginOverlap(acts[x]);
					st.col++;
				}
			}
		}
	}


	const unsigned int k = 2;
	struct Link{
		double w = 9999;
		Link *n[k];
		Actor *a[k];
	};

	vector<Link *> ls;
	int id = 0;
	void swapTargets(Link *a, Link *b, int ida, int idb) {
		auto bf = a->a[ida];
		a->a[ida] = b->a[idb];
		b->a[idb] = bf;
	}
	pair<double, int> minorDistance(Link *trg, Actor *a) {
		pair<double, int> out = {999999, -1};
		for (auto ta : trg->a){
			double nd = Vector::distance(a->pos, ta->pos);
			if (nd < out.first) {
				i = nd;
			}

		}
		return out;
	}
	void linkBased() {
		if (!ls.size()) {
			unsigned int maxSize = acts.size() / k;
			ls.resize(maxSize);
			int aId = 0, lId = 0;
			for (auto &l : ls) {				
				Link *ln = new Link;
				ln->a[0] = acts[aId++];
				ln->a[1] = acts[aId++];
				l = ln;
			}
			for (int i = 0; i < maxSize; i++) {
				ls[i]->n[0] = ls[i - 1 < 0 ? maxSize - 1 : i - 1 % maxSize];
				ls[i]->n[1] = ls[i + 1 % maxSize - 1];
				ls[i]->w = Vector::distance(ls[i]->a[0]->pos, ls[i]->a[1]->pos);
			}
		}
		else {
			for (size_t i = 0; i < ls.size(); i++){
				auto &ia = ls[i];
				ia->w = Vector::distance(ia->a[0]->pos, ia->a[1]->pos);
				for (auto &n : ia->n){
					

					//if (ia->w <= nd) {
					//	swapTargets(ia, n, 0, 0);
					//	ia->w = nd;
					//	break;
					//}
				}
			}

			for (auto l : ls)
				drawDebugLine(l->a[0]->pos, l->a[1]->pos, Color::Red);
		}
	}
};
int main() {
	for (size_t i = 0; i < 6; i++)
		acts.push_back(new Actor);

	while (true){

		if (st.f++ == 0)
			st.ms.restart();

		for (int x = 0; x < acts.size(); x++) {
			if (x == 0) {
				acts[0]->pos = mousePos;
			}
			acts[x]->tick();

		}
		Experimental::linkBased();

		if (st.f >= 60) {
			cout << st.ms.getElapsedTime().asMilliseconds() << ", " << st.col << endl;
			st.f = 0;
			st.col = 0;
		}


		
		e.tick();
	}
	return 0;
}