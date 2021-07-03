#include "../Headers/Keys.hpp"


sf::Keyboard::Key convertKeysToKey(Keys key)
{
    return static_cast<sf::Keyboard::Key>(key);
}

sf::Mouse::Button convertKeysToButton(Keys key)
{
    return static_cast<sf::Mouse::Button>(key - sf::Keyboard::KeyCount);
}