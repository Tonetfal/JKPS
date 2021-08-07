#include "../Headers/GfxParameter.hpp"
#include "../Headers/StringHelper.hpp"
#include "../Headers/ResourceHolder.hpp"
#include "../Headers/ParameterLine.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

#include <cassert>


const sf::Color GfxParameter::defaultRectColor(sf::Color(120,120,120));
const sf::Color GfxParameter::defaultAimedRectColor(sf::Color(160,160,160));
const sf::Color GfxParameter::defaultSelectedRectColor(sf::Color(200,200,200));
const TextureHolder *GfxParameter::mTextures(nullptr);
const FontHolder *GfxParameter::mFonts(nullptr);

// All types, except Bool
GfxParameter::GfxParameter(const ParameterLine *parent, const std::string &str, unsigned n, sf::Vector2f rectSize)
: mParent(parent)
{
    assert(mTextures && mFonts);

    mRect.setSize(rectSize);
    mRect.setOrigin(mRect.getSize() / 2.f);
    mRect.setFillColor(defaultRectColor);

    mValText.setFont(mFonts->get(Fonts::Value));
    mValText.setString(str);
    setupValPos();

    float distance = 80;
    setPosition(distance * n, 0);
}

// Bool type
GfxParameter::GfxParameter(const ParameterLine *parent, bool b)
: mParent(parent)
{
    mValText.setString(b ? "True" : "False");
    setRightTexture();
    mSprite.setOrigin(static_cast<sf::Vector2f>(mSprite.getTexture()->getSize()) / 2.f);
}

// Tab
GfxParameter::GfxParameter(const std::string &str, unsigned nTab)
: mParent(nullptr)
{
    mRect.setSize(sf::Vector2f(129.133f, 25.f));
    mRect.setFillColor(defaultRectColor);
    mValText.setFont(mFonts->get(Fonts::Value));
    mValText.setString(str);
    mValText.setCharacterSize(16);
    mValText.setOrigin(
        mValText.getLocalBounds().left + mValText.getLocalBounds().width  / 2,
        mValText.getLocalBounds().top  + mValText.getLocalBounds().height / 2);

    setPosition(129.133f * nTab, 0);
    mValText.setPosition(mRect.getSize() / 2.f);
}

GfxParameter::GfxParameter(const ParameterLine *parent)
: mParent(parent)
{
    mSprite.setTexture(mTextures->get(Textures::Refresh));
    mSprite.setOrigin(static_cast<sf::Vector2f>(mSprite.getTexture()->getSize()) / 2.f);
}

void GfxParameter::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    states.transform *= getTransform();

    if (mRect.getSize().x != 0)
    {
        target.draw(mRect, states);
        target.draw(mValText, states);
    }
    else
    {
        target.draw(mSprite, states);
    }
}

void GfxParameter::setupValPos()
{
    mValText.setCharacterSize(20);
    mValText.setOrigin(
        mValText.getLocalBounds().left + mValText.getLocalBounds().width  / 2,
        mValText.getLocalBounds().top  + mValText.getLocalBounds().height / 2);
}


float GfxParameter::getPosX()
{
    unsigned maxValues = 4, valRectWidth = 70, distBetweenEdges = 10, distBetweenOrigins = 80;
    return distBetweenOrigins * (maxValues - 1) + valRectWidth / 2 + distBetweenEdges;
}

bool GfxParameter::contains(sf::Vector2f v2) const
{
    return getGlobalBounds().contains(v2);
}

sf::FloatRect GfxParameter::getGlobalBounds() const
{
    const sf::Vector2f size(mValText.getFont() ? 
        mRect.getSize() : sf::Vector2f(mSprite.getTexture()->getSize()));
    return { getGlobalPosition() - size / 2.f, size };
}

sf::Vector2f GfxParameter::getGlobalPosition() const
{
    return getPosition() + (mParent ? mParent->getPosition() : sf::Vector2f(0,0));
}

void GfxParameter::setInverseMark()
{
    setRightTexture();
}

void GfxParameter::setRightTexture()
{
    assert(mTextures);
    std::string str = static_cast<std::string>(mValText.getString());
    if (str == "True" || str == "true" || str == "TRUE")
        mSprite.setTexture(mTextures->get(Textures::vMark));
    else
        mSprite.setTexture(mTextures->get(Textures::xMark));
}
