#pragma once

#include "ResourceHolder.hpp"
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace Textures
{
    enum ID
    {
        KeyButton,
        ButtonAnimation,
        Background,
        rgbCircle,
        vMark,
        xMark,
    };
}

namespace Fonts
{
    enum ID
    {
        KeyCounters,
        Statistics,
        KPSText,
        KPSNumber,
        Parameter,
        Value,
    };
}

typedef ResourceHolder<sf::Texture, Textures::ID>   TextureHolder;
typedef ResourceHolder<sf::Font, Fonts::ID>         FontHolder;