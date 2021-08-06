#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/VertexArray.hpp>

#include "LogicalParameter.hpp"

#include <array>


class KeysPerSecondGraph
{
    public:
        KeysPerSecondGraph();

        void handleOwnEvent();
        void update();
        void render();

        void openWindow();
        void closeWindow();
        bool isOpen() const;

        void updateParameters();
        static bool parameterIdMatches(LogicalParameter::ID id);


    private:
        sf::RenderWindow mWindow;

        sf::VertexArray mVertecies;
        unsigned mActiveVertecies;
};