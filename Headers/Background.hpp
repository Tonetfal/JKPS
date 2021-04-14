#pragma once

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

#include "Settings.hpp"
#include "ResourceHolder.hpp"
#include "ResourceIdentifiers.hpp"

class Background
{
    public:
                                Background(sf::RenderWindow& window);

        void                    handleEvent(sf::Event event);
        void                    draw();

        void                    loadTextures(TextureHolder& textureHolder);

        void                    scaleTexture();


    private:
        sf::RenderWindow&       mWindow;

        sf::Sprite              mBackgroundSprite;
};