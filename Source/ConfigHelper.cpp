#include "../Headers/ConfigHelper.hpp"
#include "../Headers/StringHelper.hpp"
#include "../Headers/DefStrs.hpp"
#include "../Headers/Settings.hpp"

#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <fstream>
#include <array>
#include <algorithm>


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

bool readConfig(
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
        return false;
    }

    // Create new error log for further warnings
    ofErrLog.open(errLogPath);

    // Read all the parameters, except keys and buttons
    fillParameters(parameters);
    controlAssets(parameters);


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
    return true;
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

std::queue<LogKey> getLogKeys()
{
    static const std::string parName1 = "Keyboard keys";
    static const std::string parName2 = "Visual keys";
    bool parameterFound = false, parameterEmpty = false, nothing;
    std::string keysStr = readParameter(parName1, parameterFound, parameterEmpty);
    std::string visualKeysStr = readParameter(parName2, nothing, nothing);

    if (!parameterFound || parameterEmpty || keysStr == "No" || keysStr == "no" || keysStr == "NO")
        return { };

    return readKeys(keysStr, visualKeysStr);
}

std::queue<LogKey> getLogButtons()
{
    static const std::string parName1 = "Mouse buttons";
    static const std::string parName2 = "Visual buttons";
    bool parameterFound = false, parameterEmpty = false, nothing;
    std::string buttonsStr = readParameter(parName1, parameterFound, parameterEmpty);
    std::string visualButtonsStr = readParameter(parName2, nothing, nothing);

    if (!parameterFound || parameterEmpty || buttonsStr == "No" || buttonsStr == "no" || buttonsStr == "NO")
        return { };

    return readButtons(buttonsStr, visualButtonsStr);
}

// Finds the parameter and returns everything after its name and ": ", 
// can also return empty string, if there is no such parameter or if there is no value
std::string readParameter(const std::string &parName, bool &parameterFound, bool &parameterEmpty)
{
    std::ifstream cfg(cfgPath);
    assert(cfg.is_open());

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
        if (line[i] != ':')
            i = 0;
    }

    cfg.close();

    if (line.compare(0, parName.size(), parName))
        return "";
    // Does it have 2 characters after? (": " have to be deleted)
    if (line.length() <= parName.length() + 2)
    {
        parameterFound = true;
        parameterEmpty = true;
        return "";
    }

    parameterFound = true;
    parameterEmpty = false;
    // Remove parameter name, ':' and space after it
    return line.substr(parName.length() + 2, 81);
}

