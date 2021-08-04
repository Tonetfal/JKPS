#include "../Headers/GfxButton.hpp"
#include "../Headers/ResourceHolder.hpp"
#include "../Headers/Settings.hpp"
#include "../Headers/Button.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>


GfxButton::GfxButton(const TextureHolder& textureHolder, const FontHolder& fontHolder)
: mTextures(textureHolder)
, mFonts(fontHolder)
, mButtonsHeightOffset(0.f)
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
}

void GfxButton::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    states.transform.translate(getPosition());
    states.transform.scale(getScale());

    for (const auto &sprite : mSprites)
        target.draw(*sprite, states);

    const GfxButton::TextID id = getTextIdToDisplay();
    target.draw(*mTexts[id], states);
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
    mSprites[ButtonSprite]->setColor(Settings::ButtonTextureColor);
    // Substraction by black (0,0,0,255) is needed to set alpha channel on 0 when any related animation key parameter is changed
    mSprites[AnimationSprite]->setColor(Settings::AnimationColor - sf::Color::Black);

    for (auto &text : mTexts)
    {
        text->setFillColor(Settings::ButtonTextColor);
        text->setCharacterSize(Settings::ButtonTextCharacterSize);
        text->setPosition(Settings::ButtonTextPosition);
        text->setStyle(Settings::ButtonTextBold ? sf::Text::Bold : 0 | Settings::ButtonTextItalic ? sf::Text::Italic : 0);
    }
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
    const sf::Vector2f btnTxtrSz(Settings::ButtonTextureSize);
    const sf::Vector2f btnTxtrScale(btnTxtrSz.x / origBtnTxtrSz.x, btnTxtrSz.y / origBtnTxtrSz.y);
    const sf::Vector2f aniTxtrScale(btnTxtrSz.x / origAniTxtrSz.x, btnTxtrSz.y / origAniTxtrSz.y);

    buttonSprite.setScale(btnTxtrScale);
    animationSprite.setScale(aniTxtrScale);
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

unsigned GfxButton::getWidth(unsigned idx)
{
    return Settings::WindowBonusSizeLeft + 
        (Settings::ButtonTextureSize.x + Settings::ButtonDistance) * idx + 
        Settings::ButtonTextureSize.x / 2;
}

unsigned GfxButton::getHeight(unsigned idx)
{
    return Settings::WindowBonusSizeTop + Settings::ButtonTextureSize.y / 2;;
}

GfxButton::TextID GfxButton::getTextIdToDisplay()
{
    if (Settings::ButtonTextShowVisualKeys) 
        return VisualKey;
    if (Settings::ButtonTextShowTotal)
        return KeyCounters;
    if (Settings::ButtonTextShowKPS)
        return KeyPerSecond;
    if (Settings::ButtonTextShowBPM)
        return BeatsPerMinute;
    
    return VisualKey;
}



GfxButton::~GfxButton()
{    
}
