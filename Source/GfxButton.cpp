#include "../Headers/GfxButton.hpp"
#include "../Headers/ResourceHolder.hpp"
#include "../Headers/Settings.hpp"
#include "../Headers/Button.hpp"
#include "../Headers/Application.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

#include <algorithm>


bool GfxButton::mShowBounds(false);

GfxButton::GfxButton(const unsigned idx, const TextureHolder& textureHolder, const FontHolder& fontHolder)
: mTextures(textureHolder)
, mFonts(fontHolder)
, mLastKeyState(false)
, mButtonsHeightOffset(0.f)
, mBtnIdx(idx)
{
    for (unsigned i = 0; i < SpriteIdCounter; ++i)
    {
        std::unique_ptr<sf::Sprite> spritePtr(new sf::Sprite);
        mSprites[static_cast<SpriteID>(i)] = std::move(spritePtr);
    }

    for (unsigned i = 0; i < TextIdCounter; ++i)
    {
        std::unique_ptr<sf::Text> textPtr(new sf::Text);
        mTexts[static_cast<TextID>(i)] = std::move(textPtr);
    }

    updateAssets();
    updateParameters();

    mBounds.setFillColor(sf::Color::Transparent);
    mBounds.setOutlineColor(sf::Color::Magenta);
    mBounds.setOutlineThickness(1.f);
    mBounds.setSize(static_cast<sf::Vector2f>(Settings::GfxButtonTextureSize) - Settings::ButtonTextBounds);
    mBounds.setOrigin(mBounds.getSize() / 2.f);
}

void GfxButton::update(bool buttonPressed)
{
    if (Settings::LightAnimation)
    {
        buttonPressed ? lightKey() : fadeKey();
    }

    if (Settings::PressAnimation)
    {
        buttonPressed ? lowerKey() : raiseKey();
    }

    if (Settings::KeyPressVisToggle)
    {
        // Create a new rectangle on button press if last frame the button was not pressed
        if (!mLastKeyState && buttonPressed)
        {
            const auto &buttonSprite = *mSprites[ButtonSprite];
            float width = buttonSprite.getGlobalBounds().width;
            float height = buttonSprite.getGlobalBounds().height;
            sf::RectangleShape rect(sf::Vector2f(width, 0.1f));
            rect.setFillColor(Settings::KeyPressVisColor);
            rect.setOrigin(width / 2.f, height / 2.f);
            mPressRects.push_back(std::move(rect));

            removeOutOfViewPressRects();
        }
        // Make the last rectangle longer if during both last and current frame the key is pressed
        else if (mLastKeyState && buttonPressed)
        {
            auto &rect = mPressRects.back();
            auto oldSize = rect.getSize();
            rect.setSize(sf::Vector2f(oldSize.x, oldSize.y - Settings::KeyPressVisSpeed / 10.f));
        }
    }

    sf::Vector2f movement(0.f, -Settings::KeyPressVisSpeed / 10.f);

    // Move each rectangle by speed
    for (auto &rect : mPressRects)
        rect.move(movement);
        
    // Move last rectangle back
    if (Settings::KeyPressVisToggle && !mPressRects.empty() && buttonPressed)
        mPressRects.back().move(-movement);

    mLastKeyState = buttonPressed;
}

void GfxButton::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    states.transform *= getTransform();
    auto pressRectTransf = states;
    pressRectTransf.transform.rotate(-Settings::KeyPressVisRotation);
    pressRectTransf.transform = pressRectTransf.transform.translate(Settings::KeyPressVisOrig.x, -Settings::KeyPressVisOrig.y);

    for (const auto &rect : mPressRects)
        target.draw(rect, pressRectTransf);
    for (const auto &sprite : mSprites)
        target.draw(*sprite, states);

    if (Settings::ButtonTextSepPosAdvancedMode)
    {
        if (Settings::ButtonTextShowVisualKeys) 
            target.draw(*mTexts[VisualKey], states);
        if (Settings::ButtonTextShowTotal)
            target.draw(*mTexts[KeyCounter], states);
        if (Settings::ButtonTextShowKPS)
            target.draw(*mTexts[KeyPerSecond], states);
        if (Settings::ButtonTextShowBPM)
            target.draw(*mTexts[BeatsPerMinute], states);
    }
    else
    {
        const TextID id = getTextIdToDisplay();
        if (id <= BeatsPerMinute)
            target.draw(*mTexts[id], states);
    }

    if (mShowBounds)
        target.draw(mBounds, states);
}

void GfxButton::lightKey()
{
    mSprites[AnimationSprite]->setColor(Settings::AnimationColor);
    setScale(Settings::AnimationScale / 100.f);
}

