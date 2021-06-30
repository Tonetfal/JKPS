#include "../Headers/Button.hpp"

Button::Button(sf::RenderWindow& window)
: mWindow(window)
, mKeyCountersFont()
, mKeyCounters()
, mKeyCountersText()
, mButtonsSprite(Settings::ButtonAmount)
, mAnimationSprite(Settings::ButtonAmount)
{ 

    if (Settings::ShowKeyCountersText)
    {
        setupKeyCounterVec();
    }
    setupButtonsYOffsetVec();
    setupKeyCounterTextVec();
    setupKeyCounterStrVec();
}

void Button::update(std::vector<bool>& needToBeReleased)
{
    updateAnimation(needToBeReleased);
    updateKeyCounters();
}

void Button::updateAnimation(const std::vector<bool>& needToBeReleased)
{
    for (size_t i = 0; i < Settings::ButtonAmount; ++i)
    {
        auto &elem = mAnimationSprite[i];
        // Velocity = frames to perform the animation
        sf::Color animation(0, 0, 0, 255 / Settings::AnimationVelocity);

        if (!needToBeReleased[i])
        {
            // Animation color
            if (elem.getColor().a != 0)
            {
#ifndef PRESS_ANIMATION
                elem.setColor(elem.getColor() - animation);
#endif
            }

            // Animation size
#ifdef PRESS_ANIMATION
            if (true)
            {
                raiseKey(i);
            }
            else
            {
#endif
                if (getDefaultScale().x != mButtonsSprite[i].getScale().x)
                {
                    bool back;
                    sf::Vector2f scale;
                    do {
                        back = false;
                        if ((getScaleAmountPerFrame().x > 0 ? 
                            getDefaultScale().x > mButtonsSprite[i].getScale().x :
                            getDefaultScale().x < mButtonsSprite[i].getScale().x))
                        {
                            scale = mButtonsSprite[i].getScale() + getScaleAmountPerFrame();
                            back = false;
                        }

                        mButtonsSprite[i].setScale(scale);
                        mAnimationSprite[i].setScale(scale);
                        mKeyCountersText[i].setScale(mKeyCountersText[i].getScale() +
                            getScaleForText());
                        // Re-set position, otherwise the key will go to the left
                        mKeyCountersText[i].setPosition(float(getKeyCountersWidth(i)), 
                            float(getKeyCountersHeight(i)));

                        // Check if went out of needed scaling
                        if ((getScaleAmountPerFrame().x > 0 ?
                            getDefaultScale().x < mButtonsSprite[i].getScale().x :
                            getDefaultScale().x > mButtonsSprite[i].getScale().x))
                        {
                            scale = getDefaultScale();
                            back = true;
                        }
                    } while (back);
                }
#ifdef PRESS_ANIMATION
            }
#endif
        }
    }
}

void Button::updateKeyCounters()
{
    for (size_t i = 0; i < Settings::ButtonAmount; ++i)
    {
        // Display keys if...
        std::string strToSet("");
        if (Settings::IsChangeable || Settings::ShowSetKeysText 
        || (Settings::ShowKeyCountersText && mKeyCounters[i] == 0)) // if ShowKeyCountersText is false mKeyCounters is not initialized
        {
            if (i < Settings::KeyAmount)
            {
                strToSet = convertKeyToString(Settings::Keys[i], false);
            }
            else
                strToSet = convertButtonToString(
                    Settings::MouseButtons[i - Settings::KeyAmount]);
        }
        // Display clicks amount
        else if (Settings::ShowKeyCountersText)
            strToSet = std::to_string(mKeyCounters[i]);

        mKeyCountersText[i].setString(strToSet);

        setupTextPosition(i);
        if (Settings::ShowKeyCountersText)
        {
            mKeyCountersText[i].setCharacterSize(Settings::KeyCountersTextCharacterSize);
            while (mKeyCountersText[i].getLocalBounds().width > 
                    Settings::ButtonTextureSize.x - Settings::ButtonTextureSize.x / 10
                || mKeyCountersText[i].getLocalBounds().height > 
                    Settings::ButtonTextureSize.y - Settings::ButtonTextureSize.y / 10)
            {
                decreaseTextCharacterSize(i);
            }
        }
    }
}

// Increase/Decrease key amount
void Button::handleEvent(sf::Event event)
{
    if (sf::Keyboard::isKeyPressed(Settings::KeyToIncrease))
        setupKeyCounterVec(Settings::ButtonAmount - 1);
    
    setupKeyCounterTextVec();
    setFonts();

    mKeyCounters.resize(Settings::ButtonAmount);
    mKeyCountersText.resize(Settings::ButtonAmount);

    mButtonsSprite.resize(Settings::ButtonAmount);
    mAnimationSprite.resize(Settings::ButtonAmount);

    setupTextures();
}

