#include "../Headers/Button.hpp"

Button::Button(sf::RenderWindow& window)
: mWindow(window)
, mKeysSprite(Settings::ButtonAmount)
, mKeysAnimation(Settings::ButtonAmount)
{ 

}

void Button::update(std::vector<bool>& needToBeReleased)
{
    for (size_t i = 0; i < Settings::ButtonAmount; ++i)
    {
        if (!needToBeReleased[i])
            mKeysAnimation[i].setFillColor(mKeysAnimation[i].getFillColor()
                                            - sf::Color(0,0,0,Settings::AnimationVelocity));
    }
}

void Button::draw()
{
    for (auto& element : mKeysSprite)
        mWindow.draw(element);

    for (auto& element : mKeysAnimation)
        mWindow.draw(element);
}

void Button::handleEvent(sf::Event event)
{
    mKeysSprite.resize(Settings::ButtonAmount);
    mKeysAnimation.resize(Settings::ButtonAmount);

    setupTexture();
    setupAnimation();
}

void Button::handleInput(std::vector<bool>& needToBeReleased)
{
    for (size_t i = 0; i < Settings::ButtonAmount; ++i)
        if (needToBeReleased[i])
            mKeysAnimation[i].setFillColor(Settings::AnimationColor
                                         + Settings::AnimationOnClickTransparency);
}

void Button::setupTexture()
{
    setTextures();
    setColor();
    scaleTexture();
    setPositions();
}

void Button::setupAnimation()
{
    for (auto& element : mKeysAnimation)
        element.setSize(sf::Vector2f(mKeysSprite[0].getGlobalBounds().width
                                   , mKeysSprite[0].getGlobalBounds().height));
    
    for (auto& element : mKeysAnimation)
            element.setFillColor(sf::Color
                                ( Settings::AnimationColor.r
                                , Settings::AnimationColor.g
                                , Settings::AnimationColor.b
                                , 0));
}

void Button::loadTextures(TextureHolder& textureHolder)
{
    mButtonTexture = &textureHolder.get(Textures::KeyButton);
}

void Button::setTextures()
{
    for (auto& element : mKeysSprite)
        element.setTexture(*mButtonTexture);
}

void Button::setColor()
{
    for (auto& element : mKeysSprite)
        element.setColor(Settings::ButtonImageColor);
}

void Button::setPositions()
{
    for (size_t i = 0; i < mKeysSprite.size(); ++i)
    {
        mKeysSprite[i].setPosition(sf::Vector2f(
                                    Settings::Distance * (i + 1)
                                    + mKeysSprite[i].getGlobalBounds().width * i
                                    , Settings::Distance));

        mKeysAnimation[i].setPosition(mKeysSprite[i].getPosition());
    }
}

void Button::scaleTexture()
{
    for (auto& element : mKeysSprite)
    {
        element.scale(float (Settings::ButtonTextureSize.x
                        / element.getGlobalBounds().width)
                    , float (Settings::ButtonTextureSize.y
                        / element.getGlobalBounds().height));
    }
}