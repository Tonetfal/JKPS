#pragma once
 
#include <SFML/Window/Keyboard.hpp>

#include <iostream>

std::string convertKeyToString(sf::Keyboard::Key key);
sf::Keyboard::Key convertStringToKey(const std::string str);