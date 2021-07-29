#include "../Headers/Button.hpp"
#include "../Headers/Menu.hpp"
#include "../Headers/StringHelper.hpp"
#include "../Headers/ResourceHolder.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

#include <cassert>


Button::Button(const TextureHolder& textureHolder, const FontHolder& fontHolder)
: mButtonTexture(&textureHolder.get(Textures::KeyButton))
, mAnimationTexture(&textureHolder.get(Textures::ButtonAnimation))
, mKeyCountersFont(&fontHolder.get(Fonts::KeyCounters))
{ 
    assert(AnimationStyle(Settings::AnimationStyle) >= Light && AnimationStyle(Settings::AnimationStyle) <= Press);

    resizeVectors();
    setupKeyCounterTextVec();
    setupTextures();
    setFonts();
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
        static int a = 0;
        auto &elem = mAnimationSprite[i];
        // Velocity = frames to perform the animation
        const sf::Color animation(0, 0, 0, 255 / Settings::AnimationVelocity);
        
        switch (AnimationStyle(Settings::AnimationStyle))
        {
            case Light:
                if (needToBeReleased[i])
                    lightUpKey(i);
                else
                {
                    fadeKeyLight(i);
                    if (elem->getColor().a != 0)
                        elem->setColor(elem->getColor() - animation);
                }
                break;

            case Press:
                needToBeReleased[i] ? lowerKey(i) : raiseKey(i);
                break;
        }
    }
}

void Button::updateButtonText()
{
    for (size_t i = 0; i < Settings::ButtonAmount; ++i)
    {
        mButtonsText[i]->setString(getButtonText(i));

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
void Button::resize()
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
    for (size_t i = 0; i < container.mClickedKeys.size(); ++i)
        mKeyCounters[i] += container.mClickedKeys[i] * Settings::ValueToMultiplyOnClick;
}

void Button::highlightKey(int buttonIndex)
{
    for (auto& element : mButtonsText)
        element->setFillColor(Settings::KeyCountersTextColor);

    if (buttonIndex != -1)
        mButtonsText[buttonIndex]->setFillColor(Settings::HighlightedKeyColor);
}

void Button::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    states.transform.translate(Settings::WindowBonusSizeLeft, Settings::WindowBonusSizeTop);

    for (auto& elem : mButtonsSprite)
        target.draw(*elem, states);

    for (auto& elem : mAnimationSprite)
        target.draw(*elem, states);

    for (auto& elem : mButtonsText)
        target.draw(*elem, states);
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
    for (auto &elem : mButtonsText)
        elem->setCharacterSize(Settings::KeyCountersTextCharacterSize);
    for (auto &elem : mKeyCounters)
        elem = 0;
}

bool Button::parameterIdMatches(LogicalParameter::ID id)
{
    return 
        id == LogicalParameter::ID::BtnTextClr ||
        id == LogicalParameter::ID::BtnTextChSz ||
        id == LogicalParameter::ID::BtnTextWidth ||
        id == LogicalParameter::ID::BtnTextHeight ||
        id == LogicalParameter::ID::BtnTextHorzBounds ||
        id == LogicalParameter::ID::BtnTextVertBounds ||
        id == LogicalParameter::ID::BtnTextBold ||
        id == LogicalParameter::ID::BtnTextItal ||
        id == LogicalParameter::ID::BtnTextShowKeys ||
        id == LogicalParameter::ID::BtnTextShowKeyCtrs ||
        id == LogicalParameter::ID::BtnGfxDist ||
        id == LogicalParameter::ID::BtnGfxTxtrSz ||
        id == LogicalParameter::ID::BtnGfxTxtrClr ||
        id == LogicalParameter::ID::AnimGfxStl ||
        id == LogicalParameter::ID::AnimGfxVel ||
        id == LogicalParameter::ID::AnimGfxScl ||
        id == LogicalParameter::ID::AnimGfxClr ||
        id == LogicalParameter::ID::AnimGfxOffset ||
        id == LogicalParameter::ID::OtherHighText;
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
    // original size, before scaling 
    static float textureWidth = mButtonsSprite[0]->getGlobalBounds().width;
    static float textureHeight = mButtonsSprite[0]->getGlobalBounds().height;

    sf::Vector2f scale(Settings::ButtonTextureSize.x / textureWidth, 
        Settings::ButtonTextureSize.y / textureHeight);

    Settings::ScaledAnimationScale = sf::Vector2f(
        scale.x - scale.x * (1.f - Settings::AnimationScale.x / 100),
        scale.y - scale.y * (1.f - Settings::AnimationScale.y / 100));

    return scale;
}

