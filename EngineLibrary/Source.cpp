#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <list>
#include <vector>
#include <iostream>
using namespace sf;
using namespace std;

#include "Utils.h"
#include "jwindow.hpp"

enum class eDirection { up, down, left, right };
constexpr Vector vectorDir(eDirection dir) {
    switch (dir){
    case eDirection::up:    return { 0, 1 };
    case eDirection::down:  return { 0, -1 };
    case eDirection::left:  return { -1, 0 };
    case eDirection::right: return { 1, 0 };
    }
}
struct Node {
    Vector pos, worldPos;
    Node *neighbours[4] = { 0 };

    CircleShape *c = new CircleShape;
};


class NavMesh{
    Vector worldSize, meshSize;
    int totalMeshSize = 0;
public:
    vector<Node*> nodes;
    ~NavMesh() {
        for (auto n : nodes)
            delete n;
        nodes.clear();
    }



    void select(Node *node) {
        node->c->setFillColor(Color::Green);
        for (auto n : node->neighbours)
            if(n)
                n->c->setFillColor(Color::Blue);
    }
    inline Node *getAtWoldPosition(Vector pos) {
        return getAt(pos / worldSize * meshSize);
    }
    inline Node* getAt(Vector pos) {
        if (pos.x >= 0 && pos.y >= 0 && pos.x < meshSize.x && pos.y < meshSize.y) {
            unsigned int _pos = pos.x + pos.y * meshSize.y;
            return nodes[_pos];
        }
        return nullptr;
    }
    void createMesh(Vector meshSize, Vector offset, Vector worldSize) {
        this->worldSize = worldSize;
        this->meshSize = meshSize;
        this->totalMeshSize = meshSize.x * meshSize.y;

        

        float _x = worldSize.x / meshSize.x, _y = worldSize.y / meshSize.y;
        for (size_t y = 0; y < meshSize.y; y++){
            for (size_t x = 0; x < meshSize.x; x++) {
                auto n = new Node;
                n->pos = { int(x), int(y) };
                n->worldPos = {x * _x + offset.x , y * _y + offset.y};
                nodes.push_back(n);

                auto *_c = n->c;
                _c->setPosition(n->worldPos);
                _c->setFillColor(Color::Red);
                _c->setOrigin(2, 2);
                _c->setRadius(2);
                pushDraw(_c);
            }
        }

        for (auto n : nodes){
            n->neighbours[int(eDirection::up)] = getAt(n->pos + vectorDir(eDirection::up));
            n->neighbours[int(eDirection::down)] = getAt(n->pos + vectorDir(eDirection::down));
            n->neighbours[int(eDirection::left)] = getAt(n->pos + vectorDir(eDirection::left));
            n->neighbours[int(eDirection::right)] = getAt(n->pos + vectorDir(eDirection::right));
        }
    }
};
static NavMesh nav;




void simpleMove(Vector trgLocation, Node *root, list<Node *> &path, float minDistance) {
    if (root) {
        pair<float, Node *> selected = { minDistance,  nullptr };
        for (auto _n : root->neighbours) {
            if (_n) {
                float d = Vector::distance(_n->worldPos, trgLocation);                
                if (selected.first > d) {
                    cout << d << endl;
                    selected = { d, _n };
                }
            }
        }
        if (selected.second && selected.first > 0) {
            path.push_back(selected.second);
            simpleMove(trgLocation, selected.second, path, selected.first);
        }
    }
}

int main() {
    nav.createMesh({ 50, 50}, {10, 10}, { 800, 600 });

    
    
    

    while (true){
        for (auto n : nav.nodes)
            n->c->setFillColor(sf::Color::Red);

        float x, y;
        cin >> x; cin >> y;
        list<Node *> path;
        simpleMove({ x, y }, nav.getAtWoldPosition({ 0, 0 }), path, 99999);
        for (auto p : path) {
            nav.select(p);
        }
        win.tick();
        system("pause");
    }
	return 0;
}