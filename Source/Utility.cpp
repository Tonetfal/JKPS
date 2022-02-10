#include "../Headers/Utility.hpp"

#include <SFML/System/Vector2.hpp>


namespace Utility
{

sf::Vector2f swapY(sf::Vector2f vec)
{
    return ::sf::Vector2f(vec.x, -vec.y);    
}

} // namespace Utility