// User clicks a key
void Button::handleInput(std::vector<bool>& needToBeReleased, KeyPressingManager& container)
{
    if (Settings::ShowKeyCountersText)
        for (size_t i = 0; i < container.mClickedKeys.size(); ++i)
            mKeyCounters[i] += container.mClickedKeys[i];

    for (size_t i = 0; i < Settings::ButtonAmount; ++i)
    {
        if (needToBeReleased[i])
        {
#ifndef PRESS_ANIMATION
            mAnimationSprite[i].setColor(Settings::AnimationColor);
#endif
#ifdef PRESS_ANIMATION
            if (true)
            {
                lowerKey(i);
            }
            else
            {
#endif
                mAnimationSprite[i].setScale(Settings::ScaledAnimationScale);
                mButtonsSprite[i].setScale(Settings::ScaledAnimationScale);
                mKeyCountersText[i].setScale(Settings::AnimationScale);
#ifdef PRESS_ANIMATION
            }
#endif
        }
    }
}

void Button::handleHighlight(int buttonIndex)
{
    for (auto& element : mKeyCountersText)
        element.setFillColor(Settings::KeyCountersTextColor);

    if (buttonIndex != -1)
        mKeyCountersText[buttonIndex].setFillColor(Settings::HighlightedKeyColor);
}

void Button::draw()
{
    for (auto& elem : mButtonsSprite)
        mWindow.draw(elem);

    for (auto& elem : mAnimationSprite)
        mWindow.draw(elem);

    // If is changeable set keys must be displayed
    if (Settings::ShowKeyCountersText || Settings::IsChangeable
    ||  Settings::ShowSetKeysText)
        for (auto& elem : mKeyCountersText)
            mWindow.draw(elem);
}

void Button::loadTextures(TextureHolder& textureHolder)
{
    mButtonTexture = &textureHolder.get(Textures::KeyButton);
    mAnimationTexture = &textureHolder.get(Textures::ButtonAnimation);
}
void Button::loadFonts(FontHolder& fontHolder)
{
    mKeyCountersFont = nullptr;
    mKeyCountersFont = &fontHolder.get(Fonts::KeyCounters);
}

void Button::setFonts()
{
    for (auto& element : mKeyCountersText)
        element.setFont(*mKeyCountersFont);
}

void Button::setupTextures()
{
    setTextures(mButtonsSprite, *mButtonTexture);
    setColor(mButtonsSprite, Settings::ButtonTextureColor);
    scaleTexture(mButtonsSprite, Settings::ButtonTextureSize);
    centerOrigin(mButtonsSprite);
    setButtonPositions(mButtonsSprite);

    setTextures(mAnimationSprite, *mAnimationTexture);
    setColor(mAnimationSprite, sf::Color::Transparent);
    scaleTexture(mAnimationSprite, Settings::ButtonTextureSize);
    centerOrigin(mAnimationSprite);
    setButtonPositions(mAnimationSprite);
}

void Button::clear()
{
    for (auto &elem : mKeyCounters)
        elem = 0;
}

void Button::setTextures(std::vector<sf::Sprite>& vector, sf::Texture& texture)
{
    for (auto& element : vector)
        element.setTexture(texture);
}

void Button::setColor(std::vector<sf::Sprite>& vector, const sf::Color& color)
{
    for (auto& element : vector)
        element.setColor(color);
}

void Button::scaleTexture(std::vector<sf::Sprite>& vector, const sf::Vector2u& textureSize)
{
    for (auto& element : vector)
        element.setScale(getDefaultScale());
}

void Button::centerOrigin(std::vector<sf::Sprite>& vector)
{
    for (auto &elem : vector)
        elem.setOrigin(sf::Vector2f(elem.getTexture()->getSize() / 2U));
}

void Button::setButtonPositions(std::vector<sf::Sprite>& vector)
{
    for (size_t i = 0; i < vector.size(); ++i)
    {
        sf::Vector2f size(Settings::ButtonTextureSize);
        sf::Vector2f position(
            Settings::ButtonDistance * (i + 1) + size.x * i + size.x / 2,
            Settings::ButtonDistance + size.y / 2);
        vector[i].setPosition(position);
    }
}

