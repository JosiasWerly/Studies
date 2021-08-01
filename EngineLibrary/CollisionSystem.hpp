#ifndef _collisionsystem
#define _collisionsystem
#include "jEngine.hpp"

namespace E {
	static int collTests = 0;
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
		Quad *root, *tr, *br, *bl, *tl;
		unsigned int subDivisions;
		Quad(Rect rect, unsigned int subDivisions = 1, Quad *root = nullptr) {
			this->subDivisions = subDivisions;
			rc = rect;
			tr = br = bl = tl = nullptr;
			quads.push_back(this);
		}

		inline Quad* newTr() {
			return tr = new Quad({ {rc.getCenter().x, rc.pos.y}, rc.size / 2.f }, subDivisions - 1, this);
		}
		inline Quad* newTl() {
			return tl = new Quad({ rc.pos, rc.size/2.f }, subDivisions - 1, this);
		}
		inline Quad* newBr() {
			return br = new Quad({ rc.getCenter(), rc.size/2.f }, subDivisions - 1, this);
		}
		inline Quad* newBl() {
			return bl = new Quad({ {rc.pos.x, rc.getCenter().y} ,  rc.size / 2.f }, subDivisions - 1, this);
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

	static void insert(Quad *q, Instance *n) {
		if (inBoundry(q->rc, n->r)) {
			const int sub = q->subDivisions - 1;
			if (sub != 0) {
				auto &c = q->rc.getCenter();
				auto &p = n->r.pos;
				auto ep = n->r.pos + n->r.size;
				Quad *tq = q;

				if (p.x < c.x) {
					if (p.y < c.y) {
						if (!q->tl)
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
				if (ep.x < c.x) {
					if (ep.y < c.y) {
						if (!q->tl)
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
					if (ep.y < c.y) {
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
			else {
				for (auto &s : q->is){
					if (n != s && inBoundry(s->r, n->r)) {
						collTests++;
						s->collision(n);
						n->collision(s);
					}
				}
				q->is.push_back(n);
			}
		}
	}
	/*static Quad *search(Quad *q, Instance *i) {
		if (inBoundry(q->rc, i->r)) {
			if (q->subDivisions != 0) {
				Quad *out = nullptr;
				if (out = search(q->tr, i))
					return out;
				else if (out = search(q->br, i))
					return out;
				if (out = search(q->bl, i))
					return out;
				else if (out = search(q->tl, i))
					return out;
			}
			else {
				return q;
			}
		}
		return nullptr;
	}*/

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
	static void tick(Quad *q) {
		collTests = 0;
		for (auto &i : E::insts) {
			list<Quad *> qs;
			if (search(q, i, qs)) {
				for (auto &qq : qs){
					if (qq) {
						auto &isl = qq->is;
						for (auto ins : isl) {
							if (inBoundry(ins->r, i->r)) {
								i->collision(ins);
								ins->collision(i);
								collTests++;
								//drawDebugLine(i->r.getCenter(), ins->r.getCenter(), Color::Red);
							}
						}
						qq->is.push_back(i);
					}
				}
			}
		}
		for (auto &q : quads) {
			if (q->is.size()) {
				drawDebugQuad(q->rc.pos, q->rc.size, Color(int(q->rc.pos.x)%255, int(q->rc.pos.y) % 255, 0, 100));
				//for (auto &i : q->is){
				//	drawDebugLine(q->rc.center, i->r.getCenter(), Color::White);
				//}
			}
			q->is.clear();
		}

	}
};

#endif // !_collisionsystem

//static void insert(Quad *q, Instance *n) {
//	if (inBoundry(q->rc, n->r)) {
//		const int sub = q->subDivisions - 1;
//		if (sub != 0) {
//			auto &c = q->rc.getCenter();
//			auto &p = n->r.pos;
//			auto ep = n->r.pos + n->r.size;
//			Quad *tq = q;
//
//			if (p.x < c.x) {
//				if (p.y < c.y) {
//					if (!q->tl)
//						q->newTl();
//					insert(q->tl, n);
//				}
//				else {
//					if (!q->bl)
//						q->newBl();
//					insert(q->bl, n);
//				}
//			}
//			else {
//				if (p.y < c.y) {
//					if (!q->tr)
//						q->newTr();
//					insert(q->tr, n);
//				}
//				else {
//					if (!q->br)
//						q->newBr();
//					insert(q->br, n);
//				}
//			}
//			if (ep.x < c.x) {
//				if (ep.y < c.y) {
//					if (!q->tl)
//						q->newTl();
//					insert(q->tl, n);
//				}
//				else {
//					if (!q->bl)
//						q->newBl();
//					insert(q->bl, n);
//				}
//			}
//			else {
//				if (ep.y < c.y) {
//					if (!q->tr)
//						q->newTr();
//					insert(q->tr, n);
//				}
//				else {
//					if (!q->br)
//						q->newBr();
//					insert(q->br, n);
//				}
//			}
//		}
//		else {
//			for (auto &s : q->is) {
//				if (n != s && inBoundry(s->r, n->r)) {
//					collTests++;
//					s->collision(n);
//					n->collision(s);
//				}
//			}
//			q->is.push_back(n);
//		}
//	}
//}