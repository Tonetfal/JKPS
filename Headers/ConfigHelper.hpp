#pragma once

#include "LogicalParameter.hpp"
#include "ParameterLine.hpp"
#include "Button.hpp"

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>

#include <string>
#include <vector>
#include <map>


namespace ConfigHelper
{
    
void readConfig(
    std::map<LogicalParameter::ID, std::shared_ptr<LogicalParameter>> &parameters, 
    std::map<ParameterLine::ID, std::shared_ptr<ParameterLine>> &parameterLines);
void fillParameters(std::map<LogicalParameter::ID, std::shared_ptr<LogicalParameter>> &parameters);
void fillKeys(std::vector<sf::Keyboard::Key> &keys);
void fillButtons(std::vector<sf::Mouse::Button> &buttons);
std::string readParameter(const std::string &par);
void writeParameter(LogicalParameter &par);
float readDigitParameter(const LogicalParameter &par, const std::string &valStr);
sf::Vector2f readVectorParameter(const LogicalParameter &par, const std::string &valStr);
sf::Color readColorParameter(const LogicalParameter &par, const std::string &valStr);
bool readBoolParameter(const LogicalParameter &par, const std::string &valStr);
void checkAssets(std::map<LogicalParameter::ID, std::shared_ptr<LogicalParameter>> &parameters);

void readKeys(std::vector<sf::Keyboard::Key> &keys, const std::string &valStr);
void readButtons(std::vector<sf::Mouse::Button> &keys, const std::string &valStr);
bool isKeyAlreadyPresent(const std::vector<sf::Keyboard::Key> &keys, sf::Keyboard::Key key, unsigned &cpyIdx, unsigned ignIdx);
sf::Keyboard::Key getDefaultKey(const std::vector<sf::Keyboard::Key> &keys);
bool isButtonAlreadyPresent(const std::vector<sf::Mouse::Button> &buttons, sf::Mouse::Button button, unsigned &cpyIdx, unsigned ignIdx);
sf::Mouse::Button getDefaultButton(const std::vector<sf::Mouse::Button> &buttons);

void createDefaultConfig();
void saveConfig(
    const std::map<LogicalParameter::ID, std::shared_ptr<LogicalParameter>> &parameters, 
    const std::map<ParameterLine::ID, std::shared_ptr<ParameterLine>> &parameterLines);

extern const unsigned minKeys;
extern const unsigned maxKeys;
extern const unsigned minButtons;
extern const unsigned maxButtons;

} // !namespace ConfigHelper