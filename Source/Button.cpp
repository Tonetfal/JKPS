#include "../Headers/Button.hpp"
#include "../Headers/ResourceHolder.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>


Button::Button(const TextureHolder &textures, const FontHolder &fonts, Keys key)
: mKey(key)
, mCounter(0)
, mIsPressed(false)
, mIsAnimationActive(false)
#ifdef TEXT_DEBUG
, mOrigin(5.f)
, mRectangle()
#endif
{ 
#ifdef TEXT_DEBUG
    mOrigin.setOrigin(5.f, 5.f);
    mRectangle.setFillColor(sf::Color::Transparent);
    mRectangle.setOutlineThickness(1.f);
    mRectangle.setOutlineColor(sf::Color::Magenta);
#endif
    mButtonSprite.setTexture(textures.get(Textures::Button));
    mAnimationSprite.setTexture(textures.get(Textures::Animation));

    setupScaledScale(getDefaultScale());
    scaleTexture();

    // Set origin in the center in order to scale each element
    sf::Vector2f textureCenterOriginal(static_cast<sf::Vector2f>(mButtonSprite.getTexture()->getSize()) / 2.f);
    sf::Vector2f textureCenterScaled(static_cast<sf::Vector2f>(Settings::ButtonTextureSize) / 2.f);
    mButtonSprite.setOrigin(textureCenterOriginal);
    mButtonSprite.setPosition(textureCenterScaled);
    mAnimationSprite.setOrigin(textureCenterOriginal);
    mAnimationSprite.setPosition(textureCenterScaled);
    mAnimationSprite.setColor(sf::Color::Transparent);


    mText.setFont(fonts.get(Fonts::KeyCounter));

    mText.setString(convertKeyToString(mKey, false));
    mText.setCharacterSize(Settings::KeyCountersTextCharacterSize);
    centerText();
    makeFitText();
    sf::Text::Style style(static_cast<sf::Text::Style>(
        (Settings::KeyCountersBold ? sf::Text::Bold : 0) | 
        (Settings::KeyCountersItalic ? sf::Text::Italic : 0)));
}

void Button::updateCurrent(sf::Time dt)
{
    mIsPressed = KeyCombination::isCombinationPressed(mKey);

    if (mIsAnimationActive && !mIsPressed)
    {
        switch (Settings::AnimationStyle)
        {
            case Light: fadeKey(); increaseButtonSize(); break;
            case Press: break;
        }

        centerText();
    }
}

void Button::drawCurrent(sf::RenderTarget &target, sf::RenderStates states) const
{
	target.draw(mButtonSprite, states);
    target.draw(mAnimationSprite, states);
    target.draw(mText, states);
#ifdef TEXT_DEBUG
    target.draw(mOrigin, states);
    target.draw(mRectangle, states);
#endif
}

void Button::pressButton()
{
    switch (Settings::AnimationStyle)
    {
        case Light: lightKey(); reduceButtonSize(); break;
        case Press: break;
    }

    if (!mIsPressed)
    {
        mCounter += 1 * Settings::ValueToMultiplyOnClick;
        mText.setString(std::to_string(mCounter));

        // Reset text ch. size because the overall size can decrease so it will fit
        mText.setCharacterSize(Settings::KeyCountersTextCharacterSize);
        makeFitText();
        centerText();

        mIsPressed = true;
    }
}

void Button::lightKey()
{
    mAnimationSprite.setColor(Settings::AnimationColor);
    mIsAnimationActive = true;
}

void Button::fadeKey()
{
    sf::Color fadeSpeed(0, 0, 0, Settings::AnimationColor.a / Settings::AnimationVelocity);
    mAnimationSprite.setColor(mAnimationSprite.getColor() - fadeSpeed);
    mIsAnimationActive = mAnimationSprite.getColor().a != 0;
    // Finish the animation only when the color is 0, because it is an int,
    // so if velocity is 10, and the alpha channel of the animation is 64,
    // it will take 11 frames to finish the animation because it will add 6 each frame, not 6.4
}

void Button::reduceButtonSize()
{
    mButtonSprite.setScale(Settings::ScaledAnimationScale);
    mAnimationSprite.setScale(Settings::ScaledAnimationScale);
    mText.setScale(Settings::AnimationScale);
}

