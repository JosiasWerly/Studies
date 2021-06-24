#pragma once
#ifndef _jwindow
#define _jwindow

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>


#include <list>
#include <vector>
using namespace sf;
using namespace std;

#include "Utils.h"
class JWindow : 
    public Singleton<JWindow>{
protected:
    struct DebugLine{
        sf::Vertex v[2];
    };
    list<DebugLine> debugDraw;
    list<sf::Drawable*> drawables;
    RenderWindow *rwin = nullptr;
public:
    JWindow() {
        rwin = new RenderWindow(sf::VideoMode(800, 600), "SFML window");
        rwin->setFramerateLimit(30);
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

        for (DebugLine &d : debugDraw)
            rwin->draw(d.v, 2, sf::PrimitiveType::Lines);
        debugDraw.clear();
        rwin->display();
    }


    void debugLine(Vector2f pos, Vector2f end, Color c) {
        DebugLine d;
        d.v[0].position = pos;
        d.v[1].position = end;
        d.v[1].color = d.v[0].color= c;
        debugDraw.push_back(d);
    }
};
#define win JWindow::instance()
#define pushDraw(d) JWindow::instance()<<d
#define popDraw(d) JWindow::instance()>>d
#define drawDebug JWindow::instance().debugLine
#endif // !_jwindow



