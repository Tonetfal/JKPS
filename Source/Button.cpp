#include "../Headers/Button.hpp"

Button::Button(sf::RenderWindow& window)
: mWindow(window)
, mKeysSprite(Settings::ButtonAmount)
, mKeysAnimation(Settings::ButtonAmount)
{ }

void Button::update(std::vector<bool>& needToBeReleased)
{
    for (size_t i = 0; i < Settings::ButtonAmount; ++i)
    {
        if (!needToBeReleased[i])
            mKeysAnimation[i].setColor(mKeysAnimation[i].getColor() - 
                sf::Color(0,0,0,Settings::AnimationVelocity));
    }
}

void Button::draw()
{
    for (auto& element : mKeysSprite)
        mWindow.draw(element);

    for (auto& element : mKeysAnimation)
        mWindow.draw(element);
}

// Increase/Decrease key amount
void Button::handleEvent(sf::Event event)
{
    mKeysSprite.resize(Settings::ButtonAmount);
    mKeysAnimation.resize(Settings::ButtonAmount);

    setupTextures();
}

// User clicks a key
void Button::handleInput(std::vector<bool>& needToBeReleased)
{
    for (size_t i = 0; i < Settings::ButtonAmount; ++i)
        if (needToBeReleased[i])
            mKeysAnimation[i].setColor(Settings::AnimationColor);
}

void Button::setupTextures()
{
    setTextures(mKeysSprite, *mButtonTexture);
    setColor(mKeysSprite, Settings::ButtonTextureColor);
    scaleTexture(mKeysSprite, Settings::ButtonTextureSize);
    setPositions(mKeysSprite);

    setTextures(mKeysAnimation, *mAnimationTexture);
    setColor(mKeysAnimation, Settings::AnimationTextureColor);
    scaleTexture(mKeysAnimation, Settings::ButtonTextureSize);
    setPositions(mKeysAnimation);
}

void Button::loadTextures(TextureHolder& textureHolder)
{
    mButtonTexture = &textureHolder.get(Textures::KeyButton);
    mAnimationTexture = &textureHolder.get(Textures::ButtonAnimation);
}

void Button::setTextures(std::vector<sf::Sprite>& vector, sf::Texture& texture)
{
    for (auto& element : vector)
        element.setTexture(texture);
}

void Button::setColor(std::vector<sf::Sprite>& vector, sf::Color& color)
{
    for (auto& element : vector)
        element.setColor(color);
}

void Button::scaleTexture(std::vector<sf::Sprite>& vector, const sf::Vector2u& textureSize)
{
    sf::Vector2f vec2f(textureSize);
    for (auto& element : vector)
    {
        element.scale(
            vec2f.x / element.getGlobalBounds().width, 
            vec2f.y / element.getGlobalBounds().height);
    }
}

void Button::setPositions(std::vector<sf::Sprite>& vector)
{
    for (size_t i = 0; i < vector.size(); ++i)
    {
        sf::Vector2f position(
            Settings::Distance * (i + 1) + vector[i].getGlobalBounds().width * i,
            Settings::Distance);
        vector[i].setPosition(position);
    }
}