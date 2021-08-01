#include "jEngine.hpp"
#include "CollisionSystem.hpp"






struct Stats {
	unsigned int f = 0, col = 0;
	Clock ms;
};
Stats st;
class Actor : public E::Instance{
public:
	Vector pos, dir;
	CircleShape c;
	Actor() {
		pos = { rand() % 600 + 100, rand() % 500 + 50 };
		dir = { rand() % 3 + 1, rand() % 3 + 1 };
		dir = dir.rotate(rand() % 180);
		c.setRadius(rand()%10+5);
		//c.setOrigin({ 5, 5 });
		c.setFillColor(Color::Magenta);
		r.size = { c.getRadius()*2,  c.getRadius() *2};
		pushDraw(&c);
	}
	virtual ~Actor() {
		popDraw(&c);
	}
	void tick() {
		pos = pos + dir;
		if (pos.x < 0)
			pos.x = 800;
		else if (pos.x > 800)
			pos.x = 0;

		if (pos.y < 0)
			pos.y = 600;
		else if (pos.y > 600)
			pos.y = 0;

		this->r.pos = pos;
		c.setFillColor(Color::Green);
		c.setPosition(pos);
	}
	void onBeginOverlap(Actor *o) {
		c.setFillColor(Color::Red);
	}
	void collision(E::Instance *o) override{
		c.setFillColor(Color::Red);
	}
};
static vector<Actor *> acts;

vector<Actor *> orderByDistance(vector<Actor *> ls, Vector location) {
	vector<Actor *> out;
	while (ls.size()) {
		std::pair<Actor *, double> p = { nullptr, 0 };
		for (auto &a : ls) {
			double nDis = Vector::distance(location, a->pos);
			if (nDis >= p.second)
				p = { a, nDis };
		}
		out.push_back(p.first);
		out.erase(out.begin());
	}
	return out;
}



void distanceBased() {
	for (size_t x = 0; x < acts.size(); x++) {
		for (size_t y = x + 1; y < acts.size(); y++) {
			if (E::inBoundry(acts[x]->r, acts[y]->r)) {
				acts[x]->onBeginOverlap(acts[y]);
				acts[y]->onBeginOverlap(acts[x]);
			}
			st.col++;
		}
	}
}
int main() {
	E::Quad *root = new E::Quad({ { 50, 50 }, {700, 500} }, 4);
	E::build(root);
	for (size_t i = 0; i < 500; i++) {
		acts.push_back(new Actor);
	}

	while (true) {

		
		st.ms.restart();

		for (int x = 0; x < acts.size(); x++) {
			if (x == 0) {
				acts[0]->pos = mousePos;
				acts[0]->pos = acts[0]->pos - acts[0]->r.size;
			}
			acts[x]->tick();
		}
		E::tick(root);
		//distanceBased();
		st.col = E::collTests;
		cout << st.ms.getElapsedTime().asMilliseconds() << ", " << st.col << endl;
		st.col = 0;


		e.tick();
	}
	return 0;
}


