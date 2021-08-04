#include "../Headers/LogKey.hpp"

#include <cassert>


LogKey::LogKey(const std::string& realStr, const std::string& visualStr, sf::Keyboard::Key *key, sf::Mouse::Button *button)
: realStr(realStr)
, visualStr(visualStr)
, keyboardKey(key)
, mouseButton(button) 
{ 
    assert((key && !button) || (!key && button));
}

bool LogKey::isPressed() const
{
    if (keyboardKey)
        return sf::Keyboard::isKeyPressed(*keyboardKey);
    else
        return sf::Mouse::isButtonPressed(*mouseButton);
}