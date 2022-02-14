#include "../Headers/KeysPerSecondGraph.hpp"
#include "../Headers/Button.hpp"
#include "../Headers/Settings.hpp"

#include <SFML/Window/Event.hpp>

#include <iostream>
#include <stdlib.h>


KeysPerSecondGraph::KeysPerSecondGraph()
: mVertecies(sf::TriangleFan, 16)
, mActiveVertecies(16)
{
    srand(time(NULL));

    const float widthStep = 800.f / 13.f;
    float width = 0;
    float height = 600;
    for (unsigned i = 0; i < 14; ++i)
    {
        sf::Vertex &vertex = mVertecies[i];
        vertex.position.x = width;
        vertex.position.y = height - i * 20;
        width += widthStep;
    }
    mVertecies[14].position = sf::Vector2f(800, 600);
    mVertecies[15].position = sf::Vector2f(0, 600);
}

void KeysPerSecondGraph::handleOwnEvent()
{
    auto event = sf::Event();
    while (mWindow.pollEvent(event))
    { 
        if (event.type == sf::Event::KeyPressed)
        {
            const auto key = event.key;
            if (key.control && key.code == Settings::KeyExit)
                mWindow.close();
        }

        if (event.type == sf::Event::Closed)
        {
            mWindow.close();
        }
    }
}

void KeysPerSecondGraph::update()
{

}

void KeysPerSecondGraph::render()
{
    mWindow.clear(sf::Color(30, 30, 30));

    mWindow.draw(mVertecies);

    mWindow.display();
}

void KeysPerSecondGraph::openWindow()
{
    if (!mWindow.isOpen())
    {
        sf::ContextSettings settings;
        settings.antialiasingLevel = 8;
        mWindow.create(sf::VideoMode(800, 600), "Graph", sf::Style::Close, settings);
    }
}

void KeysPerSecondGraph::closeWindow()
{
    if (mWindow.isOpen())
        mWindow.close();
}

bool KeysPerSecondGraph::isOpen() const
{
    return mWindow.isOpen();
}

void KeysPerSecondGraph::updateParameters()
{
    // mActiveVertecies =
}

bool KeysPerSecondGraph::parameterIdMatches(LogicalParameter::ID id)
{
    return false;
}
