#pragma once

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/Sprite.hpp>


struct ColorButton : public sf::Drawable, public sf::Transformable
{
    ColorButton(const sf::Texture &texture);

    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

    sf::Sprite rgbCircleSprite;
};
