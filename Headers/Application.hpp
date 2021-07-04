#pragma once

#include "Graphics.hpp"
#include "User.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>


class Application  : private sf::NonCopyable
{
    public:
        Application();
        void run();


    private:
        void processInput();
        void update(sf::Time elapsedTime);
        void render();

        void moveWindow(sf::Vector2i mousePosition);


    private:
        static const sf::Time TimePerFrame;

        sf::RenderWindow mWindow;
        Graphics mGraphics;
        User mUser;
};