#pragma once

namespace Textures
{
    enum ID
    {
        Button,
        Animation,
        Background,
        rgbCircle,
        vMark,
        xMark,
        Refresh,
        KeyPressVis
    };
}

namespace Fonts
{
    enum ID
    {
        ButtonValue,
        Statistics,
        KPSText,
        KPSNumber,
        Parameter,
        Value,
    };
}

namespace sf
{
    class Texture;
    class Font;
}

template <typename Resource, typename Identifier>
class ResourceHolder;

typedef ResourceHolder<sf::Texture, Textures::ID> TextureHolder;
typedef ResourceHolder<sf::Font, Fonts::ID> FontHolder;