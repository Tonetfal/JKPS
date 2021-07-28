#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>


// f fraction
float lerp(float a, float b, float f);
sf::Color colorLerp(sf::Color a, sf::Color b, float f);

// a - top-left     d - top-right
// b - bottom-left  c - bottom-right 
template <typename T>
sf::Color bilinearInterp(sf::Color a, 
                         sf::Color b, 
                         sf::Color c, 
                         sf::Color d, 
                         sf::Vector2<T> p)
{
    sf::Color cTop = colorLerp(a, d, p.x);
    sf::Color cBot = colorLerp(b, c, p.x);
    sf::Color cUv = colorLerp(cTop, cBot, p.y);

    return cUv;
}

sf::Color rgb(double ratio);
