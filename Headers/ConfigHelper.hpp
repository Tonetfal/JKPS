#pragma once

#include "LogicalParameter.hpp"
#include "ParameterLine.hpp"
#include "Button.hpp"
#include "LogicalKey.hpp"
#include "LogicalButton.hpp"

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
void fillKeys(std::vector<std::unique_ptr<LogicalKey>> &keys);
void fillButtons(std::vector<std::unique_ptr<LogicalButton>> &buttons);
std::string readParameter(const std::string &par);
void writeParameter(LogicalParameter &par);
float readDigitParameter(const LogicalParameter &par, const std::string &valStr);
sf::Vector2f readVectorParameter(const LogicalParameter &par, const std::string &valStr);
sf::Color readColorParameter(const LogicalParameter &par, const std::string &valStr);
bool readBoolParameter(const LogicalParameter &par, const std::string &valStr);
void controlAssets(std::map<LogicalParameter::ID, std::shared_ptr<LogicalParameter>> &parameters);

void readKeys(std::vector<std::unique_ptr<LogicalKey>> &keys, const std::string &keysStr, const std::string &visualKeysStr);
bool isKeyAlreadyPresent(const std::vector<std::unique_ptr<LogicalKey>> &keys, sf::Keyboard::Key key, unsigned &cpyIdx, unsigned ignIdx);
std::unique_ptr<LogicalKey> getDefaultLogicalKey(const std::vector<std::unique_ptr<LogicalKey>> &keys);
sf::Keyboard::Key getDefaultKey(const std::vector<std::unique_ptr<LogicalKey>> &keys);

void readButtons(std::vector<std::unique_ptr<LogicalButton>> &buttons, const std::string &valStr, const std::string &visualValStr);
bool isButtonAlreadyPresent(const std::vector<std::unique_ptr<LogicalButton>> &buttons, sf::Mouse::Button button, unsigned &cpyIdx, unsigned ignIdx);
std::unique_ptr<LogicalButton> getDefaultLogicalButton(const std::vector<std::unique_ptr<LogicalButton>> &keys);
sf::Mouse::Button getDefaultButton(const std::vector<std::unique_ptr<LogicalButton>> &buttons);

void createDefaultConfig();
void saveConfig(
    const std::map<LogicalParameter::ID, std::shared_ptr<LogicalParameter>> &parameters, 
    const std::map<ParameterLine::ID, std::shared_ptr<ParameterLine>> &parameterLines);

extern const unsigned minKeys;
extern const unsigned maxKeys;
extern const unsigned minButtons;
extern const unsigned maxButtons;

} // !namespace ConfigHelper