#pragma once
#ifndef _jwindow
#define _jwindow

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>


#include <list>
#include <vector>
#include <map>
#include <set>
#include <unordered_set>
using namespace sf;
using namespace std;

#include "Utils.h"

static CircleShape cir;
static RectangleShape rs;
class JWindow : 
    public Singleton<JWindow>{
protected:
    struct DebugQuery {
        virtual void draw(RenderWindow *rwin) = 0;
    };
    struct DebugLine : DebugQuery {
        sf::Vertex v[2];
        inline void draw(RenderWindow* rwin) {
            rwin->draw(v, 2, sf::PrimitiveType::Lines);
        }
    };
    struct DebugCircle : DebugQuery {
        Color c;
        Vector2f pos;
        float radius;
        inline void draw(RenderWindow *rwin) {
            cir.setFillColor(c);
            cir.setPosition(pos);
            cir.setRadius(radius);
            cir.setOrigin(radius, radius);
            rwin->draw(cir);            
        }
    };
    struct DebugQuad : DebugQuery {
        Color c;
        Vector2f pos, size;
        inline void draw(RenderWindow *rwin) {
            rs.setFillColor(c);
            rs.setPosition(pos);
            rs.setSize(size);
            //rs.setOutlineThickness(-1);
            rwin->draw(rs);
        }
    };

    list<DebugQuery*> debugQueryDraw;


    list<sf::Drawable*> drawables;
    RenderWindow *rwin = nullptr;
public: 
    JWindow() {
        rwin = new RenderWindow(sf::VideoMode(800, 600), "SFML window");
        rwin->setFramerateLimit(60);
    }
    ~JWindow(){}

    JWindow &operator<<(Drawable *d) {
        drawables.push_back(d);
        return *this;
    }
    JWindow &operator>>(Drawable *d) {
        drawables.remove(d);
        return *this;
    }
    inline bool isOpen() {
        return rwin->isOpen();
    }
    inline void tick() {
        sf::Event event;
        while (rwin->pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                rwin->close();
        }

        rwin->clear();
        for (auto *d : drawables)
            rwin->draw(*d);

        for (DebugQuery *d : debugQueryDraw) {
            d->draw(rwin);
            delete d;
        }

        debugQueryDraw.clear();
        rwin->display();
    }

    Vector mousePos() {        
        auto pi = sf::Mouse::getPosition() - rwin->getPosition();
        pi -= {10, 30};
        return { pi.x, pi.y };
    }

    void debugLine(Vector2f pos, Vector2f end, Color c) {
        DebugLine *d = new DebugLine;
        d->v[0].position = pos;
        d->v[1].position = end;
        d->v[1].color = d->v[0].color= c;
        debugQueryDraw.push_back(d);
    }
    void debugCircle(Vector2f pos, float radius, Color c) {
        DebugCircle *dc = new DebugCircle;
        dc->pos = pos;
        dc->c = c;
        dc->radius = radius;
        debugQueryDraw.push_back(dc);
    }
    void debugQuad(Vector2f pos, Vector2f size, Color c) {
        DebugQuad *dc = new DebugQuad;
        dc->pos = pos;
        dc->size = size;
        dc->c = c;
        debugQueryDraw.push_back(dc);
    }
};
#define win JWindow::instance()
#define mousePos JWindow::instance().mousePos()
#define pushDraw(d) JWindow::instance()<<d
#define popDraw(d) JWindow::instance()>>d
#define drawDebugLine JWindow::instance().debugLine
#define drawDebugCir JWindow::instance().debugCircle
#define drawDebugQuad JWindow::instance().debugQuad


class World :
    public Singleton<World> {
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

#endif // !_jwindow



