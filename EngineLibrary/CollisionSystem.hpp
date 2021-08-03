#ifndef _collisionsystem
#define _collisionsystem
#include "jEngine.hpp"

struct Boundbox {
	Vector pos, size, center;
	Boundbox() {
		center = getCenter();
	}
	Boundbox(Vector pos, Vector size) {
		this->pos = pos;
		this->size = size;
		center = getCenter();
	}
	inline Vector &getCenter() {
		return center = pos + (size / 2.f);
	}

	static inline bool inBoundry(Boundbox &a, Boundbox &b) {
		auto
			ar = a.pos + a.size,
			br = b.pos + b.size;

		return a.pos.x < br.x &&
			ar.x > b.pos.x &&
			a.pos.y < br.y &&
			ar.y > b.pos.y;
	}
};
class Overlap {
public:
	Boundbox bb;
	Vector *targetPos;
	Overlap();
	virtual ~Overlap();
	virtual void collision(Overlap *o) {

	}
};


class Quad {
public:
	set<Overlap *> is;
	Boundbox bb;
	Quad *root, *tr, *br, *bl, *tl;
	unsigned int subDivisions;
	Quad(Boundbox bb, unsigned int subDivisions = 1, Quad *root = nullptr) {
		this->subDivisions = subDivisions;
		this->bb = bb;
		this->root = root;
		tr = br = bl = tl = nullptr;

	}

	inline Quad *newTr() {
		return tr = new Quad({ {bb.getCenter().x, bb.pos.y}, bb.size / 2.f }, subDivisions - 1, this);
	}
	inline Quad *newTl() {
		return tl = new Quad({ bb.pos, bb.size / 2.f }, subDivisions - 1, this);
	}
	inline Quad *newBr() {
		return br = new Quad({ bb.getCenter(), bb.size / 2.f }, subDivisions - 1, this);
	}
	inline Quad *newBl() {
		return bl = new Quad({ {bb.pos.x, bb.getCenter().y} ,  bb.size / 2.f }, subDivisions - 1, this);
	}



	static void build(Quad *q, set<Quad*> &totalQuads) {
		const int sub = q->subDivisions - 1;
		q->bb.getCenter();
		totalQuads.insert(q);
		if (sub >= 0) {
			if (!q->bl) {
				q->newBl();
				build(q->bl, totalQuads);
			}
			if (!q->br) {
				q->newBr();
				build(q->br, totalQuads);
			}
			if (!q->tl) {
				q->newTl();
				build(q->tl, totalQuads);
			}
			if (!q->tr) {
				q->newTr();
				build(q->tr, totalQuads);
			}
		}
	}
	static bool search(Quad *q, Overlap *i, set<Quad *> &qs, Quad *&parent) {
		if (Boundbox::inBoundry(q->bb, i->bb)) {
			qs.insert(q);
			if (q->subDivisions > 0) {
				int v = 0;
				v+=search(q->tr, i, qs, parent);
				v+=search(q->bl, i, qs, parent);
				v+=search(q->br, i, qs, parent);
				v+=search(q->tl, i, qs, parent);
				if (v > 2)
					parent = q;
			}
			else {
				parent = q;
				qs.insert(q);
			}
			return true;
		}
		return false;
	}
};

class Collision :
	public Singleton<Collision>{
public:
	Quad *root = nullptr;
	set<Quad *> tQuads;
	set<Overlap *> tOverlaps;
	Clock c;
	

	Collision() {
		root = new Quad({ { 0, 0}, {800, 600} }, 3);		
		Quad::build(root, tQuads);
	}
	void tick() {
		int col = 0;
		c.restart();
		map<Quad *, set<Overlap *>> os;
		for (auto &instA : tOverlaps) {
			if (instA->targetPos)
				instA->bb.pos = *instA->targetPos;
			set<Quad *> containedQuads;
			Quad *parent = nullptr;
			if (Quad::search(root, instA, containedQuads, parent)) {
				for (auto &quad : containedQuads) {
					if (quad) {
						std::set<Overlap*> &newCollisions = quad->is;
						for (auto &instB : newCollisions) {
							if (Boundbox::inBoundry(instA->bb, instB->bb)) {
								instA->collision(instB);
								instB->collision(instA);
								col++;
							}
						}
					}
				}
				parent->is.insert(instA);
			}
		}
		for (auto &q : tQuads) {
			//if (q->is.size()) {
			//	for (auto &i : q->is)
			//		drawDebugLine(q->bb.center, i->bb.getCenter(), Color::White);
			//	drawDebugQuad(q->bb.pos, q->bb.size, Color(255, 0, 0, 50));
			//}
			q->is.clear();
		}

		cout << c.getElapsedTime().asMilliseconds() << " " << col <<endl;
	}
};

Overlap::Overlap() {
	Collision::instance().tOverlaps.insert(this);
}
Overlap::~Overlap() {
	Collision::instance().tOverlaps.erase(this);
}
#endif // !_collisionsystem