#pragma once

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

struct GraphicalDebug : public sf::Drawable
{
    GraphicalDebug(
        float radius = 5.f, float outlineThickness = 1.f, 
        sf::Color fillColor = sf::Color::Transparent, 
        sf::Color outlineColor = sf::Color::Magenta)
    {
        origin.setRadius(radius);
        origin.setOrigin(radius, radius);
        origin.setFillColor(outlineColor);
        rectangle.setFillColor(fillColor);
        rectangle.setOutlineThickness(outlineThickness);
        rectangle.setOutlineColor(outlineColor);
    }

    void operator()(const sf::Text &text, sf::Vector2f pos = sf::Vector2f())
    {
        sf::Vector2f textSize(text.getLocalBounds().width, text.getLocalBounds().height);
        sf::Vector2f textCenter(textSize.x / 2.f, textSize.y);
        if (pos != sf::Vector2f())
            textCenter = pos;

        origin.setPosition(textCenter);
        rectangle.setSize(sf::Vector2f(textSize.x * text.getScale().x, textSize.y * text.getScale().y));
        rectangle.setPosition(text.getGlobalBounds().left, text.getGlobalBounds().top);
    }

    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const
    {
        target.draw(origin, states);
        target.draw(rectangle, states);
    }


    sf::CircleShape origin;
    sf::RectangleShape rectangle;
};