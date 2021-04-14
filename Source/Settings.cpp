#include "../Headers/Settings.hpp"

#define ERROR_MESSAGE(x) "Reading error - " + x + ". Default value will be set.\n"

std::vector<sf::Keyboard::Key> Settings::Keys({ sf::Keyboard::Key::Z
                                              , sf::Keyboard::Key::X });
std::size_t Settings::KeyAmount = Settings::Keys.size();

std::string Settings::StatisticsFontPath = "Default";
std::string Settings::KeyCountersFontPath = "Default";
sf::Color Settings::StatisticsTextColor(sf::Color::White);
sf::Color Settings::KeyCountersTextColor(sf::Color::Black);
std::size_t Settings::StatisticsTextCharacterSize = 12;
std::size_t Settings::KeyCountersTextCharacterSize = 22;
bool Settings::ShowStatisticsText = true;
bool Settings::ShowKeyCountersText = true;

float Settings::Distance = 7.f;
float Settings::SpaceBetweenButtonsAndStatistics = 10.f;

std::string Settings::ButtonTexturePath = "Default";
sf::Vector2u Settings::ButtonTextureSize(50, 50);
sf::Color Settings::ButtonImageColor(sf::Color(255,255,255,255));
std::size_t Settings::AnimationVelocity = 20;
sf::Color Settings::AnimationColor(sf::Color(255,255,255,255));
sf::Color Settings::AnimationOnClickTransparency(sf::Color(0,0,0,150)); 

std::string Settings::BackgroundTexturePath = "Default";
sf::Color Settings::BackgroundColor(sf::Color(44,28,31,180));

sf::Keyboard::Key Settings::KeyToIncrease(sf::Keyboard::Equal);
sf::Keyboard::Key Settings::KeyToDecrease(sf::Keyboard::Dash);

bool Settings::IsChangeable = false;

unsigned char* Settings::StatisticsDefaultFont = DefaultFont;
unsigned char* Settings::KeyCountersDefaultFont = DefaultFont;
unsigned char* Settings::DefaultButtonTexture = DefaultButtonTextureHeaderArray;
unsigned char* Settings::DefaultBackgroundTexture = DefaultBackgroundTextureHeaderArray;

Settings::Settings()
: configPath("KPS.cfg")
, minimumKeys(1)
, maximumKeys(10)
, mIsChangeableAlert(5.f)
, mAlertColor(sf::Color::Red)
, mButtonToChange(sf::Keyboard::Unknown)
, mButtonToChangeIndex(-1)
{
    mIsChangeableAlert.setFillColor(mAlertColor);

    std::ifstream Config(configPath);
    if (!Config.is_open())
    {
        std::cerr << "Config - Reading error. Default config will be generated.\n";
        createDefaultConfig();
        return;
    }

    setupKey(Keys, findParameter("Keys"), "Keys");

    setupFilePathParameter(StatisticsFontPath, findParameter("Statistics font"), "Statistics font");
    setupFilePathParameter(KeyCountersFontPath, findParameter("Key counters font"), "Key counters font");
    setupColor(StatisticsTextColor, findParameter("Statistics text color"), "Statistics text color");
    setupColor(KeyCountersTextColor, findParameter("Key counters text color"), "Key counters text color");
    setupDigitParameter(StatisticsTextCharacterSize, 0, 100, findParameter("Statistics character size"), "Statistics character size");
    setupDigitParameter(KeyCountersTextCharacterSize, 0, 100, findParameter("Key counters character size"), "Key counters character size");
    setupBoolParameter(ShowStatisticsText, findParameter("Show statistics"), "Show statistics");
    setupBoolParameter(ShowKeyCountersText, findParameter("Show key counters"), "Show key counters");

    setupDigitParameter(Distance, 0, 100, findParameter("Distance"), "Distance");
    setupDigitParameter(SpaceBetweenButtonsAndStatistics, 0, 200, findParameter("Space between buttons and statistics"), "Space between buttons and statistics");

    setupFilePathParameter(ButtonTexturePath, findParameter("Button image"), "Button image");
    setupVector(ButtonTextureSize, 0, 250, findParameter("Button texture size"), "Button texture size");
    setupColor(ButtonImageColor,findParameter("Button image color"), "Button image color");
    setupColor(AnimationColor, findParameter("Animation color"), "Animation color");
    setupDigitParameter(AnimationVelocity, 0, 1000, findParameter("Animation velocity"), "Animation velocity");

    setupFilePathParameter(BackgroundTexturePath, findParameter("Background texture"), "Background texture");
    setupColor(BackgroundColor, findParameter("Background color"), "Background color");

    setupColor(mAlertColor, findParameter("Changeability alert color"), "Changeability alert color");
    mIsChangeableAlert.setFillColor(mAlertColor);

    Config.close();
}

