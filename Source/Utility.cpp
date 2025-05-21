#include "../Headers/Utility.hpp"

#include <SFML/System/Vector2.hpp>

#include <cassert>
#include <string>
#include <cctype>


namespace Utility
{

sf::Vector2f swapY(sf::Vector2f vec)
{
    return ::sf::Vector2f(vec.x, -vec.y);    
}

// Does not read numbers with a sign in front of it
int retrieveNumber(std::string_view str, std::string_view templ)
{
    // str      "Something 1. Position offset"
    // templ    "Something @. Position offset"
    // Return 1

    const auto found = templ.find("@");
    assert(found != std::string::npos);

    const auto remainder = std::string(str.begin() + found, str.end());
    if (std::isdigit(remainder.front()))
    {
        const auto val = std::stoi(remainder);
        const auto valStr = std::to_string(val);

        const auto leftLhs = std::string(str.begin(), str.begin() + found);
        const auto leftRhs = std::string(templ.begin(), templ.begin() + found);
        const auto rightLhs = std::string(remainder.begin() + static_cast<long int>(valStr.length()), remainder.end());
        const auto rightRhs = std::string(templ.begin() + 1ul, templ.end());

        // Check if strings on both number sides match
        return leftLhs == leftRhs && rightLhs == rightRhs ? val : -1;
    }
    else
        return -1;
}

} // namespace Utility