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

    static bool hasNeighbours(Node *n) {
        return n->neighbours[0] || n->neighbours[1] || n->neighbours[2] || n->neighbours[3];
        //for (auto _n : n->neighbours)
        //    if (_n)
        //        return true;
        //return false;
    }
    static list<Node *> validNeighbours(Node *n) {
        list<Node *> _nodes;
        for (auto _n : n->neighbours) {
            if (_n)
                _nodes.push_back(_n);
        }
        return _nodes;
    }
    static Node* randomNeighbours(Node *n) {
        vector<Node *> _nodes;
        for (auto _n : n->neighbours){
            if (_n)
                _nodes.push_back(_n);
        }
        if (_nodes.size() > 0)
            return _nodes[rand() % _nodes.size()];
        return nullptr;
    }
    
};

class NavMesh{
    Vector worldSize, meshSize;
    int totalMeshSize = 0;
public:
    vector<Node*> nodes;
    list<Node*> eNodes;
    ~NavMesh() {
        for (auto n : nodes)
            delete n;
        nodes.clear();
    }

    inline Node *getAtWoldPosition(Vector pos) {
        if (pos.x >= 0 && pos.y >= 0 && pos.x < worldSize.x && pos.y < worldSize.y)
            return getAt((pos / worldSize * meshSize).floor());
        return nullptr;
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
                _c->setFillColor(Color::Green);
                _c->setOrigin(2, 2);
                _c->setRadius(2);
                
            }
        }
    }
    void meshByImage(Image m) {
        auto size = m.getSize();
        for (int x = 0; x < size.x; x++){
            for (int y = 0; y < size.y; y++) {
                if (m.getPixel(x, y) != sf::Color::Black) {
                    if (Node *n = getAtWoldPosition({ x, y })) {
                        enableNode(n);
                    }
                }
            }
        }
    }

	void enableNodes(list<Node *> nodes) {
		for (auto n : nodes)
			enableNode(n);
	}
	void disableNodes(list<Node *> nodes) {
		for (auto n : nodes)
			disableNode(n);
	}

    inline void enableNode(Node* n) {
		n->neighbours[int(eDirection::up)] = getAt(n->pos + vectorDir(eDirection::up));
		n->neighbours[int(eDirection::down)] = getAt(n->pos + vectorDir(eDirection::down));
		n->neighbours[int(eDirection::left)] = getAt(n->pos + vectorDir(eDirection::left));
		n->neighbours[int(eDirection::right)] = getAt(n->pos + vectorDir(eDirection::right));
        eNodes.push_back(n);
    }
    inline void disableNode(Node *n) {
        n->neighbours[int(eDirection::up)] = 0;
        n->neighbours[int(eDirection::down)] = 0;
        n->neighbours[int(eDirection::left)] = 0;
        n->neighbours[int(eDirection::right)] = 0;
        eNodes.remove(n);
    }
    
    Node *getEnableNodeAt(int id){
        int x = 0;
        for (auto n : eNodes) {
            if (x++ == id)
                return n;
        }
        return nullptr;
    }
    bool isNodeEnable(Node *node) {
        for (auto n : eNodes)
            if (n == node)
                return true;
        return false;
    }
    bool isValidPos(Vector &worldPos) {
        if (Node *n = getAtWoldPosition(worldPos))
            return Node::hasNeighbours(n);
        return false;
    }
    bool traceForward(Vector worldPos, Vector dir, int size) {        
        for (size_t i = 0; i < size; i++) {
            Vector trace = worldPos + dir * i;
            if (!isValidPos(trace))
                return true;
        }
        drawDebug(worldPos, worldPos + dir * size, Color::Red);
        return false;
    }


    void setDebug(bool enable) {
        if (enable) {
            for (auto n : nodes){
                if (Node::hasNeighbours(n))
                    pushDraw(n->c);
            }
        }
        else {
            for (auto n : nodes) {
                if (Node::hasNeighbours(n)) {
                    popDraw(n->c);
                }
            }
        }
    }
    void select(Node *node) {
        node->c->setFillColor(Color::Green);
        for (auto n : node->neighbours)
            if (n) {
                n->c->setOutlineColor(Color::Red);
                n->c->setOutlineThickness(-1);

            }
    }
};
static NavMesh nav;