sf::Vector2f Button::getDefaultScale() const
{
    static float textureWidth = mButtonsSprite[0].getGlobalBounds().width;
    static float textureHeight = mButtonsSprite[0].getGlobalBounds().height;
    static sf::Vector2f scale(Settings::ButtonTextureSize.x / textureWidth, 
        Settings::ButtonTextureSize.y / textureHeight);

    // fast solution, but that stinks :(
    static sf::Uint8 sadge = 0;
    if (sadge == 0)
    {
        Settings::ScaledAnimationScale = scale - scale * (1.f - Settings::AnimationScale.x);
        ++sadge;
    }

    return scale;
}

sf::Vector2f Button::getScaleForText() const
{
    float x = (1.0f - Settings::AnimationScale.x) / 
        Settings::AnimationVelocity;

    return {x, x};
}


sf::Vector2f Button::getScaleAmountPerFrame() const
{
    float x = (getDefaultScale().x - Settings::ScaledAnimationScale.x) / 
        Settings::AnimationVelocity;

    return {x, x};
}

void Button::setupKeyCounterTextVec()
{
    mKeyCountersText.resize(Settings::ButtonAmount);

    for (size_t i = 0; i < Settings::ButtonAmount; ++i)
    {
        mKeyCountersText[i].setCharacterSize(Settings::KeyCountersTextCharacterSize);
        mKeyCountersText[i].setFillColor(Settings::KeyCountersTextColor);

        setupTextPosition(i);
    }
}

void Button::setupButtonsYOffsetVec()
{
    mButtonsYOffset.resize(Settings::ButtonAmount);
    for (auto& element : mButtonsYOffset)
        element = 0;
}

void Button::setupKeyCounterVec()
{
    mKeyCounters.resize(Settings::ButtonAmount);
    for (auto& element : mKeyCounters)
        element = 0;
}

void Button::setupKeyCounterVec(size_t index)
{
    mKeyCounters.resize(Settings::ButtonAmount);
    mKeyCounters[index] = 0;
}

void Button::setupKeyCounterStrVec()
{
    for (size_t i = 0; i < Settings::ButtonAmount; ++i)
    {
        mKeyCountersText[i].setString("0");
    }
}

void Button::setupTextPosition(int index)
{
    mKeyCountersText[index].setPosition(getKeyCountersWidth(index), 
        getKeyCountersHeight(index) + mButtonsYOffset[index]);
}

void Button::decreaseTextCharacterSize(int index)
{
    mKeyCountersText[index].setCharacterSize(
        mKeyCountersText[index].getCharacterSize() - 1);
}

void Button::raiseKey(size_t index)
{
    static const float offset(Settings::AnimationOffset / 8.f);
    static const float buttonHeight(mButtonsSprite[0].getPosition().y);
    static const float counterHeight(mKeyCountersText[0].getPosition().y);

    if (mButtonsSprite[index].getPosition().y != buttonHeight)
    {
        mButtonsYOffset[index] -= offset;
        mButtonsSprite[index].move(0.f, -offset);
        mKeyCountersText[index].move(0.f, -offset);
    }
}

void Button::lowerKey(size_t index)
{
    static const float offset(Settings::AnimationOffset);
    static const float buttonLoweredHeight(mButtonsSprite[0].getPosition().y + offset);
    static const float counterLoweredHeight(mKeyCountersText[0].getPosition().y + offset);

    if (mButtonsSprite[index].getPosition().y != buttonLoweredHeight)
    {
        mButtonsYOffset[index] = offset;
        mButtonsSprite[index].setPosition(mButtonsSprite[index].getPosition().x, buttonLoweredHeight);
        mKeyCountersText[index].setPosition(mKeyCountersText[index].getPosition().x, counterLoweredHeight);
    }
}


unsigned int Button::getKeyCountersWidth(size_t index) const
{
    unsigned int buttonCenterX = 
        Settings::ButtonTextureSize.x * (index + 1) - 
        Settings::ButtonTextureSize.x / 2U +
        Settings::ButtonDistance * (index + 1);

    const sf::Text &elem(mKeyCountersText[index]);
    return buttonCenterX - elem.getLocalBounds().width * elem.getScale().x / 1.85f;
        // 1.85f is value by eye
}

unsigned int Button::getKeyCountersHeight(size_t index) const
{
    unsigned int buttonCenterY = Settings::ButtonTextureSize.y / 2U + Settings::ButtonDistance;

    const sf::Text &elem(mKeyCountersText[index]);
    return buttonCenterY - elem.getLocalBounds().height * elem.getScale().y  / 1.2f;
        // 1.2f is value by eye
}