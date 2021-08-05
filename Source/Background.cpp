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
    updateAssets();
}

void Background::draw(sf::RenderTarget &target, sf::RenderStates states) const 
{
    target.draw(mBackgroundSprite, states);
}

void Background::rescale()
{
    sf::Vector2f scale(1.f, 1.f);
    if (Settings::ScaleBackground)
    {
        sf::Vector2u windowSize = mWindow.getSize();
        sf::Vector2f bgSize(mBackgroundSprite.getTexture()->getSize());

        scale.x = static_cast<float>(windowSize.x) / bgSize.x;
        scale.y = static_cast<float>(windowSize.y) / bgSize.y;
    }
    mBackgroundSprite.setScale(scale);
    mBackgroundSprite.setColor(Settings::isGreenscreenSet ? sf::Color::White : Settings::BackgroundColor);
}

void Background::updateAssets()
{
    mBackgroundSprite.setTexture(mTextures.get(Textures::Background), true);
    rescale();
}