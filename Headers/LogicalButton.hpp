#pragma once

#include <SFML/Window/Mouse.hpp>

#include <string>


struct LogicalButton
{
    LogicalButton(const std::string& realStr, const std::string& visualStr, sf::Mouse::Button b)
        : realStr(realStr), visualStr(visualStr), button(b) { }

    std::string realStr;
    std::string visualStr;
    sf::Mouse::Button button;
};