sf::Vector2f Button::getScaleForText() const
{
    float x = (1.f - Settings::AnimationScale.x / 100) / 
        Settings::AnimationVelocity;
    float y = (1.f - Settings::AnimationScale.y / 100) / 
        Settings::AnimationVelocity;

    return { x, y };
}

sf::Vector2f Button::getScaleAmountPerFrame() const
{
    float x = (getDefaultScale().x - Settings::ScaledAnimationScale.x) / 
        Settings::AnimationVelocity;
    float y = (getDefaultScale().y - Settings::ScaledAnimationScale.y) / 
        Settings::AnimationVelocity;

    return { x, y };
}

float Button::getTextMaxWidth() const
{
    return Settings::ButtonTextureSize.x - Settings::KeyCountersVerticalBounds * 2.f;
}

float Button::getTextMaxHeight() const
{
    return Settings::ButtonTextureSize.y - Settings::KeyCountersHorizontalBounds * 2.f;
}

void Button::setupKeyCounterTextVec()
{
    for (size_t i = 0; i < Settings::ButtonAmount; ++i)
    {
        mButtonsText[i]->setCharacterSize(Settings::KeyCountersTextCharacterSize);
        mButtonsText[i]->setFillColor(Settings::KeyCountersTextColor);
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

std::string Button::getButtonText(unsigned index)
{
    const unsigned keyAmt = Settings::Keys.size();
    std::string str("");
    // Display keys
    if (Settings::getButtonToChangeIndex() != -1 || Settings::ShowSetKeysText 
    || (Settings::ShowKeyCountersText &&  mKeyCounters[index] == 0)) 
    {
        if (index < keyAmt)
        {
            str = keyToStr(Settings::Keys[index], false);
        }
        else
        {
            str = btnToStr(
                Settings::MouseButtons[index - keyAmt]);
        }
    }
    // Display key counters
    else 
    {
        if (Settings::ShowKeyCountersText)
        {
            str = std::to_string(mKeyCounters[index]);
        }
    }

    // If LCtrl is pressed show the opposite value of the default one
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) 
    &&  !Settings::ShowSetKeysText)
    {
        setupKeyCounterTextVec();
        if (mKeyCounters[index] == 0)
        {
            str = std::to_string(mKeyCounters[index]);
        }
        else if (index < keyAmt)
        {
            str = str = keyToStr(Settings::Keys[index], false);
        }
        else
        {
            str = btnToStr(Settings::MouseButtons[index - keyAmt]);
        }
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) 
    &&  Settings::ShowSetKeysText)
    {
        setupKeyCounterTextVec();
        str = std::to_string(mKeyCounters[index]);
    }

    // If a key is selected, and nothing have to be displayed - display set keys
    if (Settings::getButtonToChangeIndex() != -1 
    && !Settings::ShowSetKeysText && !Settings::ShowKeyCountersText)
    {
        if (index < keyAmt)
        {
            str = str = keyToStr(Settings::Keys[index], false);
        }
        else
        {
            str = btnToStr(
                Settings::MouseButtons[index - keyAmt]);
        }
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) 
    &&  Settings::getButtonToChangeIndex() == -1 
    && !Settings::ShowSetKeysText && !Settings::ShowKeyCountersText)
    {
        if (index < keyAmt)
        {
            str = str = keyToStr(Settings::Keys[index], false);
        }
        else
        {
            str = btnToStr(
                Settings::MouseButtons[index - keyAmt]);
        }
        std::string spaces = "";
        for (unsigned i = 0; i < str.size() / 2; ++i)
            spaces += " ";
        str += "\n" + spaces + std::to_string(mKeyCounters[index]);
    }

    return str;
}


