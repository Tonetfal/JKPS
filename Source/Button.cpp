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
        // Velocity = frames to perform the animation
        const sf::Color animation(0, 0, 0, 255 / Settings::AnimationVelocity);
        
        switch (AnimationStyle(Settings::AnimationStyle))
        {
            case Light:
                if (needToBeReleased[i])
                    lightUpKey(i);
                else
                {
                    auto &elem = mAnimationSprite[i];
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
    unsigned idx = 0;
    for (auto &text : mButtonsText)
    {
        std::string newStr = getButtonText(*text, idx);
        if (text->getString() != newStr)
        {
            text->setString(newStr);
            // Since text on the buttons is decreased when it is too large, and it is not reset automatically,
            // there it is reset because visual key can be less wide then the key counter
            setupKeyCounterText(*text, idx);
        }

        while (isTextTooBig(*text) && text->getCharacterSize() > 0)
        {
            decreaseTextCharacterSize(*text);
        }
        setupTextPosition(*text, idx);
        ++idx;
    }
}

// Increase/Decrease key amount
void Button::resize()
{
    resizeVectors();
    if (sf::Keyboard::isKeyPressed(Settings::KeyToIncreaseKeys))
        mKeyCounters.back() = 0;
    
    setupKeyCounterTextVec();
    setFonts();

    setupTextures();
}

// User clicks a key
void Button::handleInput(std::vector<bool>& needToBeReleased, KeyPressingManager& container)
{
    assert(container.mClickedKeys.size() == mKeyCounters.size());

    const unsigned sz = container.mClickedKeys.size();
    auto keyCountersIt = mKeyCounters.begin();
    auto clickedKeysIt = container.mClickedKeys.begin();

    for (unsigned i = 0; i < sz; ++i)
    {
        *keyCountersIt += *clickedKeysIt * Settings::ValueToMultiplyOnClick;

        ++keyCountersIt;
        ++clickedKeysIt;
    }

    assert(keyCountersIt == mKeyCounters.end() && clickedKeysIt == container.mClickedKeys.end());
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
    unsigned idx = 0;
    for (auto &text : mButtonsText)
    {
        setupKeyCounterText(*text, idx);
        ++idx;
    }
}

void Button::setupKeyCounterText(sf::Text &text, unsigned idx)
{
    text.setCharacterSize(Settings::KeyCountersTextCharacterSize);
    text.setFillColor(Settings::KeyCountersTextColor);
    sf::Text::Style style(static_cast<sf::Text::Style>(
        (Settings::KeyCountersBold ? sf::Text::Bold : 0) | 
        (Settings::KeyCountersItalic ? sf::Text::Italic : 0)));
    text.setStyle(style);

    setupTextPosition(text, idx);
    ++idx;
}

void Button::setupTextPosition(sf::Text &text, unsigned idx)
{
    text.setOrigin(getCenterOriginText(text));
    text.setPosition(getKeyCountersWidth(text, idx), getKeyCountersHeight(text) + mButtonsYOffset[idx]);
}

bool Button::isTextTooBig(const sf::Text &text) const
{
    return text.getLocalBounds().width > getTextMaxWidth() || text.getLocalBounds().height > getTextMaxHeight();
}

void Button::decreaseTextCharacterSize(sf::Text &text)
{
    text.setCharacterSize(text.getCharacterSize() - 1);
}

std::string Button::getButtonText(sf::Text &text, unsigned idx)
{
    const unsigned keyAmt = Settings::LogicalKeys.size();
    const bool lAltPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt);
    const bool changeKey = Settings::getButtonToChangeIndex() != -1;
    std::string str("");

    if (( lAltPressed && !changeKey &&  Settings::ShowSetKeysText &&  Settings::ShowKeyCountersText)
    ||  ( lAltPressed && !changeKey && (Settings::ShowSetKeysText ||  Settings::ShowKeyCountersText)
    &&    mKeyCounters[idx] == 0)
    ||  (!lAltPressed && !changeKey && !Settings::ShowSetKeysText &&  Settings::ShowKeyCountersText
    &&  mKeyCounters[idx] > 0))
    {
        str = std::to_string(mKeyCounters[idx]);
    }
    if (( lAltPressed && !changeKey && !Settings::ShowSetKeysText &&  Settings::ShowKeyCountersText
    &&    mKeyCounters[idx] > 0)
    ||  (!lAltPressed && !changeKey &&  Settings::ShowSetKeysText &&  Settings::ShowKeyCountersText)
    ||  (!lAltPressed && !changeKey && !Settings::ShowSetKeysText &&  Settings::ShowKeyCountersText
    &&    mKeyCounters[idx] == 0) 
    ||    changeKey)
    {
        str = getVisualStr(idx);
    }
    if (lAltPressed && !changeKey && !Settings::ShowSetKeysText && !Settings::ShowKeyCountersText)
    {
        str = getVisualStr(idx);
        std::string spaces = "";
        for (unsigned i = 0; i < str.size() / 2; ++i)
            spaces += " ";
        str += "\n" + spaces + std::to_string(mKeyCounters[idx]);
    }

    return str;
}

