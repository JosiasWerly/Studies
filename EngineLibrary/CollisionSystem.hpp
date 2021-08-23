#ifndef _collisionsystem
#define _collisionsystem
#include "jEngine.hpp"

typedef unsigned long TLayer;

class OverlapTarget {
public:
	Vector *trg = nullptr;
	float radius = 0;
};
struct OverlapData{
	TLayer layer;
	class OverlapInstance *inst;

	OverlapData &operator=(const OverlapData &o) {
		this->layer = o.layer;
		this->inst = o.inst;
		return *this;
	}
	bool operator==(const OverlapData &o) const {
		return inst == o.inst;
	}
};

class OverlapInstance {
	OverlapTarget _inst;
	TLayer _layer = 0;
public:
	vector<OverlapData> accOverlap, overlaping;
	Delegate<const TLayer &, OverlapInstance *> onBeginOverlap, onEndOverlap;

	OverlapInstance();
	~OverlapInstance();
	void setTarget(Vector *trg);
	void setRadius(float radius);
	void setLayer(TLayer newLayer);

	inline bool isOverlapin(OverlapInstance &b) {
		return Vector::distance(*_inst.trg, *b._inst.trg) <= _inst.radius;
	}
	virtual void overlaped(OverlapData dt) {
		accOverlap.push_back(dt);
	}
	inline void posProcess() {
		for (size_t i = 0; i < overlaping.size(); ) {
			auto &e = overlaping[i];
			auto it = std::find(accOverlap.begin(), accOverlap.end(), e);
			if (it == accOverlap.end()) {
				overlaping.erase(overlaping.begin() + i);
				onEndOverlap.broadcast(e.layer, e.inst);
			}
			else
				i++;
		}

		for (auto &i : accOverlap){
			auto it = std::find(overlaping.begin(), overlaping.end(), i);
			if (it == overlaping.end()) {
				overlaping.push_back(i);
				onBeginOverlap.broadcast(i.layer, i.inst);
			}
		}
		accOverlap.clear();
	}
	
	inline const TLayer &getLayer() {	return _layer;	}
	inline const float &getRadius() { return _inst.radius; }
	inline Vector *getTarget() { return _inst.trg; }
};



class Collision :
	public Singleton<Collision>{
public:
	list<OverlapInstance *> toverlaps;
	map<TLayer, vector<OverlapInstance *>> overlaps;

	Collision() {
		size_t sz = sizeof(TLayer) * 8;
		for (size_t i = 0; i < sz; i++)
			overlaps[1 << i] = {};
	}
	void tick() {
		for (auto &k : overlaps){
			auto &ar = k.second;
			auto sz = ar.size();
			for (size_t i = 0; i < sz; i++){
				auto a = ar[i];
				for (size_t j = i+1; j < sz; j++){
					auto b = ar[j];
					if (a->isOverlapin(*b))
						a->overlaped({ k.first, b });
					if (b->isOverlapin(*a))
						b->overlaped({ k.first, a });
				}
			}
		}
		for (auto &t : toverlaps)
			t->posProcess();
	}
};

OverlapInstance::OverlapInstance() {
	Collision::instance().toverlaps.push_back(this);
}
OverlapInstance::~OverlapInstance() {
	Collision::instance().toverlaps.remove(this);
	setLayer(0);
}
void OverlapInstance::setTarget(Vector *trg) {
	_inst.trg = trg;
}
void OverlapInstance::setLayer(TLayer newLayer) {
	auto &col = Collision::instance();
	size_t sz = sizeof(TLayer) * 8;
	for (size_t i = 0; i < sz; i++) {
		auto b = 1 << i;
		auto lb = (newLayer & b);
		if ((_layer & b) != lb) {
			if (lb == 0) {
				auto &ar = col.overlaps[b];
				auto it = std::find(ar.begin(), ar.end(), this);
				if (it != ar.end())
					col.overlaps[b].erase(it);
			}
			else 
				col.overlaps[b].push_back(this);
		}
	}
	_layer = newLayer;
}
void OverlapInstance::setRadius(float radius) {
	_inst.radius = radius;
}

#endif // !_collisionsystem