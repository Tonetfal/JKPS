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
    };
}

namespace Fonts
{
    enum ID
    {
        KeyCounters,
        Statistics,
        KPS
    };
}

typedef ResourceHolder<sf::Texture, Textures::ID>   TextureHolder;
typedef ResourceHolder<sf::Font, Fonts::ID>         FontHolder;