void Settings::handleEvent(sf::Event event)
{
    if (event.type == sf::Event::KeyPressed)
    {
        if (event.key.code == KeyToIncrease
        &&  KeyAmount <= maximumKeys)
        {
            ++KeyAmount;
            Keys.resize(KeyAmount);
            setDefaultKey(Keys.size() - 1);
        } 
        else if (event.key.code == KeyToDecrease
             &&  KeyAmount > minimumKeys)
        {
            --KeyAmount;
            Keys.resize(KeyAmount);
        }
        else if (mButtonToChange == sf::Keyboard::Unknown)
        {
            for (size_t i = 0; i < KeyAmount; ++i)
            {
                if (event.key.code == Keys[i])
                {
                    mButtonToChange = event.key.code;
                    mButtonToChangeIndex = i;
                }
            }
        }
        else
        {
            size_t sameKeyIndex = 0;
            bool isThereSameKeyB = isThereSameKey(event.key.code, sameKeyIndex);

            Keys[mButtonToChangeIndex] = event.key.code;
            if (isThereSameKeyB)
                setDefaultKey(sameKeyIndex);

            mButtonToChange = sf::Keyboard::Unknown;
            mButtonToChangeIndex = -1;
        }
    }
}

void Settings::draw()
{
    if (IsChangeable)
        mWindow->draw(mIsChangeableAlert);
}

std::string Settings::findParameter(const std::string parameterName)
{
    std::ifstream config(configPath);

    std::string line;
    bool found = false;
    size_t i = 0;

    while (!config.eof() && !found)
    {
        getline(config, line);
        for (i = 0; i < parameterName.length(); ++i)
        {
            if (line[i] != parameterName[i])
            {
                i = 0;
                break;
            }
        }

        if (i == parameterName.length())
            found = true;
    }

    config.close();

    // +1 because \0 is also character
    if (line.length() <= i + 1 || line == "")
        return "";

    // Remove parameter name, ':' and space after it
    return line.substr(parameterName.length()+2, 81);
}

template <typename T>
void Settings::setupDigitParameter(  T& parameter
                        ,       int limitMin
                        ,       int limitMax
                        , const std::string information
                        , const std::string parameterName )
                        
{
    if (information == "")
    {
        std::cerr << ERROR_MESSAGE(parameterName);
        return;
    }

    T tmp = 0;
    try 
    {
        tmp = std::stoi(information);

        if (tmp < limitMin || tmp > limitMax)
            throw std::invalid_argument("");
    } 
    catch (std::invalid_argument& e)
    {
        std::cerr << ERROR_MESSAGE(parameterName);
        return;
    }

    parameter = tmp;
}

void Settings::setupColor(sf::Color& color
                    , const std::string information
                    , const std::string parameterName)
{
    if (information == "")
    {
        std::cerr << ERROR_MESSAGE(parameterName);
        return;
    }
        
    int rgba[4] = { color.r,color.g,color.b,color.a };
    size_t stringIndex = 0;
    for (size_t i = 0; i < 4; i++)
    {
        // If there is no transparency code
        if (information[stringIndex-1] != ',' && i == 3)
            break;

        try 
        {
            rgba[i] = std::stoi(information.substr(stringIndex, 81));

            if (rgba[i] < 0 || rgba[i] > 255)
                throw std::invalid_argument("");
        } 
        catch (std::invalid_argument& e)
        {
            std::cerr << ERROR_MESSAGE(parameterName);
            return;
        }

        // Set index on next number
        stringIndex += std::to_string(rgba[i]).length() + 1;
    }
    
    color = sf::Color(rgba[0], rgba[1], rgba[2], rgba[3]);
}

template <typename T>
void Settings::setupVector( T& vector
                    ,       int limitMin
                    ,       int limitMax
                    , const std::string information
                    , const std::string parameterName)
{
    if (information == "")
    {
        std::cerr << ERROR_MESSAGE(parameterName);
        return;
    }

    float tmp[2] = { 0 };
    size_t stringIndex = 0;
    for (size_t i = 0; i < 2; i++)
    {
        try 
        {
            tmp[i] = std::stoi(information.substr(stringIndex, 81));

            if (tmp[i] < limitMin || tmp[i] > limitMax)
                throw std::invalid_argument("");
        } 
        catch (std::invalid_argument& e)
        {
            std::cerr << ERROR_MESSAGE(parameterName);
            return;
        }

        // Set index on next number
        stringIndex += std::to_string(int(tmp[i])).length() + 1;
    }
    
    vector = T(tmp[0], tmp[1]);
}

void Settings::setupFilePathParameter(  std::string& parameter
                                , const std::string information
                                , const std::string parameterName )
{
    if (information == "")
    {
        std::cerr << ERROR_MESSAGE(parameterName);
        return;
    }

    std::ifstream test(information);
    if (!test.is_open())
    {
        std::cerr << ERROR_MESSAGE(parameterName);
        test.close();
        return;
    }
    test.close();

    parameter = information;
}

