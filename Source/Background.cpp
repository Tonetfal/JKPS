#include "../Headers/Background.hpp"
#include "../Headers/ResourceHolder.hpp"
#include "../Headers/Settings.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Color.hpp>


Background::Background(const TextureHolder& textureHolder, sf::RenderWindow& window)
: mWindow(window)
, mTextures(textureHolder)
{ 
    mBackgroundSprite.setTexture(mTextures.get(Textures::Background));
    setupTexture();
}


void Background::draw(sf::RenderTarget &target, sf::RenderStates states) const 
{
    target.draw(mBackgroundSprite, states);
}

// This is needed to rescale the background when the amount of keys is changed
void Background::resize()
{
    scale();
}

void Background::scale()
{
    if (Settings::ScaleBackground)
    {
        sf::Vector2u windowSize = mWindow.getSize();
        sf::Vector2f bgSize(mBackgroundSprite.getTexture()->getSize());
        sf::Vector2f scale(
            windowSize.x / (bgSize.x * mBackgroundSprite.getScale().x),
            windowSize.y / (bgSize.y * mBackgroundSprite.getScale().y));

        mBackgroundSprite.scale(scale);
    }
    else
    {
        mBackgroundSprite.setScale(1, 1);
    }
}

void Background::setupTexture()
{
    mBackgroundSprite.setTexture(mTextures.get(Textures::Background));
    mBackgroundSprite.setColor(Settings::BackgroundColor);
    scale();
}