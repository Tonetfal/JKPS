#include "../Headers/ConfigHelper.hpp"
#include "../Headers/StringHelper.hpp"
#include "../Headers/DefStrs.hpp"
#include "../Headers/Settings.hpp"

#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <fstream>
#include <array>


static std::string getReadingErrMsg(const LogicalParameter &par)
{
    return "Failed to read " + par.mParName + ". Default value will be set.\n";
}

static std::string getOutOfBoundsErrMsg(const LogicalParameter &par)
{
    return "Input value is out of bounds - " + par.mParName + " cannot be less than " + std::to_string(par.mLowLimits) + 
        " or more than " + std::to_string(par.mHighLimits) + ". Default value will be set.\n";
}

static std::string cfgPath("JKPS.cfg");
static std::string tmpCfgPath("tmpJKPS.cfg");
static std::string errLogPath("JKPS Error Log.txt");
static std::ifstream ifCfg;
static std::ofstream ofCfg;
static std::ifstream ifErrLog;
static std::ofstream ofErrLog;

namespace ConfigHelper
{

const unsigned minKeys(0);
const unsigned maxKeys(10);
const unsigned minButtons(0);
const unsigned maxButtons(5);

void addDefaultKeys()
{
    Settings::LogicalKeys.emplace_back(new LogicalKey(keyToStr(sf::Keyboard::Z), keyToStr(sf::Keyboard::Z), sf::Keyboard::Z));
    Settings::LogicalKeys.emplace_back(new LogicalKey(keyToStr(sf::Keyboard::X), keyToStr(sf::Keyboard::X), sf::Keyboard::X));
    Settings::ButtonAmount = Settings::LogicalKeys.size();
}

void readConfig(
    std::map<LogicalParameter::ID, std::shared_ptr<LogicalParameter>> &parameters, 
    std::map<ParameterLine::ID, std::shared_ptr<ParameterLine>> &parameterLines)
{
    // Remove error log if there is already one
    ifErrLog.open(errLogPath);
    if (ifErrLog.is_open())
    {
        ifErrLog.close();
        remove(errLogPath.c_str());
    }

    // Open config / Generate default one
    ifCfg.open(cfgPath);
    if (!ifCfg.is_open())
    {
        std::cerr << "Failed to open the config. Default config will be generated.\n";
        addDefaultKeys();
        return;
    }

    // Create new error log for further warnings
    ofErrLog.open(errLogPath);

    // Read all the config
    fillParameters(parameters);
    Settings::LogicalKeys.clear();
    fillKeys(Settings::LogicalKeys);
    Settings::LogicalButtons.clear();
    fillButtons(Settings::LogicalButtons);

    Settings::ButtonAmount = Settings::LogicalKeys.size() + Settings::LogicalButtons.size();
    // If there are no keys - set 2 defaults
    if (Settings::LogicalKeys.size() == 0 && Settings::LogicalButtons.size() == 0)
        addDefaultKeys();

    checkAssets(parameters);


    // Finish work with files on last method
    ifCfg.close();
    ofErrLog.close();

    // Delete error log if there are no errors
    ifErrLog.open(errLogPath);
    if (ifErrLog.is_open())
    {
        if (ifErrLog.get() == EOF)
        {
            ifErrLog.close();
            remove(errLogPath.c_str());
        }
        else
            ifErrLog.close();
    }
}

void fillParameters(std::map<LogicalParameter::ID, std::shared_ptr<LogicalParameter>> &parameters)
{
    for (auto &pair : parameters)
    {
        if (pair.second->mType == LogicalParameter::Type::Empty 
        ||  pair.second->mType == LogicalParameter::Type::Collection)
            continue;

        writeParameter(*pair.second);
    }
}

void fillKeys(std::vector<std::unique_ptr<LogicalKey>> &keys)
{
    static const std::string parName1 = "Keys";
    static const std::string parName2 = "Visual keys";
    std::string keysStr = readParameter(parName1);
    std::string visualKeysStr = readParameter(parName2);

    if (keysStr == "No" || keysStr == "no" || keysStr == "NO")
        return;

    readKeys(keys, keysStr, visualKeysStr);
}

void fillButtons(std::vector<std::unique_ptr<LogicalButton>> &buttons)
{
    static const std::string parName1 = "Mouse buttons";
    static const std::string parName2 = "Visual buttons";
    std::string buttonsStr = readParameter(parName1);
    std::string visualButtonsStr = readParameter(parName2);

    if (buttonsStr == "No" || buttonsStr == "no" || buttonsStr == "NO")
        return;

    readButtons(buttons, buttonsStr, visualButtonsStr);
}

// Finds the parameter and returns everything after its name and ": ", 
// can also return empty string, if there is no such parameter or if there is no value
std::string readParameter(const std::string &parName)
{
    std::ifstream cfg(cfgPath);

    std::string line;
    unsigned i = 0;

    while (!cfg.eof() && i != parName.length())
    {
        getline(cfg, line);
        // Compare the whole name, but not its value
        for (i = 0; i < parName.length(); ++i)
        {
            if (line[i] != parName[i])
            {
                i = 0;
                break;
            }
        }
    }

    cfg.close();

    if (line.compare(0, parName.size(), parName))
        return "";
    // Does it have 2 characters after? (": " have to be deleted)
    if (line.length() <= parName.length() + 2)
        return "";

    // Remove parameter name, ':' and space after it
    return line.substr(parName.length() + 2, 81);
}

void writeParameter(LogicalParameter &par)
{
    std::string valStr = readParameter(par.mParName);
    if (valStr == "")
    {
        if (ofErrLog.is_open())
            ofErrLog << getReadingErrMsg(par);
        return;
    }
    if (valStr == "Default")
        return;

    switch(par.mType)
    {
        case LogicalParameter::Type::Unsigned: par.setDigit(readDigitParameter(par, valStr)); break;
        case LogicalParameter::Type::Int:      par.setDigit(readDigitParameter(par, valStr)); break;
        case LogicalParameter::Type::Bool:     par.setBool(readBoolParameter(par, valStr));  break;
        case LogicalParameter::Type::Float:    par.setDigit(readDigitParameter(par, valStr)); break;
        case LogicalParameter::Type::String:   par.setString(valStr); break;
        case LogicalParameter::Type::Color:    par.setColor(readColorParameter(par, valStr)); break;
        case LogicalParameter::Type::VectorU: 
        case LogicalParameter::Type::VectorI: 
        case LogicalParameter::Type::VectorF:  par.setVector(readVectorParameter(par, valStr)); break;
        default: break;
    }

    if (par.mType == LogicalParameter::Type::String)
    {
        std::ifstream check(par.getString());
        check.is_open() ? check.close() : par.setString(par.getDefValStr());
    }
}

float readDigitParameter(const LogicalParameter &par, const std::string &valStr)
{
    float val = std::stof(valStr);

    if ((val < par.mLowLimits || val > par.mHighLimits)
    && ofErrLog.is_open())
    {
        ofErrLog << getOutOfBoundsErrMsg(par);
        const std::string defVal = par.getDefValStr();
        val = std::stof(defVal);
    }
    return val;
}

sf::Vector2f readVectorParameter(const LogicalParameter &par, const std::string &valStr)
{
    std::array<float, 2> vec;
    for (unsigned i = 0, strIdx = 0; i < vec.size(); ++i)
    {
        // std::stoi transforms a string into int until it encounters any non numerical character, such as ','
        vec[i] = std::stof(valStr.substr(strIdx, 81));

        if ((vec[i] < par.mLowLimits || vec[i] > par.mHighLimits)
        && ofErrLog.is_open())
        {
            ofErrLog << getOutOfBoundsErrMsg(par);
            const std::string defVal = par.getDefValStr();
            return { std::stof(defVal), std::stof(defVal.substr(defVal.find(','))) };
        }

        // Move index on next value
        strIdx += valStr.substr(strIdx).find(',') + 1;
    }

    return { vec[0], vec[1] };
}

sf::Color readColorParameter(const LogicalParameter &par, const std::string &valStr)
{
    std::array<unsigned char, 4> rgba;
    for (unsigned i = 0, strIdx = 0; i < rgba.size(); ++i)
    {
        // std::stoi transforms a string into int until it encounters any non numerical character, such as ','
        int tmp = std::stoi(valStr.substr(strIdx, 81));

        if ((tmp < par.mLowLimits || tmp > par.mHighLimits)
        && ofErrLog.is_open())
        {
            ofErrLog << getOutOfBoundsErrMsg(par);
            const std::string defVal = par.getDefValStr();
            return {   
                static_cast<unsigned char>(std::stoi(defVal)),
                static_cast<unsigned char>(std::stoi(defVal.substr(defVal.find(',')))),
                static_cast<unsigned char>(std::stoi(defVal.substr(defVal.find(',')).substr(defVal.find(',')))),
                static_cast<unsigned char>(std::stoi(defVal.substr(defVal.find(',')).substr(defVal.find(',')).substr(defVal.find(','))))
            };
        }
        rgba[i] = tmp;

        // If there is no other values
        if (valStr.substr(strIdx).find(',') == std::string::npos)
        {
            // If alpha channel wasn't written
            if (i < 3) 
            {
                rgba[3] = 255;
                break;
            }
        }
        else
        {
            // Move index on next value
            strIdx += valStr.substr(strIdx).find(',') + 1;
        }
    }
    
    return { rgba[0], rgba[1], rgba[2], rgba[3] };
}

bool readBoolParameter(const LogicalParameter &par, const std::string &valStr)
{
    // "true" - 4 characters. "false" - 5 characters
    if (valStr == "true" || valStr == "false"
    ||  valStr == "True" || valStr == "False"
    ||  valStr == "TRUE" || valStr == "FALSE")
    {
        return valStr.size() == 4;
    }
    else if (ofErrLog.is_open())
        ofErrLog << getReadingErrMsg(par);

    return par.getDefValStr().size() == 4;
}

// Check if passed assets are right
void checkAssets(std::map<LogicalParameter::ID, std::shared_ptr<LogicalParameter>> &parameters)
{
    sf::Texture texture;
    sf::Font font;
    const std::string defAssetName = "Default";

    // Textures
    if (Settings::ButtonTexturePath != defAssetName)
        if (!texture.loadFromFile(Settings::ButtonTexturePath))
        {
            ofErrLog << getReadingErrMsg(*parameters.find(LogicalParameter::ID::BtnGfxTxtr)->second);
            parameters.find(LogicalParameter::ID::BtnGfxTxtr)->second->resetToDefaultValue();
        }
    if (Settings::AnimationTexturePath != defAssetName)
        if (!texture.loadFromFile(Settings::AnimationTexturePath))
        {
            ofErrLog << getReadingErrMsg(*parameters.find(LogicalParameter::ID::AnimGfxTxtr)->second);
            parameters.find(LogicalParameter::ID::AnimGfxTxtr)->second->resetToDefaultValue();
        }
    if (Settings::BackgroundTexturePath != defAssetName)
        if (!texture.loadFromFile(Settings::BackgroundTexturePath))
        {
            ofErrLog << getReadingErrMsg(*parameters.find(LogicalParameter::ID::BgTxtr)->second);
            parameters.find(LogicalParameter::ID::BgTxtr)->second->resetToDefaultValue();
        }

    // Fonts
    if (Settings::StatisticsFontPath != defAssetName)
        if (!font.loadFromFile(Settings::StatisticsFontPath))
        {
            ofErrLog << getReadingErrMsg(*parameters.find(LogicalParameter::ID::StatTextFont)->second);
            parameters.find(LogicalParameter::ID::StatTextFont)->second->resetToDefaultValue();
        }
    if (Settings::KeyCountersFontPath != defAssetName)
        if (!font.loadFromFile(Settings::KeyCountersFontPath))
        {
            ofErrLog << getReadingErrMsg(*parameters.find(LogicalParameter::ID::BtnTextFont)->second);
            parameters.find(LogicalParameter::ID::BtnTextFont)->second->resetToDefaultValue();
        }
    if (Settings::KPSWindowTextFontPath != defAssetName)
        if (!font.loadFromFile(Settings::KPSWindowTextFontPath))
        {
            ofErrLog << getReadingErrMsg(*parameters.find(LogicalParameter::ID::KPSWndwTxtFont)->second);
            parameters.find(LogicalParameter::ID::KPSWndwTxtFont)->second->resetToDefaultValue();
        }
    if (Settings::KPSWindowNumberFontPath != defAssetName)
        if (!font.loadFromFile(Settings::KPSWindowNumberFontPath))
        {
            ofErrLog << getReadingErrMsg(*parameters.find(LogicalParameter::ID::KPSWndwNumFont)->second);
            parameters.find(LogicalParameter::ID::KPSWndwNumFont)->second->resetToDefaultValue();
        }
}


void readKeys(std::vector<std::unique_ptr<LogicalKey>> &keys, const std::string &keysStr, const std::string &visualKeysStr)
{
    unsigned strIdx1 = 0, strIdx2 = 0;
    for (unsigned i = 0; strIdx1 < keysStr.size() && i < maxKeys; ++i)
    {
        std::string keyStr = keysStr.substr(strIdx1, keysStr.substr(strIdx1).find(','));
        std::string visualKeyStr = "";
        std::string checkStr;
        if (strIdx2 < visualKeysStr.size())
            visualKeyStr = visualKeysStr.substr(strIdx2, visualKeysStr.substr(strIdx2).find(','));

        sf::Keyboard::Key key = strToKey(keyStr);

        checkStr = keyToStr(key);
        unsigned maxLength = 20;
        if (visualKeyStr.size() > maxLength || visualKeyStr.size() == 0)
            visualKeyStr = checkStr;

        keys.emplace_back(new LogicalKey(keyStr, visualKeyStr, key));
        keys.back()->realStr = checkStr == keyStr ? keyStr : checkStr;

        // // If there is a copy - find a free key for the new one
        // unsigned cpyIdx; // redundant here, since new key is swapped, not the old one
        // while (isKeyAlreadyPresent(keys, keys.back()->key, cpyIdx, i))
        //     keys.back() = getDefaultLogicalKey(keys);

        // If there is no data to read - break
        if (keysStr.find(',', strIdx1) == std::string::npos)
            break;

        // Move index to the beggining of the next key
        strIdx1 = keysStr.find(',', strIdx1) + 1;
        strIdx2 = visualKeysStr.find(',', strIdx2) + 1;
    }
}


bool isKeyAlreadyPresent(const std::vector<std::unique_ptr<LogicalKey>> &keys, sf::Keyboard::Key key, unsigned &cpyIdx, unsigned ignIdx)
{
    cpyIdx = 0;
    for (const auto &elem : keys)
    {
        if (elem->key == key && cpyIdx != ignIdx)
            return true;
        ++cpyIdx;
    }
    return false;
}

std::unique_ptr<LogicalKey> getDefaultLogicalKey(const std::vector<std::unique_ptr<LogicalKey>> &keys)
{
    sf::Keyboard::Key key = getDefaultKey(keys);
    std::string lKeyStr = keyToStr(key);
    std::unique_ptr<LogicalKey> lKey(new LogicalKey(lKeyStr, lKeyStr, key));

    return std::move(lKey);
}

sf::Keyboard::Key getDefaultKey(const std::vector<std::unique_ptr<LogicalKey>> &keys)
{
    unsigned key = sf::Keyboard::A;

    for (auto it = keys.begin(); it != keys.end(); ++it)
    {
        if (it->get()->key == key)
        {
            it = keys.begin() - 1;
            ++key;
        }
    }

    return static_cast<sf::Keyboard::Key>(key);
}

void readButtons(std::vector<std::unique_ptr<LogicalButton>> &buttons, const std::string &buttonsStr, const std::string &visualButtonsStr)
{
    unsigned strIdx1 = 0, strIdx2 = 0;
    for (unsigned i = 0; strIdx1 < buttonsStr.size() && i < maxButtons; ++i)
    {
        std::string buttonStr = buttonsStr.substr(strIdx1, buttonsStr.substr(strIdx1).find(','));
        std::string visualButtonStr = "";
        std::string checkStr;
        if (strIdx2 < visualButtonsStr.size())
            visualButtonStr = visualButtonsStr.substr(strIdx2, visualButtonsStr.substr(strIdx2).find(','));

        sf::Mouse::Button button = strToBtn(buttonStr);

        checkStr = btnToStr(button);
        unsigned maxLength = 20;
        if (visualButtonStr.size() > maxLength || visualButtonStr.size() == 0)
            visualButtonStr = checkStr;

        buttons.emplace_back(new LogicalButton(buttonStr, visualButtonStr, button));
        buttons.back()->realStr = checkStr == buttonStr ? buttonStr : checkStr;

        // // If there is a copy - find a free button for the new one
        // unsigned cpyIdx; // redundant here, since new button is swapped, not the old one
        // while (isButtonAlreadyPresent(buttons, buttons.back()->button, cpyIdx, i))
        //     buttons.back() = getDefaultLogicalButton(buttons);

        // If there is no data to read - break
        if (buttonsStr.find(',', strIdx1) == std::string::npos)
            break;

        // Move index to the beggining of the next key
        strIdx1 = buttonsStr.find(',', strIdx1) + 1;
        strIdx2 = visualButtonsStr.find(',', strIdx2) + 1;
    }
}

bool isButtonAlreadyPresent(const std::vector<std::unique_ptr<LogicalButton>> &buttons, sf::Mouse::Button button, unsigned &cpyIdx, unsigned ignIdx)
{
    cpyIdx = 0;
    for (const auto &elem : buttons)
    {
        if (elem->button == button && cpyIdx != ignIdx)
            return true;
        ++cpyIdx;
    }
    return false;
}

std::unique_ptr<LogicalButton> getDefaultLogicalButton(const std::vector<std::unique_ptr<LogicalButton>> &buttons)
{
    sf::Mouse::Button button = getDefaultButton(buttons);
    std::string lKeyStr = btnToStr(button);
    std::unique_ptr<LogicalButton> lButton(new LogicalButton(lKeyStr, lKeyStr, button));

    return std::move(lButton);
}

sf::Mouse::Button getDefaultButton(const std::vector<std::unique_ptr<LogicalButton>> &buttons)
{
    unsigned button = sf::Mouse::Left;

    for (auto it = buttons.begin(); it != buttons.end(); ++it)
    {
        if (it->get()->button == button)
        {
            it = buttons.begin() - 1;
            ++button;
        }
    }

    return static_cast<sf::Mouse::Button>(button);
}

void saveConfig(
    const std::map<LogicalParameter::ID, std::shared_ptr<LogicalParameter>> &parameters, 
    const std::map<ParameterLine::ID, std::shared_ptr<ParameterLine>> &parameterLines)
{
    ofCfg.open(tmpCfgPath);

    ofCfg << defCfgComment;
    ofCfg << "\n";

    ofCfg << "[Keys]\nKeys: ";
    for (const auto &key : Settings::LogicalKeys)
    {
        ofCfg << key->realStr;
        if (key != Settings::LogicalKeys.back())
            ofCfg << ",";
    }
    if (Settings::LogicalKeys.size() == 0)
        ofCfg << "No";
    ofCfg << "\n\n";

    ofCfg << "[Visual keys]\nVisual keys: ";
    for (const auto &key : Settings::LogicalKeys)
    {
        ofCfg << key->visualStr;
        if (key != Settings::LogicalKeys.back())
            ofCfg << ",";
    }
    if (Settings::LogicalKeys.size() == 0)
        ofCfg << "No";
    ofCfg << "\n\n";

    ofCfg << "[Mouse buttons]\nMouse buttons: ";
    for (const auto &button : Settings::LogicalButtons)
    {
        ofCfg << button->realStr;
        if (button != Settings::LogicalButtons.back())
            ofCfg << ",";
    }
    if (Settings::LogicalButtons.size() == 0)
        ofCfg << "No";
    ofCfg << "\n\n";

    ofCfg << "[Visual buttons]\nVisual buttons: ";
    for (const auto &button : Settings::LogicalButtons)
    {
        ofCfg << button->visualStr;
        if (button != Settings::LogicalButtons.back())
            ofCfg << ",";
    }
    if (Settings::LogicalButtons.size() == 0)
        ofCfg << "No";
    ofCfg << "\n\n";

    auto parmPair = parameters.begin();
    auto parmLinePair = parameterLines.begin();
    while (parmPair != parameters.end() || parmLinePair != parameterLines.end())
    {
        typedef std::shared_ptr<LogicalParameter> Ptr;
        Ptr parP = parmPair != parameters.end() ? parmPair->second : nullptr;
        Ptr parP2 = parmLinePair != parameterLines.end() ? parmLinePair->second->getParameter() : nullptr;
        Ptr mainParP = nullptr;

        if (parP == parP2)
        {
            mainParP = parP;
            ++parmPair; ++parmLinePair;
        } 
        
        // Write Empty or Collection first, since any collection doesn't end up with a String
        if (parmLinePair != parameterLines.end()
        &&  parP2->mType == LogicalParameter::Type::Empty 
        ||  parP2->mType == LogicalParameter::Type::Collection)
        {
            mainParP = parP2;
            ++parmLinePair;
        } 
        else if (parmPair != parameters.end() && parP != parP2) 
        {
            mainParP = parP;
            ++parmPair;
        }

        ofCfg << mainParP->mParName;
        if (mainParP->mType != LogicalParameter::Type::Empty 
        &&  mainParP->mType != LogicalParameter::Type::Collection)
            ofCfg << ": " << mainParP->getValStr();
        ofCfg << "\n";
    }

    ofCfg.close();

    std::ofstream ofCfgCheck(cfgPath, std::fstream::in);
    if (ofCfgCheck.is_open())
    {
        ofCfgCheck.close();
        remove(cfgPath.c_str());
    }

    rename(tmpCfgPath.c_str(), cfgPath.c_str());
}

} // !namespace ConfigHelper

