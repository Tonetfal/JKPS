#pragma once
 
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>

#include <iostream>

std::string convertKeyToString(sf::Keyboard::Key key);
sf::Keyboard::Key convertStringToKey(const std::string str);
std::string convertButtonToString(sf::Mouse::Button button);
sf::Mouse::Button convertStringToButton(const std::string str);