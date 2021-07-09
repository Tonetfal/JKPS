#include "../Headers/Button.hpp"

Button::Button(sf::RenderWindow& window)
: mWindow(window)
, mKeyCountersFont()
, mKeyCounters()
, mButtonsText()
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
    updateButtonText();
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

void Button::updateButtonText()
{
    for (size_t i = 0; i < Settings::ButtonAmount; ++i)
    {
        mButtonsText[i]->setString(getButtonText(i));
        mButtonsText[i]->setCharacterSize(Settings::KeyCountersTextCharacterSize);

        while ((mButtonsText[i]->getLocalBounds().width > getTextMaxWidth()
             || mButtonsText[i]->getLocalBounds().height > getTextMaxHeight())
             && mButtonsText[i]->getCharacterSize() > 1)
        {
            decreaseTextCharacterSize(i);
        }
        setupTextPosition(i);
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
    for (auto& element : mButtonsText)
        element->setFillColor(Settings::KeyCountersTextColor);

    if (buttonIndex != -1)
        mButtonsText[buttonIndex]->setFillColor(Settings::HighlightedKeyColor);
}

void Button::draw()
{
    sf::Transform transform = sf::Transform::Identity;
    transform.translate(Settings::WindowBonusSizeLeft, Settings::WindowBonusSizeTop);

    for (auto& elem : mButtonsSprite)
        mWindow.draw(*elem, transform);

    for (auto& elem : mAnimationSprite)
        mWindow.draw(*elem, transform);


    // If is changeable set keys must be displayed
    if (Settings::ShowKeyCountersText || Settings::IsChangeable
    ||  Settings::ShowSetKeysText)
        for (auto& elem : mButtonsText)
            mWindow.draw(*elem, transform);
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
    setFonts();
}

void Button::setFonts()
{
    for (auto& element : mButtonsText)
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

float Button::getDefaultTextHeight(unsigned chSz) const
{
    static float defHeight = 0.f;
    static sf::Text tmp;
    if (defHeight == 0.f)
    {
        auto &elem = *mButtonsText[0];
        tmp.setFont(*elem.getFont());
        tmp.setString("0");
    }
    tmp.setCharacterSize(chSz);
    defHeight = tmp.getGlobalBounds().height;

    return defHeight;
}

float Button::getTextMaxWidth() const
{
    return Settings::ButtonTextureSize.x - Settings::KeyCountersHorizontalBounds * 2.f;
}

float Button::getTextMaxHeight() const
{
    return Settings::ButtonTextureSize.y - Settings::KeyCountersVerticalBounds * 2.f;
}

void Button::setupKeyCounterTextVec()
{
    for (size_t i = 0; i < Settings::ButtonAmount; ++i)
    {
        mButtonsText[i]->setCharacterSize(Settings::KeyCountersTextCharacterSize);
        mButtonsText[i]->setFillColor(Settings::KeyCountersTextColor);
        // Doesn't want to work w/o cast
        sf::Text::Style style(static_cast<sf::Text::Style>(
            (Settings::KeyCountersBold ? sf::Text::Bold : 0) | 
            (Settings::KeyCountersItalic ? sf::Text::Italic : 0)));
        mButtonsText[i]->setStyle(style);

        setupTextPosition(i);
    }
}

void Button::setupTextPosition(int idx)
{
    sf::Text &elem = *mButtonsText[idx];

    elem.setOrigin(getCenterOriginText(idx));
    elem.setPosition(getKeyCountersWidth(idx), getKeyCountersHeight(idx) + mButtonsYOffset[idx]);
}

void Button::decreaseTextCharacterSize(int index)
{
    mButtonsText[index]->setCharacterSize(
        mButtonsText[index]->getCharacterSize() - 1);
}

std::string Button::getButtonText(unsigned index) const
{
    // Display keys if...
    std::string str("");
    if (Settings::IsChangeable || Settings::ShowSetKeysText 
    || (Settings::ShowKeyCountersText && mKeyCounters[index] == 0)) // if ShowKeyCountersText is false mKeyCounters is not initialized
    {
        if (index < Settings::KeyAmount)
            str = convertKeyToString(Settings::Keys[index], false);
        else
            str = convertButtonToString(
                Settings::MouseButtons[index - Settings::KeyAmount]);
    }
    // Display key counters
    else 
        if (Settings::ShowKeyCountersText)
            str = std::to_string(mKeyCounters[index]);

    // If LCtrl is pressed show the opposite value of the default one
    if (mWindow.hasFocus())
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) 
        &&  !Settings::ShowSetKeysText)
        {
            if (index < Settings::KeyAmount)
                str = str = convertKeyToString(Settings::Keys[index], false);
            else
                str = convertButtonToString(
                    Settings::MouseButtons[index - Settings::KeyAmount]);
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) 
        &&  Settings::ShowSetKeysText)
                str = std::to_string(mKeyCounters[index]);
    }
    return str;
}


