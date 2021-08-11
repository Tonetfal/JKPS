#pragma once

#include "LogicalParameter.hpp"
#include "ParameterLine.hpp"
#include "Button.hpp"

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>

#include <string>
#include <vector>
#include <queue>
#include <map>


namespace ConfigHelper
{

extern const std::string separationSign;
    
bool readConfig(
    std::map<LogicalParameter::ID, std::shared_ptr<LogicalParameter>> &parameters, 
    std::map<ParameterLine::ID, std::shared_ptr<ParameterLine>> &parameterLines);
void fillParameters(std::map<LogicalParameter::ID, std::shared_ptr<LogicalParameter>> &parameters);
std::queue<LogKey> getLogKeys();
std::queue<LogKey> oldGetLogKeys();
std::queue<LogKey> oldGetLogButtons();
std::string readParameter(const std::string &par, bool &parameterFound, bool &parameterEmpty);
void writeParameter(LogicalParameter &par);
float readDigitParameter(const LogicalParameter &par, const std::string &valStr);
sf::Vector2f readVectorParameter(const LogicalParameter &par, const std::string &valStr);
sf::Color readColorParameter(const LogicalParameter &par, const std::string &valStr);
bool readBoolParameter(const LogicalParameter &par, const std::string &valStr);
void controlAssets(std::map<LogicalParameter::ID, std::shared_ptr<LogicalParameter>> &parameters);

std::queue<LogKey> readKeys(const std::string &keysStr, const std::string &visualKeysStr);
std::queue<LogKey> oldReadKeys(const std::string &keysStr, const std::string &visualKeysStr);
std::queue<LogKey> oldReadButtons(const std::string &valStr, const std::string &visualValStr);

void createDefaultConfig();
void saveConfig(
    const std::map<LogicalParameter::ID, std::shared_ptr<LogicalParameter>> &parameters, 
    const std::map<ParameterLine::ID, std::shared_ptr<ParameterLine>> &parameterLines,
    const std::vector<std::unique_ptr<Button>> *mKeys,
    bool saveKeys);

std::string getKeysStr(const std::vector<std::unique_ptr<Button>> &mKeys, bool readRealStr);

} // !namespace ConfigHelper