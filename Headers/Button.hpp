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

        void                        setupTextures();

        void                        loadTextures(TextureHolder& textureHolder);


    private:
        void                        setTextures(std::vector<sf::Sprite>& vector, sf::Texture& texture);
        void                        setColor(std::vector<sf::Sprite>& vector, sf::Color& color);
        void                        scaleTexture(std::vector<sf::Sprite>& vector, const sf::Vector2u& texture);
        void                        setPositions(std::vector<sf::Sprite>& vector);

    
    private:
        sf::RenderWindow&           mWindow;

        sf::Texture*                mButtonTexture;
        sf::Texture*                mAnimationTexture;

        std::vector<sf::Sprite>     mKeysSprite;
        std::vector<sf::Sprite>     mKeysAnimation;
};