void GfxButton::fadeKey()
{
    sf::Sprite &animationSprite = *mSprites[AnimationSprite];
    const sf::Color color(animationSprite.getColor());
    sf::Vector2f scale(getScale());
    if (scale.x == 1 && scale.y == 1 && color.a == 0)
        return;

    const sf::Color animationStep(0, 0, 0, 255 / Settings::AnimationFrames);
    const sf::Vector2f scaleStep(getScaleStep());

    animationSprite.setColor(color - animationStep);
    setScale(getScale() + scaleStep);

    scale = getScale();
    // Scaling can go beyond due to floating point issues
    if ((scaleStep.x > 0 && scale.x > 1) || (scaleStep.x < 0 && scale.x < 1))
        setScale(1, getScale().y);
    if ((scaleStep.y > 0 && scale.y > 1) || (scaleStep.y < 0 && scale.y < 1))
        setScale(getScale().x, 1);
}

void GfxButton::lowerKey()
{
    if (mButtonsHeightOffset == Settings::AnimationOffset)
        return;

    const sf::Vector2f position = getPosition();

    setPosition(position.x, position.y + Settings::AnimationOffset - mButtonsHeightOffset);
    mButtonsHeightOffset = Settings::AnimationOffset;
}

void GfxButton::raiseKey()
{
    if (mButtonsHeightOffset == 0)
        return;

    const sf::Vector2f position = getPosition();
    const float step = getRiseStep();

    setPosition(position.x, position.y - step);
    mButtonsHeightOffset -= step;

    // Offset can go beyond due to floating point issues
    if ((step > 0 && mButtonsHeightOffset < 0) || (step < 0 && mButtonsHeightOffset > 0))
        mButtonsHeightOffset = 0;
}

sf::Vector2f GfxButton::getScaleStep() const
{
    return (sf::Vector2f(1, 1) - Settings::AnimationScale / 100.f) / static_cast<float>(Settings::AnimationFrames);
}

float GfxButton::getRiseStep() const
{
    return Settings::AnimationOffset / Settings::AnimationFrames;
}

void GfxButton::updateAssets()
{
    resetAssets();
    scaleSprites();
    centerOrigins();
}

void GfxButton::updateParameters()
{
    scaleSprites();
    mSprites[ButtonSprite]->setColor(Settings::GfxButtonTextureColor);
    // Substraction by black (0,0,0,255) is needed to set alpha channel on 0 when any related animation key parameter is changed
    mSprites[AnimationSprite]->setColor(Settings::AnimationColor - sf::Color::Black);

    unsigned idx = 0;
    for (auto &text : mTexts)
    {
        const bool isInRange = mBtnIdx < Settings::GfxButtonsBtnPositions.size();
        const sf::Vector2f advancedPos = !Settings::ButtonTextPosAdvancedMode || !isInRange ? 
            sf::Vector2f(Settings::ButtonTextPosition.x, -Settings::ButtonTextPosition.y) : 
            sf::Vector2f(Settings::ButtonsTextPositions[mBtnIdx].x, -Settings::ButtonsTextPositions[mBtnIdx].y);

        text->setFillColor(Settings::ButtonTextColor);
        text->setCharacterSize(Settings::ButtonTextCharacterSize);
        text->setPosition(advancedPos);
        text->setStyle((Settings::ButtonTextBold ? sf::Text::Bold : 0) | (Settings::ButtonTextItalic ? sf::Text::Italic : 0));
        text->setOutlineThickness(Settings::ButtonTextOutlineThickness / 10.f);
        text->setOutlineColor(Settings::ButtonTextOutlineColor);

        if (Settings::ButtonTextSepPosAdvancedMode)
        {
            switch(idx)
            {
                case VisualKey:      text->setPosition(advancedPos + sf::Vector2f(Settings::ButtonVisualKeysTextPosition.x, -Settings::ButtonVisualKeysTextPosition.y)); break;
                case KeyCounter:     text->setPosition(advancedPos + sf::Vector2f(Settings::ButtonTotalTextPosition.x, -Settings::ButtonTotalTextPosition.y)); break;
                case KeyPerSecond:   text->setPosition(advancedPos + sf::Vector2f(Settings::ButtonKPSTextPosition.x, -Settings::ButtonKPSTextPosition.y)); break;
                case BeatsPerMinute: text->setPosition(advancedPos + sf::Vector2f(Settings::ButtonBPMTextPosition.x, -Settings::ButtonBPMTextPosition.y)); break;
            }
        }

        const bool lAlt = sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt);
        if (lAlt && idx == VisualKey && Settings::ButtonTextShowVisualKeys && !Settings::ButtonTextShowTotal)
            text->setPosition(advancedPos + sf::Vector2f(Settings::ButtonTotalTextPosition.x, -Settings::ButtonTotalTextPosition.y));
        if (lAlt && idx == KeyCounter && !Settings::ButtonTextShowVisualKeys && Settings::ButtonTextShowTotal)
            text->setPosition(advancedPos + sf::Vector2f(Settings::ButtonVisualKeysTextPosition.x, -Settings::ButtonVisualKeysTextPosition.y));

        ++idx;
    }

    mBounds.setSize(static_cast<sf::Vector2f>(Settings::GfxButtonTextureSize) - Settings::ButtonTextBounds);
    mBounds.setOrigin(mBounds.getSize() / 2.f);
}

