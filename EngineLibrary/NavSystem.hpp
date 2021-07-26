#ifndef _navsystem
#define _navsystem
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <list>
#include <vector>
#include <iostream>
#include "Utils.h"
#include "jwindow.hpp"
using namespace sf;
using namespace std;
enum class eDirection { up, right, down, left };
constexpr Vector dirToVector(eDirection dir) {
	switch (dir) {
	case eDirection::up:    return { 0, 1 };
	case eDirection::down:  return { 0, -1 };
	case eDirection::left:  return { -1, 0 };
	case eDirection::right: return { 1, 0 };
	}
}
struct Node {
	Vector wPos;
	Node *neighbours[4] = { 0 };
	CircleShape *c = new CircleShape;
	Node() {
		c->setFillColor(Color::Blue);
		c->setRadius(5);
		c->setOrigin(5, 5);
	}
	bool operator==(Node &o) const {
		return *this == o;
	}
};
class Navigation :
	public Singleton<Navigation> {
public:
	Vector cellSize;
	list<Node *> nodes;
	~Navigation() {
		for (auto n : nodes)
			delete n;
		nodes.clear();
	}

	inline void linkNode(Node *trg) {
#define fillNeighbours(e) \
		{Node *f = getAt(trg->wPos + dirToVector(e) * cellSize); \
		 trg->neighbours[int(e)] = f != trg ? f : nullptr;}

		fillNeighbours(eDirection::up);
		fillNeighbours(eDirection::down);
		fillNeighbours(eDirection::left);
		fillNeighbours(eDirection::right);
#undef fillNeighbours
	}

	void config(Vector cellSize) {
		this->cellSize = cellSize;
	}
	void meshByImage(Image m) {
		auto size = m.getSize();
		list<Vector> validPoints;
		for (int x = 0; x < size.x; x++) {
			for (int y = 0; y < size.y; y++) {
				if (m.getPixel(x, y) != sf::Color::Black) {
					Vector pixPos{ x, y };
					Vector pos = (pixPos / this->cellSize).round() * this->cellSize;
					if (Vector::distance(pixPos, pos) > 0)
						continue;
					else if ((std::find(validPoints.begin(), validPoints.end(), pos) != validPoints.end()))
						continue;
					else {
						validPoints.push_back(pos);
						Node *n = new Node;
						n->wPos = { pos };
						nodes.push_back(n);
					}
				}
			}
		}
		for (auto n : nodes)
			linkNode(n);
	}
	Node *getAt(Vector pos) {
		double d = cellSize.length() / 2.f;
		for (auto n : nodes) {
			if (Vector::distance(n->wPos, pos) < d)
				return n;
		}
		return nullptr;
	}
	void setDebug(bool enable) {
		if (enable) {
			for (auto n : nodes) {
				n->c->setPosition(n->wPos);
				pushDraw(n->c);
			}
		}
		else {
			for (auto n : nodes) {
				popDraw(n->c);
			}
		}
	}
};
#endif // !_navsystem