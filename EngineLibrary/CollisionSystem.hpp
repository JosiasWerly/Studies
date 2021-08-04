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
class OverlapInstance {
	Vector *targetPos;
	Boundbox bb;
	set<string> overlapLayer;
	list<OverlapInstance *> overlaping;

	friend class Quad;
	friend class QuadTree;
public:
	void config(Boundbox bb, Vector *pos) {
		this->bb = bb;
		this->targetPos = pos;
	}
	inline set<string> &getLayers() {
		return overlapLayer; 
	}
	void setLayer(bool enable, string layerName);


	inline void preProcess() {
		if (targetPos)
			bb.pos = *targetPos;
		overlaping.clear();
	}
	virtual void collision(OverlapInstance *o) {
		overlaping.push_back(o);
	}
};
class Quad {
public:
	set<OverlapInstance *> is;
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
	static bool search(Quad *q, OverlapInstance *i, set<Quad *> &qs) {
		if (Boundbox::inBoundry(q->bb, i->bb)) {
			if (q->subDivisions > 0) {
				search(q->tr, i, qs);
				search(q->bl, i, qs);
				search(q->br, i, qs);
				search(q->tl, i, qs);
			}
			else {				
				qs.insert(q);
			}
			return true;
		}
		return false;
	}
};
struct QuadTree{
	Quad *root = nullptr;
	set<Quad *> tQuads;
	set<OverlapInstance *> tOverlaps;
	Clock c;

	void buildTree(Vector pos, Vector size, unsigned int subDivisions){
		for (auto &q : tQuads)
			delete q;
		tQuads.clear();

		root = new Quad({ pos, size }, subDivisions);
		Quad::build(root, tQuads);
	}
	inline void processTree() {
		int col = 0;
		c.restart();
		map<Quad *, set<OverlapInstance *>> os;
		for (auto &instA : tOverlaps) {
			instA->preProcess();
			set<Quad *> containedQuads;
			if (Quad::search(root, instA, containedQuads)) {
				if (containedQuads.size() == 1) {
					auto &quad = *containedQuads.begin();
					for (auto &instB : quad->is) {
						if (Boundbox::inBoundry(instA->bb, instB->bb)) {
							instA->collision(instB);
							instB->collision(instA);
							col++;
						}
					}
					quad->is.insert(instA);
				}
				else {
					set<OverlapInstance *> checked;
					for (auto &quad : containedQuads) {
						for (auto &instB : quad->is) {
							if (Boundbox::inBoundry(instA->bb, instB->bb) && !checked.count(instB)) {
								instA->collision(instB);
								instB->collision(instA);
								checked.insert(instB);
								col++;
							}
						}
						quad->is.insert(instA);
					}
				}
			}
		}


		for (auto &q : tQuads) {
			if (q->subDivisions == 0)
				drawDebugQuad(q->bb.pos, q->bb.size, Color(100, 0, 0, 50));
			q->is.clear();
		}

		cout << c.getElapsedTime().asMilliseconds() << " " << col << endl;
	}
};
class Collision :
	public Singleton<Collision>{
public:
	map<string, QuadTree *> quadTrees;

	Collision() {
	}

	void addQuadTree(string profile, Vector pos, Vector size, unsigned int subDivisions) {
		QuadTree *qt = new QuadTree;
		qt->buildTree(pos, size, subDivisions);
		quadTrees[profile] = qt;
	}
	QuadTree *getQuadTree(string profile) {
		if (quadTrees.count(profile)) {
			return quadTrees[profile];
		}
		return nullptr;
	}
	void tick() {
		for (auto &q : quadTrees)
			q.second->processTree();
	}
};
void OverlapInstance::setLayer(bool enable, string layerName) {
	auto &c = Collision::instance();
	if (enable && !overlapLayer.count(layerName)) {
		c.getQuadTree(layerName)->tOverlaps.insert(this);
		overlapLayer.insert(layerName);

	}
	else if (overlapLayer.count(layerName)) {
		c.getQuadTree(layerName)->tOverlaps.erase(this);
		overlapLayer.erase(layerName);
	}
}
#endif // !_collisionsystem