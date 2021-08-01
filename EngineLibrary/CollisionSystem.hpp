#ifndef _collisionsystem
#define _collisionsystem
#include "jEngine.hpp"

namespace E {

	class Quad;
	static list<struct Instance *> insts;
	static list<Quad *> quads;

	struct Rect {
		Vector pos, size, center;
		Rect() {}
		Rect(Vector pos, Vector size) :
			pos(pos),
			size(size){
			center = getCenter();			
		}
		inline Vector &getCenter() {
			return center = pos + (size /2.f);
		}
	};
	struct Instance {
		Rect r;
		Instance(){
			insts.push_back(this);
		}
		virtual ~Instance() {
			insts.remove(this);
		}
		virtual void collision(Instance *o) {
		}
	};
	class Quad {
	public:
		list<Instance *> is;

		Rect rc;
		Quad *tr, *br, *bl, *tl;
		unsigned int subDivisions;
		Quad(Rect rect, unsigned int subDivisions = 1) {
			this->subDivisions = subDivisions;
			rc = rect;
			tr = br = bl = tl = nullptr;
			quads.push_back(this);
		}

		inline Quad* newTr() {
			return tr = new Quad({ {rc.getCenter().x, rc.pos.y}, rc.size / 2.f }, subDivisions - 1);
		}
		inline Quad* newTl() {
			return tl = new Quad({ rc.pos, rc.size/2.f }, subDivisions - 1);
		}
		inline Quad* newBr() {
			return br = new Quad({ rc.getCenter(), rc.size/2.f }, subDivisions - 1);
		}
		inline Quad* newBl() {
			return bl = new Quad({ {rc.pos.x, rc.getCenter().y} ,  rc.size / 2.f }, subDivisions - 1);
		}
	};
	
	static inline bool inBoundry(Rect &a, Rect &b) {
		auto 
			ar = a.pos + a.size,
			br = b.pos + b.size;
		return a.pos.x < br.x &&
			ar.x > b.pos.x &&
			a.pos.y < br.y &&
			ar.y > b.pos.y;
	}
	static void insert(Quad *q, Instance *n) {
		if (inBoundry(q->rc, n->r)) {
			const int sub = q->subDivisions - 1;
			q->is.push_back(n);
			if (sub != 0) {
				auto &c = q->rc.getCenter();
				auto &p = n->r.pos;
				Quad *tq = q;
				if (p.x < c.x) {
					if (p.y < c.y) {
						if(!q->tl)
							q->newTl();
						insert(q->tl, n);
					}
					else {
						if (!q->bl)
							q->newBl();
						insert(q->bl, n);
					}
				}
				else {
					if (p.y < c.y) {
						if (!q->tr)
							q->newTr();
						insert(q->tr, n);
					}
					else {
						if (!q->br)
							q->newBr();
						insert(q->br, n);
					}
				}
			}
		}
		else {
			q->is.remove(n);
		}

		/*
		if (Rect::inBoundry(q->rc, n->pos)) {
			if (q->subDivisions - 1 != 0) {
				auto &center = q->rc.center;
				if (n->pos.y > center.y) {
					if (n->pos.x > center.x) {
						if (!q->br)
							q->br = new Quad({ center, q->rc.endPoint }, q->subDivisions - 1);
						insert(q->br, n);
					}
					else {
						if (!q->bl)
							q->bl = new Quad({ {q->rc.startPoint.x, center.y} ,  {center.x, q->rc.endPoint.y} }, q->subDivisions - 1);
						insert(q->bl, n);
					}
				}
				else {
					if (n->pos.x > center.x) {
						if (!q->tr)
							q->tr = new Quad({ {center.x, q->rc.startPoint.y}, {q->rc.endPoint.x , center.y} }, q->subDivisions - 1);
						insert(q->tr, n);
					}
					else {
						if (!q->tl)
							q->tl = new Quad({ q->rc.startPoint, center }, q->subDivisions - 1);
						insert(q->tl, n);
					}
				}
			}
		}*/
	}

	static void tick(Quad *q) {
		insert(q, insts.front());
		for (auto &q : quads) {
			drawDebugQuad(q->rc.pos, q->rc.size, Color(255, 0, 0, 20));
			for (auto &i : q->is){
				drawDebugLine(q->rc.center, i->r.pos, Color::White);
			}
		}

	}
};

#endif // !_collisionsystem
