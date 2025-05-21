#pragma once

#include "LogicalParameter.hpp"
#include "ParameterLine.hpp"
#include "Button.hpp"
#include "Menu.hpp"

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>

#include <string>
#include <vector>
#include <queue>
#include <map>


namespace ConfigHelper
{

extern const std::string separationSign;
    
bool readConfig(Menu::ParametersContainer &parameters, const std::vector<std::string> &collectionNames);
void readParameters(Menu::ParametersContainer &parameters, const std::vector<std::string> &collectionNames);
std::queue<LogKey> getLogKeys();
std::queue<LogKey> oldGetLogKeys();
std::queue<LogKey> oldGetLogButtons();
std::string scanParameterValue(const std::string &par, bool &parameterFound, bool &parameterEmpty, std::string collection);
void readParameter(LogicalParameter &par, std::string collection);
float readDigitParameter(const LogicalParameter &par, std::string &valStr);
sf::Vector2f readVectorParameter(const LogicalParameter &par, const std::string &valStr);
sf::Color readColorParameter(const LogicalParameter &par, const std::string &valStr);
bool readBoolParameter(const LogicalParameter &par, const std::string &valStr);
void controlAssets(Menu::ParametersContainer &parameters);

std::queue<LogKey> readKeys(const std::string &keysStr, const std::string &visualKeysStr);
std::queue<LogKey> oldReadKeys(const std::string &keysStr, const std::string &visualKeysStr);
std::queue<LogKey> oldReadButtons(const std::string &valStr, const std::string &visualValStr);

void createDefaultConfig();
void saveConfig(
    const Menu::ParametersContainer &parameters, const Menu::ParameterLinesContainer &parameterLines,
    const std::vector<std::unique_ptr<Button>> *mKeys, bool saveKeys);

std::string getKeysStr(const std::vector<std::unique_ptr<Button>> &mKeys, bool readRealStr);

} // !namespace ConfigHelper