#pragma once

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>

#include <string>


struct LogKey
{
    LogKey(const std::string& realStr, const std::string& visualStr, sf::Keyboard::Key *key, sf::Mouse::Button *button);

    bool isPressed() const;

    std::string realStr;
    std::string visualStr;
    sf::Keyboard::Key *keyboardKey;
    sf::Mouse::Button *mouseButton;
};