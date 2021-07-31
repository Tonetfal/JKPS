#pragma once

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
        Refresh,
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

namespace sf
{
    class Texture;
    class Font;
}

template <typename Resource, typename Identifier>
class ResourceHolder;

typedef ResourceHolder<sf::Texture, Textures::ID> TextureHolder;
typedef ResourceHolder<sf::Font, Fonts::ID> FontHolder;