#include "../Headers/MathHelper.hpp"

#include <string>
#include <stdexcept>


float lerp(float a, float b, float f)
{
    if (f < 0 || f > 1)
        throw std::runtime_error("std::runtime_error exception: "
            "lerp() cannot work with following fraction " + std::to_string(f) + "\n");

    return a + f * (b - a);
}

sf::Color colorLerp(sf::Color a, sf::Color b, float f)
{
    return { static_cast<sf::Uint8>(lerp(a.r, b.r, f)),
             static_cast<sf::Uint8>(lerp(a.g, b.g, f)),
             static_cast<sf::Uint8>(lerp(a.b, b.b, f)) };
}
