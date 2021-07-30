#pragma once

#include <SFML/Window/Keyboard.hpp>

#include <string>


struct LogicalKey
{
    LogicalKey(const std::string& realStr, const std::string& visualStr, sf::Keyboard::Key key)
        : realStr(realStr), visualStr(visualStr), key(key) { }
        
    std::string realStr;
    std::string visualStr;
    sf::Keyboard::Key key;
};