void Settings::setupKey(std::vector<sf::Keyboard::Key>& keys
                , const std::string information
                , const std::string parameterName)
{
    if (information == "")
    {
        std::cerr << ERROR_MESSAGE(parameterName);
        return;
    }

    size_t stringIndex = 0, nKeys = 0;
    for (size_t i = 0
        ; information.length() > stringIndex && nKeys < maximumKeys
        ; ++i)
    {
        std::string keyStr;
        for (size_t j = stringIndex
            ; information[j] != ',' && information[j] != '\0'
            ; ++j)
        {
            keyStr += information[j];
        }
        
        stringIndex = information.find(',', stringIndex) + 1;

        sf::Keyboard::Key tmp = convertStringToKey(keyStr);

        if (i > 1)
            keys.resize(keys.size() + 1);

        size_t owo;
        if (tmp != sf::Keyboard::Unknown)
            keys[i] = tmp;
        else
        {
            std::cerr << ERROR_MESSAGE(parameterName);
            setDefaultKey(i);
        }

        // find function returns 0 if the value was not found
        if (stringIndex == 0)
            break;
    }
    KeyAmount = keys.size();
}

void Settings::setupBoolParameter(  bool& parameter
                            , const std::string information
                            , const std::string parameterName)
                        
{
    parameter = !(information == "false" || information == "FALSE");

    if (information == "true" || information == "TRUE"
    ||  information == "false" || information == "FALSE")
        return;

    std::cerr << ERROR_MESSAGE(parameterName);
}

bool Settings::isThereSameKey(sf::Keyboard::Key key, size_t& whichOne)
{
    for (size_t i = 0; i < Keys.size(); ++i)
    {
        if (Keys[i] == key)
        {
            whichOne = i;
            return true;
        }
    }
    
    return false;
}

void Settings::setDefaultKey(size_t index)
{
    size_t keyToAssign = 0;
    for (size_t i = 0; i < Settings::KeyAmount; i++)
    {
        if (keyToAssign == Keys[i] && i != index)
        {
            ++keyToAssign;
            i = -1;
        }
    }

    Keys[index] = sf::Keyboard::Key(keyToAssign);
}

void Settings::changeChangeability()
{
    IsChangeable = !IsChangeable;
}

void Settings::setChangeabilityPosition()
{
    mIsChangeableAlert.setPosition(sf::Vector2f(
                                              mWindow->getSize().x
                                            - mIsChangeableAlert.getRadius()*2
                                            , 0.f) );
}

void Settings::setWindowReference(sf::RenderWindow& window)
{
    mWindow = &window;
}

void Settings::saveSettings()
{
    std::ofstream ofConfig("tmpConfig.cfg");
    std::ofstream ofConfigTry(configPath, std::fstream::in);
    if (!ofConfigTry.is_open())
    {
        createDefaultConfig();
        std::cerr << "Config - Reading error. Default config will be generated.\n";
        return;
    }
    ofConfigTry.close();

    writeKeys(ofConfig);

    remove(configPath.c_str());
    rename("tmpConfig.cfg", configPath.c_str());

    ofConfig.close();
}

void Settings::createDefaultConfig()
{
    std::ofstream Config(configPath);
    if (!Config.is_open())
        std::cerr << "Config - Creating error. Config cannot be created.\n";

    Config << DefaultConfigString;

    Config.close();
}

void Settings::writeKeys(std::ofstream& ofConfig)
{
    std::ifstream ifConfig(configPath);

    std::string line, toFind = "Keys";
    bool found = false;
    size_t i = 0, lineN = 0;

    while (!ifConfig.eof())
    {
        getline(ifConfig, line);
        ++lineN;
        for (i = 0; i < toFind.length(); ++i)
        {
            if (line[i] != toFind[i])
            {
                i = 0;
                break;
            }
        }

        if (i == toFind.length())
        {
            found = true;
            break;
        }
    }

    ifConfig.close();

    if (!found)
        return;

    ifConfig.open(configPath);
    for (size_t i = 0; i+1 < lineN; ++i)
    {
        getline(ifConfig, line);
        ofConfig << line << "\n";
    }
    // Skip line to replace
    getline(ifConfig, line);

    ofConfig << "Keys: ";
    for (size_t i = 0; i < Keys.size(); ++i)
    {
        ofConfig << convertKeyToString(Keys[i]) << (i+1 != Keys.size() ? "," : "\n");
    }

    while (!ifConfig.eof())
    {
        getline(ifConfig, line);
        ofConfig << line << (ifConfig.peek() == EOF ? "" : "\n");
    }

    ifConfig.close();
}