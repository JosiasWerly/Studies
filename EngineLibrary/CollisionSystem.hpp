#ifndef _collisionsystem
#define _collisionsystem
#include "jEngine.hpp"

namespace E {

	class Quad;
	static list<struct Instance *> insts;
	static list<Quad *> quads;

	struct Rect {
		Vector startPoint, endPoint, center;
		Rect() {}
		Rect(Vector startPoint, Vector endPoint) : 
			startPoint(startPoint), 
			endPoint(endPoint){
			center = startPoint + (endPoint - startPoint) / 2.f;
		}
		static inline bool inBoundry(Rect &a, Vector &p) {
			return
				p.x >= a.startPoint.x &&
				p.y >= a.startPoint.y &&
				p.x <= a.endPoint.x &&
				p.y <= a.endPoint.y;
		}
	};
	struct Instance {
		Vector &pos;
		float radius;
		Instance(Vector &pos, float radius) : 
			pos(pos), 
			radius(radius){	
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
		Rect rc;
		Quad *tr, *br, *bl, *tl;
		unsigned int subDivisions;
		Quad(Rect rect, unsigned int subDivisions = 1) {
			this->subDivisions = subDivisions;
			rc = rect;
			tr = br = bl = tl = nullptr;
			quads.push_back(this);
		}
	};
	static inline bool inBoundry(Rect &a, Vector &p) {
	}
	static void tick(Quad *q, Instance *n) {
		
	}
	static void insert(Quad *q, Instance *n) {
		if (!n)
			return;
		float minSize = 50;
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
		}
	}
	static void mountTree(Quad *q) {
		if (q->subDivisions - 1 != 0) {
			auto center = q->rc.startPoint + (q->rc.endPoint - q->rc.startPoint) / 2.f;

			q->br = new Quad({ center, q->rc.endPoint }, q->subDivisions - 1);
			mountTree(q->br);

			q->bl = new Quad({ {q->rc.startPoint.x, center.y} ,  {center.x, q->rc.endPoint.y} }, q->subDivisions - 1);
			mountTree(q->bl);

			q->tr = new Quad({ {center.x, q->rc.startPoint.y}, {q->rc.endPoint.x , center.y} }, q->subDivisions - 1);
			mountTree(q->tr);

			q->tl = new Quad({ q->rc.startPoint, center }, q->subDivisions - 1);
			mountTree(q->tl);
		}
	}
};

#endif // !_collisionsystem
