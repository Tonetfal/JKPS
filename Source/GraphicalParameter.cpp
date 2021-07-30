#include "../Headers/GraphicalParameter.hpp"
#include "../Headers/StringHelper.hpp"
#include "../Headers/ResourceHolder.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

#include <cassert>


sf::Color GraphicalParameter::defaultRectColor(sf::Color(120,120,120));
sf::Color GraphicalParameter::defaultSelectedRectColor(sf::Color(200,200,200));
const TextureHolder *GraphicalParameter::mTextures(nullptr);

// All types, except Bool
GraphicalParameter::GraphicalParameter(const std::string &str, const sf::Font &font, unsigned n, sf::Vector2f rectSize)
{
    mRect.setSize(rectSize);
    mRect.setOrigin(mRect.getSize() / 2.f);
    mRect.setFillColor(defaultRectColor);

    mValText.setFont(font);
    mValText.setString(str);
    setupValPos();

    float distance = 80;
    setPosition(distance * n, 0);
}

// Bool type
GraphicalParameter::GraphicalParameter(const std::string &str)
{
    mValText.setString(str);
    setRightTexture();
    mSprite.setOrigin(static_cast<sf::Vector2f>(mSprite.getTexture()->getSize()) / 2.f);
}

void GraphicalParameter::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    states.transform *= getTransform();

    if (mValText.getFont() != nullptr)
    {
        target.draw(mRect, states);
        target.draw(mValText, states);
    }
    else
    {
        target.draw(mSprite, states);
    }
}

void GraphicalParameter::setupValPos()
{
    mValText.setCharacterSize(20);
    mValText.setOrigin(
        mValText.getLocalBounds().left + mValText.getLocalBounds().width  / 2,
        mValText.getLocalBounds().top  + mValText.getLocalBounds().height / 2);
}


float GraphicalParameter::getPosX()
{
    unsigned maxValues = 4, valRectWidth = 70, distBetweenEdges = 10, distBetweenOrigins = 80;
    return distBetweenOrigins * (maxValues - 1) + valRectWidth / 2 + distBetweenEdges;
}

sf::FloatRect GraphicalParameter::getGlobalBounds() const
{
    if (mValText.getFont() != nullptr)
        return mRect.getGlobalBounds();
    else
        return mSprite.getGlobalBounds();
}

void GraphicalParameter::setInverseMark()
{
    setRightTexture();
}

void GraphicalParameter::setRightTexture()
{
    assert(mTextures);
    std::string str = static_cast<std::string>(mValText.getString());
    if (str == "True" || str == "true" || str == "TRUE")
        mSprite.setTexture(mTextures->get(Textures::vMark));
    else
        mSprite.setTexture(mTextures->get(Textures::xMark));
}
