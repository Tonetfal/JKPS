#include "../Headers/ColorButton.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/Texture.hpp>


ColorButton::ColorButton(const sf::Texture &texture)
{
    rgbCircleSprite.setTexture(texture);
}

void ColorButton::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    states.transform *= getTransform();
    target.draw(rgbCircleSprite, states);
}