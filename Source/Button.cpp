#include "../Headers/Button.hpp"

Button::Button(sf::RenderWindow& window)
: mWindow(window)
, mKeyCountersFont()
, mKeyCounters()
, mKeyCountersText()
, mButtonsSprite()
, mAnimationSprite()
, mButtonsYOffset()
, mAnimationStyle(AnimationStyle(Settings::AnimationStyle))
{ 
    resizeVectors();
    if (mAnimationStyle > AnimationCounter && mAnimationStyle < 0)
        mAnimationStyle = AnimationCounter;

    // Everything must be initialized regardless the fact that every parameter could be false,
    // because if user enters in edit mode actual keys must be displayed
    setupKeyCounterTextVec();
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
        static const sf::Color animation(0, 0, 0, 255 / Settings::AnimationVelocity);

        if (!needToBeReleased[i])
        {
            switch (mAnimationStyle)
            {
                case Light:
                    fadeKeyLight(i);
                    if (elem->getColor().a != 0)
                        elem->setColor(elem->getColor() - animation);
                    break;
                case Press:
                    raiseKey(i);
                    break;
            }
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
        || (Settings::ShowKeyCountersText && mKeyCounters[i] == 0)
        || sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) // if ShowKeyCountersText is false mKeyCounters is not initialized
        {
            if (i < Settings::KeyAmount)
                strToSet = convertKeyToString(Settings::Keys[i], false);
            else
                strToSet = convertButtonToString(
                    Settings::MouseButtons[i - Settings::KeyAmount]);
        }
        // Display clicks amount
        else if (Settings::ShowKeyCountersText)
            strToSet = std::to_string(mKeyCounters[i]);

        mKeyCountersText[i]->setString(strToSet);

        setupTextPosition(i);
        if (Settings::ShowKeyCountersText)
        {
            mKeyCountersText[i]->setCharacterSize(Settings::KeyCountersTextCharacterSize);
            while (mKeyCountersText[i]->getLocalBounds().width > 
                    Settings::ButtonTextureSize.x - Settings::ButtonTextureSize.x / 10
                || mKeyCountersText[i]->getLocalBounds().height >
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
    resizeVectors();
    if (sf::Keyboard::isKeyPressed(Settings::KeyToIncrease))
        mKeyCounters.back() = 0;
    
    setupKeyCounterTextVec();
    setFonts();

    setupTextures();
}

// User clicks a key
void Button::handleInput(std::vector<bool>& needToBeReleased, KeyPressingManager& container)
{
    if (Settings::ShowKeyCountersText)
        for (size_t i = 0; i < container.mClickedKeys.size(); ++i)
        {
            mKeyCounters[i] += container.mClickedKeys[i] * Settings::ValueToMultiplyOnClick;
        }

    for (size_t i = 0; i < Settings::ButtonAmount; ++i)
    {
        if (needToBeReleased[i])
        {
            switch (mAnimationStyle)
            {
                case Light: lightUpKey(i); break;
                case Press: lowerKey(i); break;
            }
        }
    }
}

void Button::handleHighlight(int buttonIndex)
{
    for (auto& element : mKeyCountersText)
        element->setFillColor(Settings::KeyCountersTextColor);

    if (buttonIndex != -1)
        mKeyCountersText[buttonIndex]->setFillColor(Settings::HighlightedKeyColor);
}

void Button::draw()
{
    sf::Transform transform = sf::Transform::Identity;
    transform.translate(Settings::ButtonDistance, Settings::ButtonDistance);

    for (auto& elem : mButtonsSprite)
        mWindow.draw(*elem, transform);

    for (auto& elem : mAnimationSprite)
        mWindow.draw(*elem, transform);


    // If is changeable set keys must be displayed
    if (Settings::ShowKeyCountersText || Settings::IsChangeable
    ||  Settings::ShowSetKeysText)
        for (auto& elem : mKeyCountersText)
            mWindow.draw(*elem, transform);
}

void Button::loadTextures(TextureHolder& textureHolder)
{
    mButtonTexture = &textureHolder.get(Textures::KeyButton);
    mButtonTexture->setSmooth(true);
    mAnimationTexture = &textureHolder.get(Textures::ButtonAnimation);
    mAnimationTexture->setSmooth(true);
}
void Button::loadFonts(FontHolder& fontHolder)
{
    mKeyCountersFont = nullptr;
    mKeyCountersFont = &fontHolder.get(Fonts::KeyCounters);
    setFonts();
}

void Button::setFonts()
{
    for (auto& element : mKeyCountersText)
        element->setFont(*mKeyCountersFont);
}

void Button::setupTextures()
{
    resizeVectors();

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

void Button::setTextures(std::vector<std::unique_ptr<sf::Sprite>>& vector, sf::Texture& texture)
{
    for (auto& element : vector)
        element->setTexture(texture);
}

void Button::setColor(std::vector<std::unique_ptr<sf::Sprite>>& vector, const sf::Color& color)
{
    for (auto& element : vector)
        element->setColor(color);
}

void Button::scaleTexture(std::vector<std::unique_ptr<sf::Sprite>>& vector, const sf::Vector2u& textureSize)
{
    for (auto& element : vector)
        element->setScale(getDefaultScale());
}

void Button::centerOrigin(std::vector<std::unique_ptr<sf::Sprite>>& vector)
{
    for (auto &elem : vector)
        elem->setOrigin(sf::Vector2f(elem->getTexture()->getSize() / 2U));
}

void Button::setButtonPositions(std::vector<std::unique_ptr<sf::Sprite>>& vector)
{
    for (size_t i = 0; i < vector.size(); ++i)
    {
        sf::Vector2f size(Settings::ButtonTextureSize);
        sf::Vector2f position(
            Settings::ButtonDistance * i + size.x * i + size.x / 2,
            size.y / 2);
        vector[i]->setPosition(position);
    }
}

sf::Vector2f Button::getDefaultScale() const
{
    static float textureWidth = mButtonsSprite[0]->getGlobalBounds().width;
    static float textureHeight = mButtonsSprite[0]->getGlobalBounds().height;
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
    for (size_t i = 0; i < Settings::ButtonAmount; ++i)
    {
        mKeyCountersText[i]->setCharacterSize(Settings::KeyCountersTextCharacterSize);
        mKeyCountersText[i]->setFillColor(Settings::KeyCountersTextColor);
        // Doesn't want to work w/o cast
        sf::Text::Style style(static_cast<sf::Text::Style>(
            (Settings::KeyCountersBold ? sf::Text::Bold : 0) | 
            (Settings::KeyCountersItalic ? sf::Text::Italic : 0)));
        mKeyCountersText[i]->setStyle(style);

        setupTextPosition(i);
    }
}

void Button::setupTextPosition(int index)
{
    sf::Text &elem = *mKeyCountersText[index];
    elem.setOrigin(elem.getGlobalBounds().width / 2.f, elem.getGlobalBounds().height / 2.f);
    elem.setPosition(getKeyCountersWidth(index), 
        getKeyCountersHeight(index) + mButtonsYOffset[index]);
}

void Button::decreaseTextCharacterSize(int index)
{
    mKeyCountersText[index]->setCharacterSize(
        mKeyCountersText[index]->getCharacterSize() - 1);
}

void Button::resizeVectors()
{
    assert (
        mKeyCounters.size() == mButtonsYOffset.size() && 
        mKeyCounters.size() == mKeyCountersText.size() && 
        mKeyCounters.size() == mButtonsSprite.size() && 
        mKeyCounters.size() == mAnimationSprite.size());
    size_t prevSize = mKeyCounters.size();

    mKeyCounters.resize(Settings::ButtonAmount);
    mButtonsYOffset.resize(Settings::ButtonAmount);
    mKeyCountersText.resize(Settings::ButtonAmount);
    mButtonsSprite.resize(Settings::ButtonAmount);
    mAnimationSprite.resize(Settings::ButtonAmount);

    if (prevSize < Settings::ButtonAmount)
    {
        for (size_t i = prevSize; i < Settings::ButtonAmount; i++)
        {
            mButtonsYOffset[i] = 0;
            mKeyCounters[i] = 0;
            mKeyCountersText[i] = std::move(std::unique_ptr<sf::Text>(new sf::Text));
            mButtonsSprite[i] = std::move(std::unique_ptr<sf::Sprite>(new sf::Sprite));
            mAnimationSprite[i] = std::move(std::unique_ptr<sf::Sprite>(new sf::Sprite));
        }
    }
}

void Button::lightUpKey(size_t index)
{
    mAnimationSprite[index]->setColor(Settings::AnimationColor);
    mAnimationSprite[index]->setScale(Settings::ScaledAnimationScale);
    mButtonsSprite[index]->setScale(Settings::ScaledAnimationScale);
    mKeyCountersText[index]->setScale(Settings::AnimationScale);
}

void Button::fadeKeyLight(size_t index)
{
    sf::Sprite &sprite = *mButtonsSprite[index];
    if (getDefaultScale().x != sprite.getScale().x)
    {
        bool back;
        sf::Vector2f scale;
        do {
            back = false;
            if ((getScaleAmountPerFrame().x > 0 ? 
                getDefaultScale().x > sprite.getScale().x :
                getDefaultScale().x < sprite.getScale().x))
            {
                scale = sprite.getScale() + getScaleAmountPerFrame();
                back = false;
            }

            sprite.setScale(scale);
            mAnimationSprite[index]->setScale(scale);
            mKeyCountersText[index]->setScale(mKeyCountersText[index]->getScale() +
                getScaleForText());
            // Re-set position, otherwise the key will go to the left
            setupTextPosition(index);

            // Check if went out of needed scaling
            if ((getScaleAmountPerFrame().x > 0 ?
                getDefaultScale().x < sprite.getScale().x :
                getDefaultScale().x > sprite.getScale().x))
            {
                scale = getDefaultScale();
                back = true;
            }
        } while (back);
    }
}

void Button::raiseKey(size_t index)
{
    static const float offset(Settings::AnimationOffset / 8.f);
    static const float buttonHeight(mButtonsSprite[0]->getPosition().y);
    static const float counterHeight(mKeyCountersText[0]->getPosition().y);

    if (mButtonsSprite[index]->getPosition().y != buttonHeight)
    {
        mButtonsYOffset[index] -= offset;
        mButtonsSprite[index]->move(0.f, -offset);
        mKeyCountersText[index]->move(0.f, -offset);
    }
}

void Button::lowerKey(size_t index)
{
    static const float offset(Settings::AnimationOffset);
    static const float buttonLoweredHeight(mButtonsSprite[0]->getPosition().y + offset);
    static const float counterLoweredHeight(mKeyCountersText[0]->getPosition().y + offset);

    if (mButtonsSprite[index]->getPosition().y != buttonLoweredHeight)
    {
        mButtonsYOffset[index] = offset;
        mButtonsSprite[index]->setPosition(mButtonsSprite[index]->getPosition().x, buttonLoweredHeight);
        mKeyCountersText[index]->setPosition(mKeyCountersText[index]->getPosition().x, counterLoweredHeight);
    }
}

unsigned int Button::getKeyCountersWidth(size_t index) const
{
    const sf::Text &elem(*mKeyCountersText[index]);
    unsigned int buttonCenterX = 
        Settings::ButtonDistance * index +
        Settings::ButtonTextureSize.x * (index + 1) - 
        Settings::ButtonTextureSize.x / 2U;

    return buttonCenterX + elem.getLocalBounds().width * Settings::KeyCounterWidth - elem.getLocalBounds().width;
}

unsigned int Button::getKeyCountersHeight(size_t index) const
{
    const sf::Text &elem(*mKeyCountersText[index]);
    unsigned int buttonCenterY = Settings::ButtonTextureSize.y / 2U;

    return buttonCenterY + elem.getLocalBounds().height * -Settings::KeyCounterHeight + elem.getLocalBounds().height;
}