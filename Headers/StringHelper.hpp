#pragma once
 
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>

#include <string>

unsigned readAmountOfParms(const std::string &str);
std::string readValue(const std::string &str, unsigned n);
void addChOnIdx(std::string &str, unsigned idx, int num);
void rmChOnIdx(std::string &str, unsigned idx);

std::string keyToStr(sf::Keyboard::Key key, bool saveToCfg);
sf::Keyboard::Key strToKey(const std::string &str);
std::string btnToStr(sf::Mouse::Button button);
sf::Mouse::Button strToBtn(const std::string &str);