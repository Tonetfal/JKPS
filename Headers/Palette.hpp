#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include <array>
#include <memory>


class ParameterLine;

class Palette
{
    public:
        Palette(int);

        void processInput();
        void render();

        void setColorOnPalette(sf::Color color);
        void openWindow(sf::Vector2i position);
        void closeWindow();
        bool isWindowOpen() const;


    private:
        void processOwnEvents();
        void moveLineIndicator();
        void goUp();
        void goDown();
        void moveCanvasIndicator();
        void setColor();
        static sf::Color rgb(double ratio);
        static int positionToNumber(sf::Vector2i position);


    private:
        sf::RenderWindow mWindow;
        sf::Vector2f mWindowOffset;

        unsigned mLineSize;
        std::array<sf::Vertex, 3050> mLine;
        sf::FloatRect mLineRect;
        sf::RectangleShape mLineIndicator;
        sf::Color mIndicatorColor;
        int mLineElemIdx;
        bool wasButtonPressedOnLine;

        std::array<sf::Vertex, 4> mCanvas;
        sf::FloatRect mCanvasRect;
        sf::CircleShape mCanvasIndicator;
        bool wasButtonPressedOnCanvas;

        sf::Vector2f mNormilizedMouseVec;

        static float mDistance;
};