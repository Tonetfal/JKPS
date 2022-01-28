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


std::string getReadingErrMsg(const LogicalParameter &par, std::string_view collection)
{
    return std::string("Failed to read '" + std::string(collection.begin(), collection.end()) 
        + " " + par.mParName + "'. Default value will be set.\n");
}

std::string getOutOfBoundsErrMsg(const LogicalParameter &par)
{
    return "Input value is out of bounds - " + par.mParName + " cannot be less than " + std::to_string(par.mLowLimits) + 
        " or more than " + std::to_string(par.mHighLimits) + ". Default value will be set.\n";
}

std::string getOldParName(std::string_view newParName, std::string_view collection);

static std::string cfgPath("JKPS.cfg");
static std::string tmpCfgPath("tmpJKPS.cfg");
static std::string errLogPath("JKPS Error Log.txt");
static std::ifstream ifCfg;
static std::ofstream ofCfg;
static std::ifstream ifErrLog;
static std::ofstream ofErrLog;

namespace ConfigHelper
{

const std::string separationSign(" $SEPARATION$ ");

bool readConfig(
    std::map<LogicalParameter::ID, std::shared_ptr<LogicalParameter>> &parameters, 
    const std::vector<std::string> &collectionNames)
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
    readParameters(parameters, collectionNames);
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

void readParameters(
    std::map<LogicalParameter::ID, std::shared_ptr<LogicalParameter>> &parameters,
    const std::vector<std::string> &collectionNames)
{
    auto it = collectionNames.begin();
    for (auto &pair : parameters)
    {
        if (// the first will  pair.first == LogicalParameter::ID::StatTextDist ||
            pair.first == LogicalParameter::ID::StatTextPosAdvMode ||
            pair.first == LogicalParameter::ID::BtnTextFont ||
            pair.first == LogicalParameter::ID::BtnGfxDist ||
            pair.first == LogicalParameter::ID::BtnTextSepPosAdvMode ||
            pair.first == LogicalParameter::ID::BtnGfxBtnPosAdvMode ||
            pair.first == LogicalParameter::ID::AnimGfxLight ||
            pair.first == LogicalParameter::ID::BgTxtr ||
            pair.first == LogicalParameter::ID::KPSWndwEn ||
            pair.first == LogicalParameter::ID::KeyPressVisToggle ||
            pair.first == LogicalParameter::ID::OtherSaveStats ||
            pair.first == LogicalParameter::ID::SaveStatMaxKPS)
        {
            ++it;
        }

        readParameter(*pair.second, *it);
    }
}

std::queue<LogKey> getLogKeys()
{
    static const std::string parName1 = "Real keys";
    static const std::string parName2 = "Visual keys";
    bool parameterFound1 = false, parameterEmpty1 = false, nothing;

    std::string keysStr = scanParameter(parName1, parameterFound1, parameterEmpty1, "[" + parName1 + "]");
    std::string visualKeysStr = scanParameter(parName2, nothing, nothing, "[" + parName2 + "]");
         
    if (!parameterFound1 || parameterEmpty1 || keysStr == "No" || keysStr == "no" || keysStr == "NO")
        return { };

    std::queue<LogKey> keyboardQueue = readKeys(keysStr, visualKeysStr);
    
    return keyboardQueue;
}

std::queue<LogKey> oldGetLogKeys()
{
    static const std::string parName1 = "Keyboard keys";
    static const std::string parName2 = "Visual keys";
    // This needs to support old versions, but it has to be removed later
    static const std::string oldParName1 = "Keys";
    bool parameterFound = false, parameterEmpty = false, nothing;
    std::string keysStr = scanParameter(parName1, parameterFound, parameterEmpty, "[" + parName1 + "]");
    std::string visualKeysStr = scanParameter(parName2, nothing, nothing, "[" + parName2 + "]");
    if (!parameterFound)
        keysStr = scanParameter(oldParName1, parameterFound, parameterEmpty, "[" + oldParName1 + "]");
         
    if (!parameterFound || parameterEmpty || keysStr == "No" || keysStr == "no" || keysStr == "NO")
        return { };

    return oldReadKeys(keysStr, visualKeysStr);
}

std::queue<LogKey> oldGetLogButtons()
{
    static const std::string parName1 = "Mouse buttons";
    static const std::string parName2 = "Visual buttons";
    bool parameterFound = false, parameterEmpty = false, nothing;
    std::string buttonsStr = scanParameter(parName1, parameterFound, parameterEmpty, "[" + parName1 + "]");
    std::string visualButtonsStr = scanParameter(parName2, nothing, nothing, "[" + parName2 + "]");

    if (!parameterFound || parameterEmpty || buttonsStr == "No" || buttonsStr == "no" || buttonsStr == "NO")
        return { };

    return oldReadButtons(buttonsStr, visualButtonsStr);
}

// Finds the parameter and returns everything after its name and ": ", 
// can also return empty string, if there is no such parameter or if there is no value
std::string scanParameter(const std::string &parName, bool &parameterFound, bool &parameterEmpty, std::string_view collection)
{
    std::ifstream cfg(cfgPath);
    assert(cfg.is_open());

    std::string line;
    std::string_view curCollection;
    unsigned i = 0;

    while (!cfg.eof() && i != parName.length())
    {
        getline(cfg, line);

        // Make search only in the required collection
        if (collection == line)
            curCollection = collection;
        // If current line is a collection which is not the required one, don't let to search into it
        else if (line[0] == '[')
            curCollection = "";

        // If current collection is not the required one, procede on searching
        if (curCollection.empty())
            continue;

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

    // If the loop finished with an invalid collection the parameter was not found
    if (curCollection.empty())
        return "";

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
    return std::string(line, parName.length() + 2);
}

void readParameter(LogicalParameter &par, std::string_view collection)
{
    bool parameterFound = false;
    bool parameterEmpty = false;
    auto valStr = scanParameter(par.mParName, parameterFound, parameterEmpty, collection);

    auto parName = par.mParName;
    // If no value was read try to check if current parameter name has an alternative in older versions
    while (valStr == "" && parName != "")
    {
        if (parName = getOldParName(parName, collection); parName != "")
        {
            valStr = scanParameter(parName, parameterFound, parameterEmpty, collection);
            // if (valStr == "")
            //     std::cout << parName << " was not found even with old version support\n";
        }
    }

    parName = par.mParName;
    // only these 4 can be empty
    if ((parameterEmpty && 
            !( parName == "KPS text" 
            || parName == "KPS text when it is 0" 
            || parName == "Total text" 
            || parName == "BPM text"))
        ||  !parameterFound)
    {
        if (ofErrLog.is_open())
            ofErrLog << getReadingErrMsg(par, collection);
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
                ofErrLog << getReadingErrMsg(par, collection);
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
        strIdx += valStr.substr(strIdx).find(',') + 1lu;
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
        float tmpF = static_cast<float>(tmp);

        if ((tmpF < par.mLowLimits || tmpF > par.mHighLimits)
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
            strIdx += valStr.substr(strIdx).find(',') + 1lu;
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
        ofErrLog << getReadingErrMsg(par, "[Bool]");

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
            ofErrLog << getReadingErrMsg(*parameters.find(LogicalParameter::ID::BtnGfxTxtr)->second, "[Button graphics]");
            parameters.find(LogicalParameter::ID::BtnGfxTxtr)->second->resetToDefaultValue();
        }
    if (Settings::AnimationTexturePath != defAssetName)
        if (!texture.loadFromFile(Settings::AnimationTexturePath))
        {
            ofErrLog << getReadingErrMsg(*parameters.find(LogicalParameter::ID::AnimGfxTxtr)->second, "[Animation graphics]");
            parameters.find(LogicalParameter::ID::AnimGfxTxtr)->second->resetToDefaultValue();
        }
    if (Settings::BackgroundTexturePath != defAssetName)
    {
        if (Settings::BackgroundTexturePath != "GreenscreenBG.png")
        if (!texture.loadFromFile(Settings::BackgroundTexturePath))
        {
            ofErrLog << getReadingErrMsg(*parameters.find(LogicalParameter::ID::BgTxtr)->second, "[Main window]");
            parameters.find(LogicalParameter::ID::BgTxtr)->second->resetToDefaultValue();
        }
    }

    // Fonts
    if (Settings::StatisticsTextFontPath != defAssetName)
        if (!font.loadFromFile(Settings::StatisticsTextFontPath))
        {
            ofErrLog << getReadingErrMsg(*parameters.find(LogicalParameter::ID::StatTextFont)->second, "[Statistics text]");
            parameters.find(LogicalParameter::ID::StatTextFont)->second->resetToDefaultValue();
        }
    if (Settings::ButtonTextFontPath != defAssetName)
        if (!font.loadFromFile(Settings::ButtonTextFontPath))
        {
            ofErrLog << getReadingErrMsg(*parameters.find(LogicalParameter::ID::BtnTextFont)->second, "[Button text]");
            parameters.find(LogicalParameter::ID::BtnTextFont)->second->resetToDefaultValue();
        }
    if (Settings::KPSWindowTextFontPath != defAssetName)
        if (!font.loadFromFile(Settings::KPSWindowTextFontPath))
        {
            ofErrLog << getReadingErrMsg(*parameters.find(LogicalParameter::ID::KPSWndwTxtFont)->second, "[KPS window]");
            parameters.find(LogicalParameter::ID::KPSWndwTxtFont)->second->resetToDefaultValue();
        }
    if (Settings::KPSWindowNumberFontPath != defAssetName)
        if (!font.loadFromFile(Settings::KPSWindowNumberFontPath))
        {
            ofErrLog << getReadingErrMsg(*parameters.find(LogicalParameter::ID::KPSWndwNumFont)->second, "[KPS window]");
            parameters.find(LogicalParameter::ID::KPSWndwNumFont)->second->resetToDefaultValue();
        }
}

std::queue<LogKey> readKeys(const std::string &keysStr, const std::string &visualKeysStr)
{
    std::queue<LogKey> logKeysQueue;
    auto strIdx1 = 0lu, strIdx2 = 0lu;
    const auto len = ConfigHelper::separationSign.length();

    for (unsigned i = 0; strIdx1 < keysStr.size(); ++i)
    {
        std::string keyStr(keysStr, strIdx1, keysStr.substr(strIdx1).find(ConfigHelper::separationSign));
        std::string visualKeyStr(visualKeysStr, strIdx2, visualKeysStr.substr(strIdx2).find(ConfigHelper::separationSign));   
        std::string checkStr;
        sf::Keyboard::Key key;
        sf::Mouse::Button button;

        const bool isKeyB = isKey(keyStr);
        const bool isButtonB = isButton(keyStr);
        if (!isKeyB && !isButtonB)
        {
            // If there is no data to read - break
            if (keysStr.find(ConfigHelper::separationSign, strIdx1) == std::string::npos)
                break;

            // Move index to the beggining of the next key
            strIdx1 = keysStr.find(ConfigHelper::separationSign, strIdx1) + len;
            strIdx2 = visualKeysStr.find(ConfigHelper::separationSign, strIdx2) + len;
        } 
        else if (isKeyB)
        {
            key = strToKey(keyStr);
            checkStr = keyToStr(key, true);
        }
        else
        {
            button = strToBtn(keyStr);
            checkStr = btnToStr(button);
        }
            
        unsigned maxLength = 20;
        if (visualKeyStr.size() > maxLength || visualKeyStr.size() == 0)
            visualKeyStr = checkStr;

        // TODO remove *new LogKey, use smart ptrs instead
        if (isKeyB)
            logKeysQueue.emplace(*new LogKey(keyStr, visualKeyStr, new sf::Keyboard::Key(key), nullptr));
        else
            logKeysQueue.emplace(*new LogKey(keyStr, visualKeyStr, nullptr, new sf::Mouse::Button(button)));

        logKeysQueue.back().realStr = checkStr == keyStr ? keyStr : checkStr;

        // If there is no data to read - break
        if (keysStr.find(ConfigHelper::separationSign, strIdx1) == std::string::npos)
            break;

        // Move index to the beggining of the next key
        strIdx1 = keysStr.find(ConfigHelper::separationSign, strIdx1) + len;
        strIdx2 = visualKeysStr.find(ConfigHelper::separationSign, strIdx2) + len;
    }

    return logKeysQueue;
}

std::queue<LogKey> oldReadKeys(const std::string &keysStr, const std::string &visualKeysStr)
{
    std::queue<LogKey> logKeysQueue;
    unsigned strIdx1 = 0, strIdx2 = 0;

    for (unsigned i = 0; strIdx1 < keysStr.size(); ++i)
    {
        std::string keyStr(keysStr, strIdx1, keysStr.substr(strIdx1).find(','));
        std::string visualKeyStr(visualKeysStr, strIdx2, visualKeysStr.substr(strIdx2).find(','));
        std::string checkStr;

        sf::Keyboard::Key key = strToKey(keyStr);
        if (key == sf::Keyboard::Unknown)
        {
            if (keysStr.find(',', strIdx1) == std::string::npos)
                break;
            strIdx1 = keysStr.find(',', strIdx1) + 1;
            strIdx2 = visualKeysStr.find(',', strIdx2) + 1;
        }

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

std::queue<LogKey> oldReadButtons(const std::string &buttonsStr, const std::string &visualButtonsStr)
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
        ofCfg << "[Real keys]\nReal keys: " << getKeysStr(*mKeys, true);
        ofCfg << "[Visual keys]\nVisual keys: " << getKeysStr(*mKeys, false);
    }
    else
    {
        ofCfg << "[Real keys]\nReal keys: Z,X\n";
        ofCfg << "[Visual keys]\nVisual keys: Z,X\n";
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

std::string getKeysStr(const std::vector<std::unique_ptr<Button>> &mKeys, bool readRealStr)
{
    std::string str;
    auto it = mKeys.begin();

    for (; it != mKeys.end(); ++it)
    {
        const LogKey *lKey = (*it)->getLogKey();
        assert(lKey && (lKey->keyboardKey || lKey->mouseButton));

        str += (readRealStr ? lKey->realStr : lKey->visualStr) + ConfigHelper::separationSign;
    }
    if (it != mKeys.begin())
        str.erase(str.size() - ConfigHelper::separationSign.length());
    else
        str += "No";
    str += "\n\n";

    return str;
}

} // !namespace ConfigHelper


std::string getOldParName(std::string_view newParName, std::string_view collection)
{
         if (newParName == "Distance between" && collection == "[Statistics text]")
        return 			   "Statistics text distance";
    else if (newParName == "Position offset" && collection == "[Statistics text]")
        return 			   "Statistics text position";
    else if (newParName == "Number position offset" && collection == "[Statistics text]")
        return 			   "Statistics value text position";
    else if (newParName == "Font filepath" && collection == "[Statistics text]")
        return 			   "Statistics text font";
    else if (newParName == "Text color" && collection == "[Statistics text]")
        return 			   "Statistics text color";
    else if (newParName == "Character size" && collection == "[Statistics text]")
        return 			   "Statistics text character size";
    else if (newParName == "Outline thickness" && collection == "[Statistics text]")
        return 			   "Statistics text outline thickness";
    else if (newParName == "Outline color" && collection == "[Statistics text]")
        return 			   "Statistics text outline color";
    else if (newParName == "Bold" && collection == "[Statistics text]")
        return 			   "Statistics text bold";
    else if (newParName == "Italic" && collection == "[Statistics text]")
        return 			   "Statistics text italic";
    else if (newParName == "Enabled" && collection == "[Statistics text]")
        return 			   "Show statistics text";

    else if (newParName == "Enable advanced mode for text positions" && collection == "[Statistics text advanced settings]")
        return             "Enable advanced mode for stats text positions";
    else if (newParName == "KPS text position offset" && collection == "[Statistics text advanced settings]")
        return 			   "Statistics KPS text position";
    else if (newParName == "Total text position offset" && collection == "[Statistics text advanced settings]")
        return 			   "Statistics total text position";
    else if (newParName == "BPM text position offset" && collection == "[Statistics text advanced settings]")
        return 			   "Statistics BPM text position";
    else if (newParName == "Enable advanced mode for number text positions" && collection == "[Statistics text advanced settings]")
        return             "Enable advanced mode for stats value text positions";

    else if (newParName == "KPS number text position offset" && collection == "[Statistics text advanced settings]")
        return             "Statistics KPS value text position";
    else if (newParName == "Total number text position offset" && collection == "[Statistics text advanced settings]")
        return             "Statistics total value text position";
    else if (newParName == "BPM number text position offset" && collection == "[Statistics text advanced settings]")
        return             "Statistics BPM value text position";

    else if (newParName == "Enable advanced mode for text colors" && collection == "[Statistics text advanced settings]")
        return 			   "Enable advanced mode for stats text colors";
    else if (newParName == "KPS text color" && collection == "[Statistics text advanced settings]")
        return 			   "Statistics KPS text color";
    else if (newParName == "Total text color" && collection == "[Statistics text advanced settings]")
        return 			   "Statistics total text color";
    else if (newParName == "BPM text color" && collection == "[Statistics text advanced settings]")
        return 			   "Statistics BPM text color";
    else if (newParName == "KPS text character size" && collection == "[Statistics text advanced settings]")
        return 			   "Statistics KPS text character size";
    else if (newParName == "Total text character size" && collection == "[Statistics text advanced settings]")
        return 			   "Statistics total text character size";
    else if (newParName == "BPM text character size" && collection == "[Statistics text advanced settings]")
        return 			   "Statistics BPM text character size";
    else if (newParName == "KPS bold" && collection == "[Statistics text advanced settings]")
        return 			   "Statistics KPS bold";
    else if (newParName == "Total bold" && collection == "[Statistics text advanced settings]")
        return 			   "Statistics total bold";
    else if (newParName == "BPM bold" && collection == "[Statistics text advanced settings]")
        return 			   "Statistics BPM bold";
    else if (newParName == "KPS italic" && collection == "[Statistics text advanced settings]")
        return 			   "Statistics KPS italic";
    else if (newParName == "Total italic" && collection == "[Statistics text advanced settings]")
        return 			   "Statistics total italic";
    else if (newParName == "BPM italic" && collection == "[Statistics text advanced settings]")
        return 			   "Statistics BPM italic";

    else if (newParName == "Font filepath" && collection == "[Buttons text]")
        return 			   "Buttons text font";
    else if (newParName == "Text color" && collection == "[Buttons text]")
        return 			   "Buttons text color";
    else if (newParName == "Character size" && collection == "[Buttons text]")
        return 			   "Buttons text character size";
    else if (newParName == "Outline thickness" && collection == "[Buttons text]")
        return 			   "Buttons text outline thickness";
    else if (newParName == "Outline color" && collection == "[Buttons text]")
        return 			   "Buttons text outline color";
    else if (newParName == "Position offset" && collection == "[Buttons text]")
        return 			   "Buttons text position";
    else if (newParName == "Text bounds" && collection == "[Buttons text]")
        return 			   "Buttons text bounds";
    else if (newParName == "Bold" && collection == "[Buttons text]")
        return 			   "Buttons text bold";
    else if (newParName == "Italic" && collection == "[Buttons text]")
        return 			   "Buttons text italic";
    else if (newParName == "Enabled" && collection == "[Buttons text]")
        return 			   "Show visual keys";

    // else if (newParName == "Enable advanced mode for separate button text positions" && collection == "[Button text advanced settings]")
    //     return             "Enable advanced mode for separate button text positions"
    else if (newParName == "Visual keys text position offset" && collection == "[Buttons text advanced settings]")
        return 			   "Buttons visual keys text position";
    else if (newParName == "Key counters text position offset" && collection == "[Buttons text advanced settings]")
        return 			   "Buttons key counters text position";
    else if (newParName == "KPS text position offset" && collection == "[Buttons text advanced settings]")
        return 			   "Buttons KPS text position";
    else if (newParName == "BPM text position offset" && collection == "[Buttons text advanced settings]")
        return 			   "Buttons BPM text position";
    else if (newParName == "Enable advanced mode for button text position" && collection == "[Buttons text advanced settings]")
        return 			   "Enable advanced mode for button text positions";
    else if (newParName == "Button 1 text position offset" && collection == "[Buttons text advanced settings]")
        return 			   "Button text 1 position";
    else if (newParName == "Button 2 text position offset" && collection == "[Buttons text advanced settings]")
        return 			   "Button text 2 position";
    else if (newParName == "Button 3 text position offset" && collection == "[Buttons text advanced settings]")
        return 			   "Button text 3 position";
    else if (newParName == "Button 4 text position offset" && collection == "[Buttons text advanced settings]")
        return 			   "Button text 4 position";
    else if (newParName == "Button 5 text position offset" && collection == "[Buttons text advanced settings]")
        return 			   "Button text 5 position";
    else if (newParName == "Button 6 text position offset" && collection == "[Buttons text advanced settings]")
        return 			   "Button text 6 position";
    else if (newParName == "Button 7 text position offset" && collection == "[Buttons text advanced settings]")
        return 			   "Button text 7 position";
    else if (newParName == "Button 8 text position offset" && collection == "[Buttons text advanced settings]")
        return 			   "Button text 8 position";
    else if (newParName == "Button 9 text position offset" && collection == "[Buttons text advanced settings]")
        return 			   "Button text 9 position";
    else if (newParName == "Button 10 text position offset" && collection == "[Buttons text advanced settings]")
        return 			   "Button text 10 position";
    else if (newParName == "Button 11 text position offset" && collection == "[Buttons text advanced settings]")
        return 			   "Button text 11 position";
    else if (newParName == "Button 12 text position offset" && collection == "[Buttons text advanced settings]")
        return 			   "Button text 12 position";
    else if (newParName == "Button 13 text position offset" && collection == "[Buttons text advanced settings]")
        return 			   "Button text 13 position";
    else if (newParName == "Button 14 text position offset" && collection == "[Buttons text advanced settings]")
        return 			   "Button text 14 position";
    else if (newParName == "Button 15 text position offset" && collection == "[Buttons text advanced settings]")
        return 			   "Button text 15 position";

    else if (newParName == "Distance" && collection == "[Button graphics]")
        return 			   "Button distance";
    else if (newParName == "Texture filepath" && collection == "[Button graphics]")
        return 			   "Button texture";
    else if (newParName == "Texture size" && collection == "[Button graphics]")
        return 			   "Button texture size";
    else if (newParName == "Texture color" && collection == "[Button graphics]")
        return 			   "Button texture color";

    // else if (newParName == "Enable advanced mode for button positions" && collection == "[Button graphics advanced settings]")
    //     return 			   "Enable advanced mode for button positions";
    else if (newParName == "Button 1 position offset" && collection == "[Button graphics advanced settings]")
        return 			   "Button 1 position";
    else if (newParName == "Button 2 position offset" && collection == "[Button graphics advanced settings]")
        return 			   "Button 2 position";
    else if (newParName == "Button 3 position offset" && collection == "[Button graphics advanced settings]")
        return 			   "Button 3 position";
    else if (newParName == "Button 4 position offset" && collection == "[Button graphics advanced settings]")
        return 			   "Button 4 position";
    else if (newParName == "Button 5 position offset" && collection == "[Button graphics advanced settings]")
        return 			   "Button 5 position";
    else if (newParName == "Button 6 position offset" && collection == "[Button graphics advanced settings]")
        return 			   "Button 6 position";
    else if (newParName == "Button 7 position offset" && collection == "[Button graphics advanced settings]")
        return 			   "Button 7 position";
    else if (newParName == "Button 8 position offset" && collection == "[Button graphics advanced settings]")
        return 			   "Button 8 position";
    else if (newParName == "Button 9 position offset" && collection == "[Button graphics advanced settings]")
        return 			   "Button 9 position";
    else if (newParName == "Button 10 position offset" && collection == "[Button graphics advanced settings]")
        return 			   "Button 10 position";
    else if (newParName == "Button 11 position offset" && collection == "[Button graphics advanced settings]")
        return 			   "Button 11 position";
    else if (newParName == "Button 12 position offset" && collection == "[Button graphics advanced settings]")
        return 			   "Button 12 position";
    else if (newParName == "Button 13 position offset" && collection == "[Button graphics advanced settings]")
        return 			   "Button 13 position";
    else if (newParName == "Button 14 position offset" && collection == "[Button graphics advanced settings]")
        return 			   "Button 14 position";
    else if (newParName == "Button 15 position offset" && collection == "[Button graphics advanced settings]")
        return 			   "Button 15 position";

    else if (newParName == "Light animation enabled" && collection == "[Animation graphics]")
        return 			   "Light animation";
    else if (newParName == "Press animation enabled" && collection == "[Animation graphics]")
        return 			   "Press animation";
    else if (newParName == "Texture filepath" && collection == "[Animation graphics]")
        return 			   "Animation texture";
    else if (newParName == "Scale on click (%)" && collection == "[Animation graphics]")
        return 			   "Animation scale on click (%)";
    else if (newParName == "Color" && collection == "[Animation graphics]")
        return 			   "Animation color";
    else if (newParName == "Offset" && collection == "[Animation graphics]")
        return 			   "Animation offset";

    else if (newParName == "Background texture filepath" && collection == "[Main window]")
        return 			   "Background texture";
    else if (newParName == "Title bar" && collection == "[Main window]")
        return 			   "Window title bar";
    else if (newParName == "Bonus size top" && collection == "[Main window]")
        return 			   "Window bonus size top";
    else if (newParName == "Bonus size bottom" && collection == "[Main window]")
        return 			   "Window bonus size bottom";
    else if (newParName == "Bonus size left" && collection == "[Main window]")
        return 			   "Window bonus size left";
    else if (newParName == "Bonus size right" && collection == "[Main window]")
        return 			   "Window bonus size right";

    else if (newParName == "Background color" && collection == "[Extra KPS window]")
        return 			   "KPS Background color";
    else if (newParName == "Text color" && collection == "[Extra KPS window]")
        return 			   "KPS text color";
    else if (newParName == "Number color" && collection == "[Extra KPS window]")
        return 			   "KPS number color";
    else if (newParName == "Text font filepath" && collection == "[Extra KPS window]")
        return 			   "KPS window text font";
    else if (newParName == "Number font filepaht" && collection == "[Extra KPS window]")
        return 			   "KPS window number font";
    else if (newParName == "Top padding" && collection == "[Extra KPS window]")
        return 			   "KPS top padding";
    else if (newParName == "Distance between text" && collection == "[Extra KPS window]")
        return 			   "KPS extra window distance between text";
    else if (newParName == "Spawn position offset" && collection == "[Key press visualization]")
        return 			   "Origin";
    else
        return "";
}