void Button::resizeVectors()
{
    assert (
        mKeyCounters.size() == mButtonsYOffset.size() && 
        mKeyCounters.size() == mButtonsText.size() && 
        mKeyCounters.size() == mButtonsSprite.size() && 
        mKeyCounters.size() == mAnimationSprite.size());
    size_t prevSize = mKeyCounters.size();

    mKeyCounters.resize(Settings::ButtonAmount);
    mButtonsYOffset.resize(Settings::ButtonAmount);
    mButtonsText.resize(Settings::ButtonAmount);
    mButtonsSprite.resize(Settings::ButtonAmount);
    mAnimationSprite.resize(Settings::ButtonAmount);
    mCurDefaultTextHeight.resize(Settings::ButtonAmount);
    mKeyCounterDigits.resize(Settings::ButtonAmount);

    if (prevSize < Settings::ButtonAmount)
    {
        for (size_t i = prevSize; i < Settings::ButtonAmount; i++)
        {
            mButtonsYOffset[i] = mKeyCounters[i] = mCurDefaultTextHeight[i] = mKeyCounterDigits[i] = 0;
            mButtonsText[i] = std::move(std::unique_ptr<sf::Text>(new sf::Text));
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
    mButtonsText[index]->setScale(Settings::AnimationScale);
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
            mButtonsText[index]->setScale(mButtonsText[index]->getScale() +
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
    static const float counterHeight(mButtonsText[0]->getPosition().y);

    if (mButtonsSprite[index]->getPosition().y != buttonHeight)
    {
        mButtonsYOffset[index] -= offset;
        mButtonsSprite[index]->move(0.f, -offset);
        mButtonsText[index]->move(0.f, -offset);
    }
}

void Button::lowerKey(size_t index)
{
    static const float offset(Settings::AnimationOffset);
    static const float buttonLoweredHeight(mButtonsSprite[0]->getPosition().y + offset);
    static const float counterLoweredHeight(mButtonsText[0]->getPosition().y + offset);

    if (mButtonsSprite[index]->getPosition().y != buttonLoweredHeight)
    {
        mButtonsYOffset[index] = offset;
        mButtonsSprite[index]->setPosition(mButtonsSprite[index]->getPosition().x, buttonLoweredHeight);
        mButtonsText[index]->setPosition(mButtonsText[index]->getPosition().x, counterLoweredHeight);
    }
}

unsigned int Button::getKeyCountersWidth(size_t index) const
{
    const sf::Text &elem(*mButtonsText[index]);
    unsigned int buttonCenterX = 
        Settings::ButtonDistance * index +
        Settings::ButtonTextureSize.x * (index + 1) - 
        Settings::ButtonTextureSize.x / 2U;

    return buttonCenterX + Settings::KeyCounterWidth;
}

unsigned int Button::getKeyCountersHeight(size_t index) const
{
    const sf::Text &elem(*mButtonsText[index]);
    unsigned int buttonCenterY = Settings::ButtonTextureSize.y / 2U;

    return buttonCenterY - Settings::KeyCounterHeight;
}

sf::Vector2f Button::getCenterOriginText(unsigned idx) const
{
    sf::Text &e = *mButtonsText[idx];
    return { e.getLocalBounds().left + e.getLocalBounds().width / 2.f , 
             e.getLocalBounds().top + e.getLocalBounds().height / 2.f  };
}