void GfxButton::resetAssets()
{
    mSprites[ButtonSprite]->setTexture(mTextures.get(Textures::Button), true);
    mSprites[AnimationSprite]->setTexture(mTextures.get(Textures::Animation), true);

    const sf::Font &font(mFonts.get(Fonts::ButtonValue));
    for (auto &text : mTexts)
        text->setFont(font);
}

void GfxButton::scaleSprites()
{
    sf::Sprite &buttonSprite = *mSprites[ButtonSprite];
    sf::Sprite &animationSprite = *mSprites[AnimationSprite];
    const sf::Vector2u origBtnTxtrSz(buttonSprite.getTexture()->getSize());
    const sf::Vector2u origAniTxtrSz(animationSprite.getTexture()->getSize());
    const bool isInRange = mBtnIdx < Settings::GfxButtonsBtnPositions.size();
    const sf::Vector2f btnTxtrSz(!Settings::GfxButtonSizesAdvancedMode || !isInRange ? sf::Vector2f(Settings::GfxButtonTextureSize) : Settings::GfxButtonsSizes[mBtnIdx]);
    const sf::Vector2f btnTxtrScale(btnTxtrSz.x / origBtnTxtrSz.x, btnTxtrSz.y / origBtnTxtrSz.y);
    const sf::Vector2f aniTxtrScale(btnTxtrSz.x / origAniTxtrSz.x, btnTxtrSz.y / origAniTxtrSz.y);

    buttonSprite.setScale(btnTxtrScale);
    animationSprite.setScale(aniTxtrScale);
}

void GfxButton::removeOutOfViewPressRects()
{
    sf::FloatRect bounds(sf::Vector2f(), sf::Vector2f(Application::getWindowWidth(), Application::getWindowHeight()));
    sf::Transform transform = getTransform();
    transform.rotate(-Settings::KeyPressVisRotation);
    transform = transform.translate(Settings::KeyPressVisOrig.x, -Settings::KeyPressVisOrig.y);

    mPressRects.erase(std::remove_if(mPressRects.begin(), mPressRects.end(), 
        [transform, bounds](const auto& rect) 
        { return !bounds.intersects(transform.transformRect(rect.getGlobalBounds())); }),
        mPressRects.end());
}

void GfxButton::keepInBounds(sf::Text &text)
{
    const sf::Vector2f bounds(static_cast<sf::Vector2f>(Settings::GfxButtonTextureSize) - Settings::ButtonTextBounds);
    sf::FloatRect rect(text.getLocalBounds());
    unsigned chSz = text.getCharacterSize();;

    while ((rect.width > bounds.x || rect.height > bounds.y) && chSz > 2)
    {
        text.setCharacterSize(chSz - 1);

        chSz = text.getCharacterSize();
        rect = text.getLocalBounds();
    }
}

void GfxButton::centerOrigins()
{
    sf::Sprite &buttonSprite = *mSprites[ButtonSprite];
    sf::Sprite &animationSprite = *mSprites[AnimationSprite];
    buttonSprite.setOrigin(static_cast<sf::Vector2f>(buttonSprite.getTexture()->getSize()) / 2.f);
    animationSprite.setOrigin(static_cast<sf::Vector2f>(animationSprite.getTexture()->getSize()) / 2.f);

    for (auto &text : mTexts)
        text->setOrigin(getTextCenter(*text));
}

sf::Vector2f GfxButton::getTextCenter(const sf::Text &text)
{
    const sf::FloatRect rect = text.getLocalBounds();
    return { rect.left + rect.width / 2, rect.top + rect.height / 2};
}

float GfxButton::getWidth(unsigned idx)
{
    const float width = Settings::WindowBonusSizeLeft + 
        (Settings::GfxButtonTextureSize.x + Settings::GfxButtonDistance) * idx + 
        Settings::GfxButtonTextureSize.x / 2;
    return width;
}

float GfxButton::getHeight(unsigned idx)
{
    const float height = Settings::WindowBonusSizeTop + Settings::GfxButtonTextureSize.y / 2;
    return height;
}

GfxButton::TextID GfxButton::getTextIdToDisplay()
{
    if (Settings::ButtonTextShowVisualKeys) 
        return VisualKey;
    if (Settings::ButtonTextShowTotal)
        return KeyCounter;
    if (Settings::ButtonTextShowKPS)
        return KeyPerSecond;
    if (Settings::ButtonTextShowBPM)
        return BeatsPerMinute;
    
    return Nothing;
}

void GfxButton::setShowBounds(bool b)
{
    mShowBounds = b;
}

GfxButton::~GfxButton()
{    
}