void Button::resizeVectors()
{
    assert(
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
    mButtonsText[index]->setScale(Settings::AnimationScale / 100.f);
}

void Button::fadeKeyLight(size_t index)
{
    sf::Sprite &sprite = *mButtonsSprite[index];
    if (getDefaultScale().x != sprite.getScale().x 
    ||  getDefaultScale().y != sprite.getScale().y)
    {
        sf::Vector2f scale;
        scale = sprite.getScale() + getScaleAmountPerFrame();

        sprite.setScale(scale);
        mAnimationSprite[index]->setScale(scale);
        mButtonsText[index]->setScale(mButtonsText[index]->getScale() +
            getScaleForText());
        // Re-set position, otherwise the key will go to the left
        setupTextPosition(index);
        
        // Check if went out of needed scaling
        if (isBeyondDefaultScale(sprite))
        {
            sprite.setScale(getDefaultScale());
            mAnimationSprite[index]->setScale(getDefaultScale());
            mButtonsText[index]->setScale(sf::Vector2f(1,1));
            setupTextPosition(index);
        }
    }
}

void Button::raiseKey(size_t index)
{
    if (mButtonsYOffset[index] == 0)
        return;

    const float buttonHeight(mButtonsSprite[0]->getGlobalBounds().height / 2.f);
    const float counterHeight(getKeyCountersHeight(0));
    sf::Vector2f spritePos = mButtonsSprite[index]->getPosition();
    float step = Settings::AnimationOffset / Settings::AnimationVelocity;
    
    // Buttons text shouldn't be moved since its position is changed in another place

    mButtonsYOffset[index] -= step;
    mButtonsSprite[index]->move(0.f, -step);

    // If textures went beyond required - reset
    if ((spritePos.y < buttonHeight && Settings::AnimationOffset > 0)
    ||  (spritePos.y > buttonHeight && Settings::AnimationOffset < 0))
    {
        mButtonsYOffset[index] = 0;
        mButtonsSprite[index]->setPosition(spritePos.x, buttonHeight);
        mButtonsText[index]->setPosition(mButtonsText[index]->getPosition().x, counterHeight);
    }
    
}

void Button::lowerKey(size_t index)
{
    const float buttonLoweredHeight(mButtonsSprite[0]->getGlobalBounds().height / 2.f + Settings::AnimationOffset);
    const float counterLoweredHeight(getKeyCountersHeight(0) + Settings::AnimationOffset);

    if (mButtonsSprite[index]->getPosition().y != buttonLoweredHeight)
    {
        mButtonsYOffset[index] = Settings::AnimationOffset;
        mButtonsSprite[index]->setPosition(mButtonsSprite[index]->getPosition().x, buttonLoweredHeight);
        mButtonsText[index]->setPosition(mButtonsText[index]->getPosition().x, buttonLoweredHeight);
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
    unsigned int buttonCenterY = Settings::ButtonTextureSize.y / 2U;

    return buttonCenterY - Settings::KeyCounterHeight;
}

sf::Vector2f Button::getCenterOriginText(unsigned idx) const
{
    sf::Text &e = *mButtonsText[idx];
    return { e.getLocalBounds().left + e.getLocalBounds().width / 2.f , 
             e.getLocalBounds().top + e.getLocalBounds().height / 2.f  };
}

bool Button::isBeyondDefaultScale(const sf::Sprite &sprite) const
{
    // if it has to extend on x axes (!(> 0)), then check if it isn't extended beyond. same thing with y axes
    return 
        (getScaleAmountPerFrame().x > 0 ? 
            getDefaultScale().x < sprite.getScale().x :
            getDefaultScale().x > sprite.getScale().x)
    ||  (getScaleAmountPerFrame().y > 0 ? 
            getDefaultScale().y < sprite.getScale().y :
            getDefaultScale().y > sprite.getScale().y);
}   