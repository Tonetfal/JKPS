#pragma once

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include "ResourceHolder.hpp"
#include "ResourceIdentifiers.hpp"

class Background
{
    public:
                                Background(sf::RenderWindow& window);

        void                    draw();

        void                    loadTextures(TextureHolder& textureHolder);


    private:
        sf::RenderWindow&       mWindow;

        sf::Sprite              mBackgroundSprite;
};