void writeParameter(LogicalParameter &par)
{
    bool parameterFound = false;
    bool parameterEmpty = false;
    const std::string valStr = readParameter(par.mParName, parameterFound, parameterEmpty);
    const std::string tmp = par.mParName;
    // only these 4 can be empty
    if ((parameterEmpty && !(tmp == "KPS text" || tmp == "KPS text when it is 0" || tmp == "Total text" || tmp == "BPM text"))
    ||  !parameterFound)
    {
        if (ofErrLog.is_open())
            ofErrLog << getReadingErrMsg(par);
        return;
    }
    if (valStr == "Default")
        return;

    switch(par.mType)
    {
        case LogicalParameter::Type::Unsigned: 
        case LogicalParameter::Type::Int:      
        case LogicalParameter::Type::Float:    par.setDigit(readDigitParameter(par, valStr)); break;
        case LogicalParameter::Type::Bool:     par.setBool(readBoolParameter(par, valStr));  break;
        case LogicalParameter::Type::StringPath: 
        case LogicalParameter::Type::String:   par.setString(valStr); break;
        case LogicalParameter::Type::Color:    par.setColor(readColorParameter(par, valStr)); break;
        case LogicalParameter::Type::VectorU: 
        case LogicalParameter::Type::VectorI: 
        case LogicalParameter::Type::VectorF:  par.setVector(readVectorParameter(par, valStr)); break;
        default: break;
    }

    if (par.mType == LogicalParameter::Type::StringPath)
    {
        if (par.mType == LogicalParameter::Type::StringPath 
        &&  Settings::BackgroundTexturePath == "GreenscreenBG.png")
            return;
            
        std::ifstream check(par.getString());
        if (check.is_open())
        {
            check.close();
        }
        else
        {
            par.setString(par.getDefValStr());
            if (ofErrLog.is_open())
                ofErrLog << getReadingErrMsg(par);
        }
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
void controlAssets(std::map<LogicalParameter::ID, std::shared_ptr<LogicalParameter>> &parameters)
{
    sf::Texture texture;
    sf::Font font;
    const std::string defAssetName = "Default";

    // Textures
    if (Settings::GfxButtonTexturePath != defAssetName)
        if (!texture.loadFromFile(Settings::GfxButtonTexturePath))
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
    {
        if (Settings::BackgroundTexturePath != "GreenscreenBG.png")
        if (!texture.loadFromFile(Settings::BackgroundTexturePath))
        {
            ofErrLog << getReadingErrMsg(*parameters.find(LogicalParameter::ID::BgTxtr)->second);
            parameters.find(LogicalParameter::ID::BgTxtr)->second->resetToDefaultValue();
        }
    }

    // Fonts
    if (Settings::StatisticsTextFontPath != defAssetName)
        if (!font.loadFromFile(Settings::StatisticsTextFontPath))
        {
            ofErrLog << getReadingErrMsg(*parameters.find(LogicalParameter::ID::StatTextFont)->second);
            parameters.find(LogicalParameter::ID::StatTextFont)->second->resetToDefaultValue();
        }
    if (Settings::ButtonTextFontPath != defAssetName)
        if (!font.loadFromFile(Settings::ButtonTextFontPath))
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


std::queue<LogKey> readKeys(const std::string &keysStr, const std::string &visualKeysStr)
{
    std::queue<LogKey> logKeysQueue;
    unsigned strIdx1 = 0, strIdx2 = 0;

    for (unsigned i = 0; strIdx1 < keysStr.size(); ++i)
    {
        std::string keyStr(keysStr, strIdx1, keysStr.substr(strIdx1).find(','));
        std::string visualKeyStr(visualKeysStr, strIdx2, visualKeysStr.substr(strIdx2).find(','));
        std::string checkStr;

        sf::Keyboard::Key key = strToKey(keyStr);

        checkStr = keyToStr(key, true);
        unsigned maxLength = 20;
        if (visualKeyStr.size() > maxLength || visualKeyStr.size() == 0)
            visualKeyStr = checkStr;

        logKeysQueue.emplace(*new LogKey(keyStr, visualKeyStr, new sf::Keyboard::Key(key), nullptr));
        logKeysQueue.back().realStr = checkStr == keyStr ? keyStr : checkStr;

        // If there is no data to read - break
        if (keysStr.find(',', strIdx1) == std::string::npos)
            break;

        // Move index to the beggining of the next key
        strIdx1 = keysStr.find(',', strIdx1) + 1;
        strIdx2 = visualKeysStr.find(',', strIdx2) + 1;
    }

    return logKeysQueue;
}

std::queue<LogKey> readButtons(const std::string &buttonsStr, const std::string &visualButtonsStr)
{
    std::queue<LogKey> logBtnQueue;
    unsigned strIdx1 = 0, strIdx2 = 0;

    for (unsigned i = 0; strIdx1 < buttonsStr.size(); ++i)
    {
        std::string buttonStr(buttonsStr, strIdx1, buttonsStr.substr(strIdx1).find(','));
        std::string visualButtonStr(visualButtonsStr, strIdx2, visualButtonsStr.substr(strIdx2).find(','));
        std::string checkStr;

        sf::Mouse::Button button = strToBtn(buttonStr);

        checkStr = btnToStr(button);
        unsigned maxLength = 20;
        if (visualButtonStr.size() > maxLength || visualButtonStr.size() == 0)
            visualButtonStr = checkStr;

        logBtnQueue.emplace(*new LogKey(buttonStr, visualButtonStr, nullptr, new sf::Mouse::Button(button)));
        logBtnQueue.back().realStr = checkStr == buttonStr ? buttonStr : checkStr;

        // If there is no data to read - break
        if (buttonsStr.find(',', strIdx1) == std::string::npos)
            break;

        // Move index to the beggining of the next key
        strIdx1 = buttonsStr.find(',', strIdx1) + 1;
        strIdx2 = visualButtonsStr.find(',', strIdx2) + 1;
    }

    return logBtnQueue;
}

void saveConfig(
    const std::map<LogicalParameter::ID, std::shared_ptr<LogicalParameter>> &parameters, 
    const std::map<ParameterLine::ID, std::shared_ptr<ParameterLine>> &parameterLines,
    const std::vector<std::unique_ptr<Button>> *mKeys,
    bool saveKeys)
{
    ofCfg.open(tmpCfgPath);

    ofCfg << defCfgComment;
    ofCfg << "\n";

    if (saveKeys)
    {    
        ofCfg << getKeysStr(*mKeys, "[Keyboard keys]\nKeyboard keys: ", true);
        ofCfg << getKeysStr(*mKeys, "[Visual keys]\nVisual keys: ", false);
        ofCfg << getButtonStr(*mKeys, "[Mouse buttons]\nMouse buttons: ", true);
        ofCfg << getButtonStr(*mKeys, "[Visual buttons]\nVisual buttons: ", false);
    }
    else
    {
        ofCfg << "[Keyboard keys]\nKeyboard keys: Z,X\n";
        ofCfg << "[nVisual keys]\nVisual keys: Z,X\n";
        ofCfg << "[Mouse buttons]\nMouse buttons: No\n";
        ofCfg << "[Visual buttons]\nVisual buttons: No\n";
    }

    bool commentsSection = false;
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

        if (commentsSection)
            ofCfg << "# ";
        ofCfg << mainParP->mParName;
        if (mainParP->mType != LogicalParameter::Type::Empty 
        &&  mainParP->mType != LogicalParameter::Type::Collection)
            ofCfg << ": " << mainParP->getValStr();
        ofCfg << "\n";

        if (parmLinePair->first == ParameterLine::ID::Info1)
            commentsSection = true;
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

std::string getKeysStr(const std::vector<std::unique_ptr<Button>> &mKeys, std::string str, bool readRealStr)
{
    auto it = mKeys.begin();
    for (; it != mKeys.end(); ++it)
    {
        const LogKey *lKey = (*it)->getLogKey();
        if (lKey->keyboardKey == nullptr)
            break;

        str += (readRealStr ? lKey->realStr : lKey->visualStr) + ",";
    }
    if (it != mKeys.begin())
        str.erase(str.size() - 1);
    else
        str += "No";
    str += "\n\n";

    return str;
}

std::string getButtonStr(const std::vector<std::unique_ptr<Button>> &mKeys, std::string str, bool readRealStr)
{
    auto it = mKeys.begin();
    for (; it != mKeys.end(); ++it)
    {
        if ((*it)->getLogKey()->mouseButton)
            break;
    }

    if (it == mKeys.end())
        return str + "No\n\n";

    for (; it != mKeys.end(); ++it)
    {
        const LogKey *lKey = (*it)->getLogKey();
        if (lKey->mouseButton == nullptr)
            break;

        str += (readRealStr ? lKey->realStr : lKey->visualStr) + ",";
    }
    str.erase(str.size() - 1);
    str += "\n\n";

    return str;
}


} // !namespace ConfigHelper

