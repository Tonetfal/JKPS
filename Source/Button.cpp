#include "../Headers/Button.hpp"

Button::Button(sf::RenderWindow& window)
: mWindow(window)
, mKeyCountersFont()
, mKeyCounters()
, mKeyCountersText()
, mButtonsSprite(Settings::ButtonAmount)
, mAnimationSprite(Settings::ButtonAmount)

, tmp(2.f)
{ 
    tmp.setOrigin(2.f, 2.f);

    if (Settings::ShowKeyCountersText)
        setupKeyCounterVec();

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
        // Velocity = frames to perform the animation (be careful because the config one is substracted by 60)
        sf::Color animation(0, 0, 0, 255 / Settings::AnimationVelocity);

        if (!needToBeReleased[i])
        {
            // Animation color
            if (elem.getColor().a != 0)
            {
                elem.setColor(elem.getColor() - animation);
            }

            // Animation size
            if (getDefaultScale().x != mButtonsSprite[i].getScale().x)
            {
                bool back;
                sf::Vector2f scale;
                do {
                    back = false;
                    if (getDefaultScale().x > mButtonsSprite[i].getScale().x)
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
                    if (getDefaultScale().x < mButtonsSprite[i].getScale().x)
                    {
                        scale = getDefaultScale();
                        back = true;
                    }
                } while (back);
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
        || (Settings::ShowKeyCountersText && mKeyCounters[i] == 0)) // if ShowKeyCountersText is false mKeyCounters is not initialized
        {
            if (i < Settings::KeyAmount)
                strToSet = convertKeyToString(Settings::Keys[i]);
            else
                strToSet = convertButtonToString(
                    Settings::MouseButtons[i - Settings::KeyAmount]);
        }
        // Display clicks amount
        else if (Settings::ShowKeyCountersText)
            strToSet = std::to_string(mKeyCounters[i]);

        mKeyCountersText[i].setString(strToSet);

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
        setupTextPosition(i);
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
            if (container.mClickedKeys[i] > 0)
                mKeyCounters[i] += 999;//container.mClickedKeys[i];

    for (size_t i = 0; i < Settings::ButtonAmount; ++i)
    {
        if (needToBeReleased[i])
        {
            mAnimationSprite[i].setColor(Settings::AnimationColor);

            mAnimationSprite[i].setScale(Settings::ScaledAnimationScale);
            mButtonsSprite[i].setScale(Settings::ScaledAnimationScale);
            mKeyCountersText[i].setScale(Settings::AnimationScale);
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

    tmp.setPosition(mKeyCountersText[0].getPosition());
    mWindow.draw(tmp);
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
    mKeyCountersText[index].setPosition(sf::Vector2f(
        getKeyCountersWidth(index), getKeyCountersHeight(index)));
}

void Button::decreaseTextCharacterSize(int index)
{
    mKeyCountersText[index].setCharacterSize(
        mKeyCountersText[index].getCharacterSize() - 1);
}

unsigned int Button::getKeyCountersWidth(size_t index) const
{
    unsigned int buttonCenterX = 
        Settings::ButtonTextureSize.x * (index + 1) - 
        Settings::ButtonTextureSize.x / 2U +
        Settings::ButtonDistance * (index + 1);

    const sf::Text &elem(mKeyCountersText[index]);
    return buttonCenterX - elem.getLocalBounds().width * elem.getScale().x / 1.9f;
}

unsigned int Button::getKeyCountersHeight(size_t index) const
{
    unsigned int buttonCenterY = Settings::ButtonTextureSize.y / 2U + Settings::ButtonDistance;

    const sf::Text &elem(mKeyCountersText[index]);
    return buttonCenterY - elem.getLocalBounds().height * elem.getScale().y  / 1.4f;
        // 1.4f is value by eye
}
