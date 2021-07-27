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
    Settings::Keys.emplace_back(sf::Keyboard::Z);
    Settings::Keys.emplace_back(sf::Keyboard::X);
    Settings::ButtonAmount = Settings::Keys.size();
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
    fillKeys(Settings::Keys);
    fillButtons(Settings::MouseButtons);

    Settings::ButtonAmount = Settings::Keys.size() + Settings::MouseButtons.size();
    // If there are no keys - set 2 defaults
    if (Settings::Keys.size() == 0 && Settings::MouseButtons.size() == 0)
        addDefaultKeys();


    // Finish work with files on last method
    ifCfg.close();
    ofErrLog.close();

    // Delete error log if there are no errors
    ifErrLog.open(errLogPath);
    if (ifErrLog.is_open())
    {
        if (ifErrLog.get() == EOF)
            remove(errLogPath.c_str());
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

void fillKeys(std::vector<sf::Keyboard::Key> &keys)
{
    static const std::string parName = "Keys";
    std::string keysStr = readParameter(parName);

    if (keysStr == "No" || keysStr == "no" || keysStr == "NO")
        return;

    readKeys(keys, keysStr);
}

void fillButtons(std::vector<sf::Mouse::Button> &buttons)
{
    static const std::string parName = "Mouse buttons";
    std::string buttonsStr = readParameter(parName);

    if (buttonsStr == "No" || buttonsStr == "no" || buttonsStr == "NO")
        return;

    readButtons(buttons, buttonsStr);
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

void readKeys(std::vector<sf::Keyboard::Key> &keys, const std::string &valStr)
{
    unsigned strIdx = 0;
    for (unsigned i = 0; strIdx < valStr.size() && i < maxKeys; ++i)
    {
        // Read key separated by a comma
        std::string keyStr = valStr.substr(strIdx, valStr.substr(strIdx).find(','));
        sf::Keyboard::Key key = strToKey(keyStr);
        keys.emplace_back(key);

        // If there is a copy - find a free key for the new one
        unsigned cpyIdx; // redundant here, since new key is swapped, not the old one
        while (isKeyAlreadyPresent(keys, keys.back(), cpyIdx, i))
        {
            key = getDefaultKey(keys);
        }

        // If there is no data to read - break
        if (valStr.find(',', strIdx) == std::string::npos)
            break;

        // Move index to the beggining of the next key
        strIdx = valStr.find(',', strIdx) + 1;
    }
}

void readButtons(std::vector<sf::Mouse::Button> &buttons, const std::string &valStr)
{
    unsigned strIdx = 0;
    for (unsigned i = 0; strIdx < valStr.size() && i < maxButtons; ++i)
    {
        // Read key separated by a comma
        std::string buttonStr = valStr.substr(strIdx, valStr.substr(strIdx).find(','));
        sf::Mouse::Button button = strToBtn(buttonStr);
        buttons.emplace_back(button);

        // If there is a copy - find a free key for the new one
        unsigned cpyIdx; // redundant here, since new key is swapped, not the old one
        while (isButtonAlreadyPresent(buttons, buttons.back(), cpyIdx, i))
        {
            button = getDefaultButton(buttons);
        }

        // If there is no data to read - break
        if (valStr.find(',', strIdx) == std::string::npos)
            break;

        // Move index to the beggining of the next key
        strIdx = valStr.find(',', strIdx) + 1;
    }
}

bool isKeyAlreadyPresent(const std::vector<sf::Keyboard::Key> &keys, sf::Keyboard::Key key, unsigned &cpyIdx, unsigned ignIdx)
{
    cpyIdx = 0;
    for (const auto &elem : keys)
    {
        if (elem == key && cpyIdx != ignIdx)
            return true;
        ++cpyIdx;
    }
    return false;
}

sf::Keyboard::Key getDefaultKey(const std::vector<sf::Keyboard::Key> &keys)
{
    unsigned key = sf::Keyboard::A;
    for (unsigned i = 0; i < keys.size(); ++i)
    {
        if (keys[i] == key)
        {
            i = -1;
            ++key;
        }
    }

    return static_cast<sf::Keyboard::Key>(key);
}

bool isButtonAlreadyPresent(const std::vector<sf::Mouse::Button> &buttons, sf::Mouse::Button button, unsigned &cpyIdx, unsigned ignIdx)
{
    cpyIdx = 0;
    for (const auto &elem : buttons)
    {
        if (elem == button && cpyIdx != ignIdx)
            return true;
        ++cpyIdx;
    }
    return false;
}

sf::Mouse::Button getDefaultButton(const std::vector<sf::Mouse::Button> &buttons)
{
    unsigned button = sf::Mouse::Left;
    for (unsigned i = 0; i < buttons.size(); ++i)
    {
        if (buttons[i] == button)
        {
            i = -1;
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
    for (auto key : Settings::Keys)
    {
        ofCfg << keyToStr(key, true);
        if (key != Settings::Keys.back())
            ofCfg << ",";
    }
    if (Settings::Keys.size() == 0)
        ofCfg << "No";
    ofCfg << "\n\n";

    ofCfg << "[Mouse buttons]\nMouse buttons: ";
    for (auto button : Settings::MouseButtons)
    {
        ofCfg << btnToStr(button);
        if (button != Settings::MouseButtons.back())
            ofCfg << ",";
    }
    if (Settings::MouseButtons.size() == 0)
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
