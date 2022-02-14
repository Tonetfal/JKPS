#pragma once

#include <string_view>

namespace sf
{
    template <typename T>
    class Vector2;

    using Vector2f = Vector2<float>;
} // namespace sf


namespace Utility
{
    sf::Vector2f swapY(sf::Vector2f vec);

    // Does not read numbers with a sign in front of it
    int retrieveNumber(std::string_view str, std::string_view templ);
} // namespace Utility
