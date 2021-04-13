#include "../Headers/Settings.hpp"

std::vector<sf::Keyboard::Key> Settings::Keys({ sf::Keyboard::Key::Z
                                              , sf::Keyboard::Key::X });
std::size_t Settings::KeyAmount = Settings::Keys.size();

std::string Settings::FontPath = "Ubuntu-Regular.ttf";
sf::Color Settings::StatisticsTextColor(sf::Color::White);
sf::Color Settings::KeyCountersTextColor(sf::Color::Black);
std::size_t Settings::StatisticsTextCharacterSize = 12;
std::size_t Settings::KeyCountersTextCharacterSize = 20;

float Settings::Distance = 5.f;
float Settings::SpaceBetweenButtonsAndStatistics = 5.f;

std::string Settings::ButtonTexturePath = "ButtonTexture.png";
sf::Vector2u Settings::ButtonTextureSize(50, 50);
sf::Color Settings::ButtonImageColor(sf::Color(255,255,255,255));
std::size_t Settings::AnimationVelocity = 15;
sf::Color Settings::AnimationColor(sf::Color::White);
sf::Color Settings::AnimationOnClickTransparency(sf::Color(0,0,0,150));

std::string Settings::BackgroundTexturePath = "BackgroundTexture.png";
sf::Color Settings::BackgroundColor(sf::Color(0,0,0,0));

sf::Keyboard::Key Settings::KeyToIncrease(sf::Keyboard::Equal);
sf::Keyboard::Key Settings::KeyToDecrease(sf::Keyboard::Dash);

Settings::Settings()
: configPath("KPS.cfg")
, minimumKeys(1)
, maximumKeys(10)
, mIsChangeable(false)
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

    setupFilePathParameter(FontPath, findParameter("Text font"), "Text font");
    setupColor(StatisticsTextColor, findParameter("Statistics text color"), "Statistics text color");
    setupColor(KeyCountersTextColor, findParameter("Key counters text color"), "Key counters text color");
    setupDigitParameter(StatisticsTextCharacterSize, 0, 100, findParameter("Statistics character size"), "Statistics character size");
    setupDigitParameter(KeyCountersTextCharacterSize, 0, 100, findParameter("Key counters character size"), "Key counters character size");

    setupDigitParameter(Distance, 0, 100, findParameter("Distance"), "Distance");
    setupDigitParameter(SpaceBetweenButtonsAndStatistics, 0, 200, findParameter("Space between buttons and statistics"), "Space between buttons and statistics");

    setupFilePathParameter(ButtonTexturePath, findParameter("Button image"), "Button image");
    setupVector(ButtonTextureSize, 0, 250, findParameter("Button texture size"), "Button texture size");
    setupColor(ButtonImageColor,findParameter("Button image color"), "Button image color");
    setupColor(AnimationColor, findParameter("Animation color"), "Animation color");
    setupDigitParameter(AnimationVelocity, 0, 1000, findParameter("Animation velocity"), "Animation velocity");

    setupFilePathParameter(BackgroundTexturePath, findParameter("Background texture"), "Background texture");
    setupColor(BackgroundColor, findParameter("Background color"), "Background color");
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
    if (mIsChangeable)
        mWindow->draw(mIsChangeableAlert);
}

std::string Settings::findParameter(const std::string parameterName)
{
    std::ifstream config(configPath);

    std::string line;
    bool found = false;
    int i = 0;

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
        return;
    T tmp = 0;
    try 
    {
        tmp = std::stoi(information);

        if (tmp < limitMin || tmp > limitMax)
            throw std::invalid_argument("");
    } 
    catch (std::invalid_argument& e)
    {
        std::cerr << "Reading error - " + parameterName + ". Default value will be set.\n";
        return;
    }

    parameter = tmp;
}

void Settings::setupColor(sf::Color& color
                    , const std::string information
                    , const std::string parameterName)
{
    if (information == "")
        return;
        
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
            std::cerr << "Reading error - " + parameterName + ". Default value will be set.\n";
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
        return;

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
            std::cerr << "Reading error - " + parameterName + ". Default value will be set.\n";
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
        return;

    if (access(information.c_str(), F_OK ))
    {
        std::cerr << "Reading error - " + parameterName + ". Default value will be set.\n";
        return;
    }

    parameter = information;
}

void Settings::setupKey(std::vector<sf::Keyboard::Key>& keys
                , const std::string information
                , const std::string parameterName)
{
    if (information == "")
        return;

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
        if (tmp != sf::Keyboard::Unknown 
        && !isThereSameKey(tmp, owo))
            keys[i] = tmp;
        else
            setDefaultKey(i);

        // find function returns 0 if the value was not found
        if (stringIndex == 0)
            break;
    }
    KeyAmount = keys.size();
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
    mIsChangeable = !mIsChangeable;
}

bool Settings::isChangeable()
{
    return mIsChangeable;
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

void Settings::createDefaultConfig()
{
    std::ofstream Config(configPath);
    if (!Config.is_open())
        std::cerr << "Config - Creating error. Config cannot be created.\n";
    
    std::string KEKW = 
        "# This config file contains all KPS settings\n"
        "#\n"
        "# If executable file of KPS will not find any config in its directory\n"
        "# it will generate one by itself\n"
        "#\n"
        "# If anything will not be found it will be replaced by default settings\n"
        "#\n"
        "# Available keys to set you can get there\n"
        "# https://www.sfml-dev.org/documentation/2.5.1/classsf_1_1Keyboard.php\n"
        "# UpArrow, DownArrow etc can be written like so\n"
        "\n"
        "\n"
        "[Keys]\n"
        "Keys: Z,X\n"
        "\n"
        "[Text]\n"
        "Text font: Ubuntu-Regular.ttf\n"
        "Statistics text color: 255,255,255\n"
        "Key counters text color: 0,0,0\n"
        "Statistics character size: 12\n"
        "Key counters character size: 22\n"
        "\n"
        "[Spacing]\n"
        "# Distance from the borders and between the keys\n"
        "Distance: 5\n"
        "Space between buttons and statistics: 10\n"
        "\n"
        "[Buttons Graphic]\n"
        "Button image: ButtonTexture.png\n"
        "Button texture size: 50,50\n"
        "Button image color: 255,255,255,255\n"
        "Animation velocity: 20\n"
        "Animation color: 255,255,255,255\n"
        "\n"
        "[Background]\n"
        "\n"
        "Background texture:\n"
        "Background color: 0,0,0,0\n";

    Config << KEKW;

    Config.close();
}