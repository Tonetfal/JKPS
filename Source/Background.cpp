#include "../Headers/Background.hpp"

Background::Background(sf::RenderWindow& window)
: mWindow(window)
{ 
    mBackgroundSprite.setColor(Settings::BackgroundColor);
}

void Background::handleEvent(sf::Event event)
{
    scaleTexture();
}

void Background::draw()
{
    mWindow.draw(mBackgroundSprite);
}

void Background::loadTextures(TextureHolder& textureHolder)
{
    mBackgroundSprite.setTexture(textureHolder.get(Textures::Background));
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