void Button::increaseButtonSize()
{
    bool back;
    sf::Vector2f scale;
    do
    {
        back = false;
        // Increase only when the animation is not of default size
        if (mButtonSprite.getScale().x != getDefaultScale().x)
        {
            if (mButtonSprite.getScale().x < getDefaultScale().x)
            {
                scale = mButtonSprite.getScale() + getScaleAmountPerFrame();
            }
            mButtonSprite.setScale(scale);
            mAnimationSprite.setScale(scale);
            mText.setScale(mText.getScale() + getScaleAmountPerFrameText());
            centerText();

            // Check if the scale went out of default value
            if (mButtonSprite.getScale().x > getDefaultScale().x)
            {
                scale = getDefaultScale();
                back = true;
            }
        }
    } while(back);
}

Keys Button::getKey() const
{
    return mKey;
}

unsigned Button::getCategory() const
{
    return Category::Button;
}

float Button::getWidth(size_t index)
{
    return (Settings::ButtonTextureSize.x + Settings::ButtonDistance) * index + Settings::ButtonDistance;
}

float Button::getHeight(size_t index)
{
    return Settings::ButtonDistance;
}

void Button::centerText()
{
    sf::Vector2f buttonCenter(static_cast<sf::Vector2f>(Settings::ButtonTextureSize) / 2.f);
    sf::Vector2f textSize(mText.getGlobalBounds().width, mText.getGlobalBounds().height);
    centerOrigin();
    mText.setPosition(buttonCenter);

#ifdef TEXT_DEBUG
    mOrigin.setPosition(mText.getPosition());
    mRectangle.setSize(textSize);
    mRectangle.setPosition(mText.getGlobalBounds().left, mText.getGlobalBounds().top);
#endif

    // mText.setPosition(
    //     buttonCenter.x + textSize.x * Settings::KeyCounterWidth - textSize.x,
    //     buttonCenter.y + textSize.y * -Settings::KeyCounterHeight + textSize.y);
}

void Button::centerOrigin()
{
    sf::Vector2f textSize(mText.getLocalBounds().width, mText.getLocalBounds().height);
    mText.setOrigin(textSize.x / 2.f, textSize.y );
}

void Button::makeFitText()
{
    sf::Vector2f textSize(mText.getLocalBounds().width, mText.getLocalBounds().height);
    sf::Vector2f buttonSize(Settings::ButtonTextureSize);
    while (textSize.x > buttonSize.x || textSize.y > buttonSize.y)
    {
        mText.setCharacterSize(mText.getCharacterSize() - 1);
        textSize = sf::Vector2f(mText.getLocalBounds().width, mText.getLocalBounds().height);
    }
    centerText();
}

// Scale original texture to the size that user input
void Button::scaleTexture()
{
    mButtonSprite.setScale(getDefaultScale());
    mAnimationSprite.setScale(getDefaultScale());
}

void Button::setupScaledScale(sf::Vector2f defaultScale)
{
    Settings::ScaledAnimationScale = defaultScale - defaultScale * 
        (1.f - Settings::AnimationScale.x);
}

// The texture can be scaled in order to be as Settings::ButtonTextureSize
// I have to know that scale in order to increase/decrease its scale and know when I reach the original value
sf::Vector2f Button::getDefaultScale() const
{
    static float textureWidth = mButtonSprite.getLocalBounds().width;
    static float textureHeight = mButtonSprite.getLocalBounds().height;
    static sf::Vector2f scale(Settings::ButtonTextureSize.x / textureWidth, 
        Settings::ButtonTextureSize.y / textureHeight);

    return scale;
}

sf::Vector2f Button::getScaleAmountPerFrame() const
{
    static float x = (getDefaultScale().x - Settings::ScaledAnimationScale.x) / 
        Settings::AnimationVelocity;
    return { x, x };
}

sf::Vector2f Button::getScaleAmountPerFrameText() const
{
    static float x = (1.0f - Settings::AnimationScale.x) / 
        Settings::AnimationVelocity;

    return {x, x};
}
