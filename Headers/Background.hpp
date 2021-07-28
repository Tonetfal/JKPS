#pragma once

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include "ResourceIdentifiers.hpp"

namespace sf
{
    class RenderWindow;
}


class Background : public sf::Drawable, public sf::Transformable
{
    public:
        Background(const TextureHolder& textureHolder, sf::RenderWindow& window);

        virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

        void resize();
        void scale();
        void setupTexture();


    private:
        sf::RenderWindow& mWindow;
        sf::Sprite mBackgroundSprite;
};