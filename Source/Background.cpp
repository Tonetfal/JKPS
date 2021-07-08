#include "../Headers/Background.hpp"

Background::Background(sf::RenderWindow& window)
: mWindow(window)
{ 
    mBackgroundSprite.setColor(Settings::BackgroundColor);
}

// This is needed to rescale the background when the amount of keys is changed
void Background::handleEvent(sf::Event event)
{
    if (Settings::ScaleBackground)
        scaleTexture();
}

void Background::draw()
{
    mWindow.draw(mBackgroundSprite);
}

void Background::loadTextures(TextureHolder& textureHolder)
{
    mBackgroundSprite.setTexture(textureHolder.get(Textures::Background));
    if (Settings::ScaleBackground)
        scaleTexture();
}

void Background::scaleTexture()
{
    sf::Vector2f windowSize(mWindow.getSize());
    sf::Vector2f bgSize(mBackgroundSprite.getTexture()->getSize());
    sf::Vector2f scale(
        windowSize.x / (bgSize.x * mBackgroundSprite.getScale().x),
        windowSize.y / (bgSize.y * mBackgroundSprite.getScale().y));

    mBackgroundSprite.scale(scale);
}