class World{
    Sprite sptr;
    Texture txt;
    Image img;
    
public:
    World() {
        txt.loadFromFile("maze.png");
        sptr.setTexture(txt);
        pushDraw(&sptr);
    }
    Image getImage() {
        return txt.copyToImage();
    }
};
static World world;












//avoidance, alignment, and coherence.
struct Boid {
private:
    bool noCollision() {
        int nColl = 0;
        int trace = 20;
        if (nav.traceForward(pos, Vector::rotate(dir, 90), trace / 2) || nav.traceForward(pos, Vector::rotate(dir, 20), trace)) {
            nColl = 1;
        }
        if (nav.traceForward(pos, Vector::rotate(dir, -90), trace/2) || nav.traceForward(pos, Vector::rotate(dir, -20), trace)) {
            nColl = 2;
        }
        
        if (collided == 0 && nColl != 0)
            collided = nColl;
        if (collided != 0 && nColl == 0)
            collided = 0;


        if (collided != 0) {
            if (collided == 1)
                dir = Vector::rotate(dir, -10);
            else if (collided == 2)
                dir = Vector::rotate(dir, 10);
        }

        //if (nav.traceForward(pos, dir, 25))
        //    collided = 3;
        //else if (nav.traceForward(pos, Vector::rotate(dir, 20), 25))
        //    collided = 1;
        //else if (nav.traceForward(pos, Vector::rotate(dir, -20), 25))
        //    collided = 2;
        //else{
        //    collided = 0;
        //    return true;
        //}
        //
        
        return collided == 0;
    }
    CircleShape cir;
public:
    Boid(Vector pos) {
        this->pos = pos;

        dir = { 1, 1 };
        speed = maxSpeed = abs(sin(rand() % 3) * (rand() % 2)) + 1;        
        cir.setFillColor(Color::Red);
        cir.setRadius(rand() % 4 + 2);
        cir.setOrigin(cir.getRadius(), cir.getRadius());
        pushDraw(&cir);
    }
    ~Boid() {
        popDraw(&cir);
    }


    int collided = 0;
    Vector pos, dir, invalidPos;
    float speed, maxSpeed;
    void getClosests(list<Boid *> &boids, list<Boid *> &out, float distance = 20) {
        for (auto b : boids) {
            float d = Vector::distance(pos, b->pos);
            if (d < distance && b->collided == 0) {
                drawDebug(pos, b->pos, Color::Magenta);
                out.push_back(b);
            }
        }
            
    }
    void tick(list<Boid *> &boids) {
        list<Boid *> group;
        getClosests(boids, group);
        if (noCollision()) {
            dir = Vector::lerp(dir, Vector::rotate(dir, rand() % 20 - 10), 0.2).normalize();
            pos = pos + dir * speed;
            cir.setPosition(pos);
        }        
    }
};


class Boids {
public:
    list<Boid*> boids;
    inline void createBoids(int qtd) {
        for (int x = 0; x < qtd; x++) {
            Node *n = nav.getEnableNodeAt(rand() % nav.eNodes.size());
            if (n)
                boids.push_back(new Boid({ n->worldPos }));
        }
    }
    inline void tick() {
        for (auto b : boids) {
            b->tick(boids);
        }
    }

};
static Boids boids;


int main() {

    srand(time(NULL));
    nav.createMesh({ 200, 200}, { 0, 0}, { 800, 600 });
    nav.meshByImage(world.getImage());
    nav.setDebug(0);
    
    boids.createBoids(100);
    
    


    while (true){
        boids.tick();
        win.tick();
    }
	return 0;
}













//void simpleMove(Vector trgLocation, Node *root, list<Node *> &path, float &minDistance) {
//    if (root) {
//        pair<float, Node *> selected = { minDistance,  nullptr };
//        for (auto _n : root->neighbours) {
//            if (_n) {
//                float d = Vector::distance(_n->worldPos, trgLocation);
//                if (selected.first > d) {
//                    cout << d << endl;
//                    selected = { d, _n };
//                }
//            }
//        }
//        if (selected.second && selected.first > 0) {
//            path.push_back(selected.second);
//            simpleMove(trgLocation, selected.second, path, selected.first);
//        }
//    }
//}