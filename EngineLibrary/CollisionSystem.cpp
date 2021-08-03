#ifndef _collisionsystem
#define _collisionsystem
#include "jEngine.hpp"

namespace E {


	

	static inline bool inBoundry(Rect &a, Rect &b) {
		auto 
			ar = a.pos + a.size,
			br = b.pos + b.size;

		return a.pos.x < br.x &&
			ar.x > b.pos.x &&
			a.pos.y < br.y &&
			ar.y > b.pos.y;
	}
	static void build(Quad *q) {
		const int sub = q->subDivisions - 1;
		q->rc.getCenter();
		if (sub >= 0) {
			if (!q->bl) {
				q->newBl();
				build(q->bl);
			}
			if (!q->br) {
				q->newBr();
				build(q->br);
			}
			if (!q->tl) {
				q->newTl();
				build(q->tl);
			}
			if (!q->tr) {
				q->newTr();
				build(q->tr);
			}

		}
	}
	static bool search(Quad *q, Instance *i, list<Quad *> &qs) {
		if (inBoundry(q->rc, i->r)) {			
			if(q->subDivisions > 1){
				if (search(q->tr, i, qs));
				else if (search(q->bl, i, qs));
				
				if (search(q->br, i, qs));
				else if (search(q->tl, i, qs));
			}
			else if (q->subDivisions == 1){ //leafCheck
				search(q->tr, i, qs);
				search(q->bl, i, qs);
				search(q->br, i, qs);
				search(q->tl, i, qs);				
			}
			else
				qs.push_back(q);
			return true;
		}
		else
			return false;
	}
};
class Quad {
public:
	list<Instance *> is;
	Rect rc;
	Quad *root, *tr, *br, *bl, *tl;
	unsigned int subDivisions;
	Quad(Rect rect, unsigned int subDivisions = 1, Quad *root = nullptr) {
		this->subDivisions = subDivisions;
		rc = rect;
		tr = br = bl = tl = nullptr;
		quads.push_back(this);
	}

	inline Quad *newTr() {
		return tr = new Quad({ {rc.getCenter().x, rc.pos.y}, rc.size / 2.f }, subDivisions - 1, this);
	}
	inline Quad *newTl() {
		return tl = new Quad({ rc.pos, rc.size / 2.f }, subDivisions - 1, this);
	}
	inline Quad *newBr() {
		return br = new Quad({ rc.getCenter(), rc.size / 2.f }, subDivisions - 1, this);
	}
	inline Quad *newBl() {
		return bl = new Quad({ {rc.pos.x, rc.getCenter().y} ,  rc.size / 2.f }, subDivisions - 1, this);
	}
};


struct Boundbox {
	Vector *pos, size, center;
	void set(Vector *pos, Vector size) {
		this->pos = pos;
		this->size = size;
		center = getCenter();
	}
	inline Vector &getCenter() {
		return center = pos + (size / 2.f);
	}
};
class Overlap{
public:
	Boundbox bb;

	Overlap(){}
	virtual ~Overlap() {}
	virtual void collision(Overlap *o) {}
};




class CollisionSystem : 
	public Singleton<CollisionSystem>{
public:
	E::Quad *root = nullptr;
	CollisionSystem() {
		root = new E::Quad({ { 0, 0}, {800, 600} }, 3);
		E::build(root);
	}
	void tick() {
		for (auto &i : E::insts) {
			list<E::Quad *> qs;
			if (E::search(root, i, qs)) {
				for (auto &qq : qs) {
					if (qq) {
						auto &isl = qq->is;
						for (auto ins : isl) {
							if (inBoundry(ins->r, i->r)) {
								i->collision(ins);
								ins->collision(i);
							}
						}
						qq->is.push_back(i);
					}
				}
			}
		}
		for (auto &q : E::quads)
			q->is.clear();
	}
};
#endif // !_collisionsystem