#pragma once

#include "ResourceHolder.hpp"
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace Textures
{
    enum ID
    {
        KeyButton,
        Background,
    };
}

namespace Fonts
{
    enum ID
    {
        Main,
    };
}

typedef ResourceHolder<sf::Texture, Textures::ID>   TextureHolder;
typedef ResourceHolder<sf::Font, Fonts::ID>         FontHolder;