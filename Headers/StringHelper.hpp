#pragma once
 
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>

#include <string>

struct LogKey;


unsigned readAmountOfParms(const std::string &str);
std::string readValue(const std::string &str, unsigned n);
void addChOnIdx(std::string &str, unsigned idx, char ch);
void rmChOnIdx(std::string &str, unsigned idx);
std::string eraseDigitsOverHundredths(const std::string &floatStr);

std::string keyToStr(sf::Keyboard::Key key, bool saveToCfg = false);
sf::Keyboard::Key strToKey(const std::string &str);
std::string btnToStr(sf::Mouse::Button button);
sf::Mouse::Button strToBtn(const std::string &str);
std::string logKeyToStr(const LogKey &logKey);
char enumKeyToStr(sf::Keyboard::Key key);