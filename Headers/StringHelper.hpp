#pragma once
 
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>

#include <string>


unsigned readAmountOfParms(const std::string &str);
std::string readValue(const std::string &str, unsigned n);
void addChOnIdx(std::string &str, unsigned idx, char ch);
void rmChOnIdx(std::string &str, unsigned idx);

std::string keyToStr(sf::Keyboard::Key key, bool saveToCfg = false);
sf::Keyboard::Key strToKey(const std::string &str);
std::string btnToStr(sf::Mouse::Button button);
sf::Mouse::Button strToBtn(const std::string &str);
char enumKeyToStr(sf::Keyboard::Key key);