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

        void rescale();
        void updateAssets();


    private:
        const TextureHolder &mTextures;
        sf::RenderWindow& mWindow;
        sf::Sprite mBackgroundSprite;
};