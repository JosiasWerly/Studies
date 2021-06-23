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
    list<sf::Drawable *> drawables;
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
        rwin->display();
    }
};
#define win JWindow::instance()
#define pushDraw(d) JWindow::instance()<<d
#define popDraw(d) JWindow::instance()>>d
#endif // !_jwindow



