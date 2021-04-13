#include "../Headers/Settings.hpp"

std::vector<sf::Keyboard::Key> Settings::Keys({   sf::Keyboard::Key::Z
                                                , sf::Keyboard::Key::X });
std::size_t Settings::KeyAmount = Settings::Keys.size();

sf::Color Settings::StatisticsTextColor(sf::Color::White);
sf::Color Settings::KeyCountersTextColor(sf::Color::Black);
std::size_t Settings::StatisticsTextCharacterSize = 12;
std::size_t Settings::KeyCountersTextCharacterSize = 20;

float Settings::Distance = 5.f;
float Settings::SpaceBetweenButtonsAndStatistics = 5.f;

sf::Vector2u Settings::ButtonTextureSize(50, 50);
sf::Color Settings::ButtonImageColor(sf::Color(255,255,255,255));
std::size_t Settings::AnimationVelocity = 15;
sf::Color Settings::AnimationColor(sf::Color::White);
sf::Color Settings::AnimationOnClickTransparency(sf::Color(0,0,0,150));

sf::Keyboard::Key Settings::KeyToIncrease(sf::Keyboard::Equal);
sf::Keyboard::Key Settings::KeyToDecrease(sf::Keyboard::Dash);

Settings::Settings(const std::string& FilePath)
: configPath(FilePath)
, minimumKeys(1)
, maximumKeys(10)
, mIsChangeable(false)
, mIsChangeableAlert(5.f)
, mAlertColor(sf::Color::Red)
, mButtonToChange(sf::Keyboard::Unknown)
, mButtonToChangeIndex(-1)
{
    mIsChangeableAlert.setFillColor(mAlertColor);

    setupKey(Keys, findParameter("Keys"), "Keys");

    // font
    setColor(StatisticsTextColor, findParameter("Statistics text color"), "Statistics text color");
    setColor(KeyCountersTextColor, findParameter("Key counters text color"), "Key counters text color");
    setupParameter(StatisticsTextCharacterSize, 0, 100, findParameter("Statistics character size"), "Statistics character size");
    setupParameter(KeyCountersTextCharacterSize, 0, 100, findParameter("Key counters character size"), "Key counters character size");

    setupParameter(Distance, 0, 100, findParameter("Distance"), "Distance");
    setupParameter(SpaceBetweenButtonsAndStatistics, 0, 200, findParameter("Space between buttons and statistics"), "Space between buttons and statistics");

    // button image
    setupVector(ButtonTextureSize, 0, 250, findParameter("Button texture size"), "Button texture size");
    setColor(ButtonImageColor,findParameter("Button image color"), "Button image color");
    setColor(AnimationColor, findParameter("Animation color"), "Animation color");
    setupParameter(AnimationVelocity, 0, 1000, findParameter("Animation velocity"), "Animation velocity");
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

    // Remove parameter name, ':' and space after it
    return line.substr(parameterName.length()+2, 81);
}

template <typename T>
void Settings::setupParameter(  T& parameter
                        ,       int limitMin
                        ,       int limitMax
                        , const std::string information
                        , const std::string parameterName )
                        
{
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

void Settings::setColor(sf::Color& color
                    , const std::string information
                    , const std::string parameterName)
{
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

void Settings::setupKey(std::vector<sf::Keyboard::Key>& keys
                , const std::string information
                , const std::string parameterName)
{
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

// void Settings::setButtonImage(std::string& Information)
// {

// }

// void Settings::setTextFont(std::string& Information)
// {

// }

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