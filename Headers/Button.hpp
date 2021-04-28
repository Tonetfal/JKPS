#pragma once

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

#include "KeyPressingManager.hpp"
#include "ResourceIdentifiers.hpp"
#include "Settings.hpp"
#include "Statistics.hpp"

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

        void                        loadTextures(TextureHolder& textureHolder);


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