std::string Button::getVisualStr(unsigned index)
{
    const unsigned keyAmt = Settings::LogicalKeys.size();

    if (index < keyAmt)
        return Settings::LogicalKeys[index]->visualStr;
    else
        return Settings::LogicalButtons[index - keyAmt]->visualStr;
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

void Button::lightUpKey(unsigned idx)
{
    mAnimationSprite[idx]->setColor(Settings::AnimationColor);
    mAnimationSprite[idx]->setScale(Settings::ScaledAnimationScale);
    mButtonsSprite[idx]->setScale(Settings::ScaledAnimationScale);
    mButtonsText[idx]->setScale(Settings::AnimationScale / 100.f);
}

void Button::fadeKeyLight(unsigned idx)
{
    sf::Sprite &btnSprite = *mButtonsSprite[idx];
    sf::Sprite &animSprite = *mAnimationSprite[idx];
    sf::Text &btnText = *mButtonsText[idx];
    if (getDefaultScale().x != btnSprite.getScale().x 
    ||  getDefaultScale().y != btnSprite.getScale().y)
    {
        sf::Vector2f scale;
        scale = btnSprite.getScale() + getScaleAmountPerFrame();

        btnSprite.setScale(scale);
        animSprite.setScale(scale);
        btnText.setScale(btnText.getScale() + getScaleForText());
        
        // Check if went out of needed scaling
        if (isBeyondDefaultScale(btnSprite))
        {
            btnSprite.setScale(getDefaultScale());
            animSprite.setScale(getDefaultScale());
            btnText.setScale(sf::Vector2f(1,1));
        }
        // Re-set position, otherwise the key will go to the left
        setupTextPosition(btnText, idx);
    }
}

void Button::raiseKey(unsigned idx)
{
    if (mButtonsYOffset[idx] == 0)
        return;

    float &btnYOffset = mButtonsYOffset[idx];
    sf::Sprite &btnSprite = *mButtonsSprite[idx];
    sf::Text &btnText = *mButtonsText[idx];

    const float buttonHeight(mButtonsSprite[0]->getGlobalBounds().height / 2.f);
    const float counterHeight(getKeyCountersHeight(*mButtonsText[0]));

    sf::Vector2f spritePos = btnSprite.getPosition();
    float step = Settings::AnimationOffset / Settings::AnimationVelocity;
    
    // Buttons text shouldn't be moved since its position is changed in another place

    btnYOffset -= step;
    btnSprite.move(0.f, -step);

    // If textures went beyond required - reset
    if ((spritePos.y < buttonHeight && Settings::AnimationOffset > 0)
    ||  (spritePos.y > buttonHeight && Settings::AnimationOffset < 0))
    {
        btnYOffset = 0;
        btnSprite.setPosition(spritePos.x, buttonHeight);
        btnText.setPosition(btnText.getPosition().x, counterHeight);
    }
}

void Button::lowerKey(unsigned idx)
{
    float &btnYOffset = mButtonsYOffset[idx];
    sf::Sprite &btnSprite = *mButtonsSprite[idx];
    sf::Text &btnText = *mButtonsText[idx];

    const float buttonLoweredHeight(mButtonsSprite[0]->getGlobalBounds().height / 2.f + Settings::AnimationOffset);
    const float counterLoweredHeight(getKeyCountersHeight(*mButtonsText[0]) + Settings::AnimationOffset);

    if (btnSprite.getPosition().y != buttonLoweredHeight)
    {
        btnYOffset = Settings::AnimationOffset;
        btnSprite.setPosition(btnSprite.getPosition().x, buttonLoweredHeight);
        btnText.setPosition(btnText.getPosition().x, buttonLoweredHeight);
    }
}

unsigned int Button::getKeyCountersWidth(const sf::Text &text, unsigned idx) const
{
    const sf::Text &elem(*mButtonsText[idx]);
    unsigned int buttonCenterX = 
        Settings::ButtonDistance * idx +
        Settings::ButtonTextureSize.x * (idx + 1) - 
        Settings::ButtonTextureSize.x / 2U;

    return buttonCenterX + Settings::KeyCounterWidth;
}

unsigned int Button::getKeyCountersHeight(const sf::Text &text) const
{
    unsigned int buttonCenterY = Settings::ButtonTextureSize.y / 2U;

    return buttonCenterY - Settings::KeyCounterHeight;
}

sf::Vector2f Button::getCenterOriginText(const sf::Text &text) const
{
    return { text.getLocalBounds().left + text.getLocalBounds().width / 2.f , 
             text.getLocalBounds().top +  text.getLocalBounds().height / 2.f  };
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