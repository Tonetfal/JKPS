#pragma once

namespace sf
{
    template <typename T>
    class Vector2;

    using Vector2f = Vector2<float>;
} // namespace sf


namespace Utility
{
    sf::Vector2f swapY(sf::Vector2f vec);
} // namespace Utility
