#include "../Headers/Background.hpp"

Background::Background(sf::RenderWindow& window)
: mWindow(window)
{ }

void Background::draw()
{
    mWindow.draw(mBackgroundSprite);
}

void Background::loadTextures(TextureHolder& textureHolder)
{
    mBackgroundSprite.setTexture(textureHolder.get(Textures::Background));
}