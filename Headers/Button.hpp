#pragma once

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Window/Event.hpp>

#include "KeyPressingManager.hpp"
#include "ResourceIdentifiers.hpp"
#include "Settings.hpp"
#include "Statistics.hpp"

#include <vector>

class Button
{
    public:
                                    Button(sf::RenderWindow& window);

        void                        update(std::vector<bool>& needToBeReleased);
        void                        draw();
        void                        handleInput(std::vector<bool>& clickedKeys);
        void                        handleEvent(sf::Event event);

        void                        setupTexture();
        void                        setupAnimation();

        void                        loadTextures(sf::Texture& buttonTexture);


    private:
        void                        setTextures();
        void                        setColor();
        void                        setPositions();
        void                        scaleTexture();

    
    private:
        sf::RenderWindow&           mWindow;

        sf::Texture*                mButtonTexture;

        std::vector<sf::Sprite>     mKeysSprite;
        std::vector<sf::RectangleShape> mKeysAnimation;
};