//
//
//#include <iostream>
//#include <cmath>
//using namespace std;
//
//// Used to hold details of a point
//struct Point
//{
//    int x;
//    int y;
//    Point(int _x, int _y)
//    {
//        x = _x;
//        y = _y;
//    }
//    Point()
//    {
//        x = 0;
//        y = 0;
//    }
//};
//
//// The objects that we want stored in the quadtree
//struct Node
//{
//    Point pos;
//    int data;
//    Node(Point _pos, int _data)
//    {
//        pos = _pos;
//        data = _data;
//    }
//    Node()
//    {
//        data = 0;
//    }
//};
//
//// The main quadtree class
//class Quad
//{
//    // Hold details of the boundary of this node
//    Point topLeft;
//    Point botRight;
//
//    // Contains details of node
//    Node *n;
//
//    // Children of this tree
//    Quad *topLeftTree;
//    Quad *topRightTree;
//    Quad *botLeftTree;
//    Quad *botRightTree;
//
//public:
//    Quad()
//    {
//        topLeft = Point(0, 0);
//        botRight = Point(0, 0);
//        n = NULL;
//        topLeftTree = NULL;
//        topRightTree = NULL;
//        botLeftTree = NULL;
//        botRightTree = NULL;
//    }
//    Quad(Point topL, Point botR)
//    {
//        n = NULL;
//        topLeftTree = NULL;
//        topRightTree = NULL;
//        botLeftTree = NULL;
//        botRightTree = NULL;
//        topLeft = topL;
//        botRight = botR;
//    }
//    void insert(Node *);
//    Node *search(Point);
//    bool inBoundary(Point);
//};
//
//// Insert a node into the quadtree
//void Quad::insert(Node *node)
//{
//    if (node == NULL)
//        return;
//
//    // Current quad cannot contain it
//    if (!inBoundary(node->pos))
//        return;
//
//    // We are at a quad of unit area
//    // We cannot subdivide this quad further
//    if (abs(topLeft.x - botRight.x) <= 1 &&
//        abs(topLeft.y - botRight.y) <= 1)
//    {
//        if (n == NULL)
//            n = node;
//        return;
//    }
//
//    if ((topLeft.x + botRight.x) / 2 >= node->pos.x)
//    {
//        // Indicates topLeftTree
//        if ((topLeft.y + botRight.y) / 2 >= node->pos.y)
//        {
//            if (topLeftTree == NULL)
//                topLeftTree = new Quad(
//                    Point(topLeft.x, topLeft.y),
//                    Point((topLeft.x + botRight.x) / 2,
//                        (topLeft.y + botRight.y) / 2));
//            topLeftTree->insert(node);
//        }
//
//        // Indicates botLeftTree
//        else
//        {
//            if (botLeftTree == NULL)
//                botLeftTree = new Quad(
//                    Point(topLeft.x,
//                        (topLeft.y + botRight.y) / 2),
//                    Point((topLeft.x + botRight.x) / 2,
//                        botRight.y));
//            botLeftTree->insert(node);
//        }
//    }
//    else
//    {
//        // Indicates topRightTree
//        if ((topLeft.y + botRight.y) / 2 >= node->pos.y)
//        {
//            if (topRightTree == NULL)
//                topRightTree = new Quad(
//                    Point((topLeft.x + botRight.x) / 2,
//                        topLeft.y),
//                    Point(botRight.x,
//                        (topLeft.y + botRight.y) / 2));
//            topRightTree->insert(node);
//        }
//
//        // Indicates botRightTree
//        else
//        {
//            if (botRightTree == NULL)
//                botRightTree = new Quad(
//                    Point((topLeft.x + botRight.x) / 2,
//                        (topLeft.y + botRight.y) / 2),
//                    Point(botRight.x, botRight.y));
//            botRightTree->insert(node);
//        }
//    }
//}
//
//// Find a node in a quadtree
//Node *Quad::search(Point p)
//{
//    // Current quad cannot contain it
//    if (!inBoundary(p))
//        return NULL;
//
//    // We are at a quad of unit length
//    // We cannot subdivide this quad further
//    if (n != NULL)
//        return n;
//
//    if ((topLeft.x + botRight.x) / 2 >= p.x)
//    {
//        // Indicates topLeftTree
//        if ((topLeft.y + botRight.y) / 2 >= p.y)
//        {
//            if (topLeftTree == NULL)
//                return NULL;
//            return topLeftTree->search(p);
//        }
//
//        // Indicates botLeftTree
//        else
//        {
//            if (botLeftTree == NULL)
//                return NULL;
//            return botLeftTree->search(p);
//        }
//    }
//    else
//    {
//        // Indicates topRightTree
//        if ((topLeft.y + botRight.y) / 2 >= p.y)
//        {
//            if (topRightTree == NULL)
//                return NULL;
//            return topRightTree->search(p);
//        }
//
//        // Indicates botRightTree
//        else
//        {
//            if (botRightTree == NULL)
//                return NULL;
//            return botRightTree->search(p);
//        }
//    }
//};
//
//// Check if current quadtree contains the point
//bool Quad::inBoundary(Point p)
//{
//    return (p.x >= topLeft.x &&
//        p.x <= botRight.x &&
//        p.y >= topLeft.y &&
//        p.y <= botRight.y);
//}
//
//// Driver program
//int main()
//{
//    Quad center(Point(0, 0), Point(8, 8));
//    Node a(Point(1, 1), 1);
//    Node b(Point(2, 5), 2);
//    Node c(Point(7, 6), 3);
//    center.insert(&a);
//    center.insert(&b);
//    center.insert(&c);
//    cout << "Node a: " <<
//        center.search(Point(1, 1))->data << "\n";
//    cout << "Node b: " <<
//        center.search(Point(2, 5))->data << "\n";
//    cout << "Node c: " <<
//        center.search(Point(7, 6))->data << "\n";
//    cout << "Non-existing node: "
//        << center.search(Point(5, 5));
//    return 0;
//}