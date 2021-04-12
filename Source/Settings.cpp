#include "../Headers/Settings.hpp"

std::size_t Settings::KeyAmount = 2;
std::vector<sf::Keyboard::Key> Settings::Keys({   sf::Keyboard::Key::V
                                                , sf::Keyboard::Key::B });

sf::Color Settings::StatisticsTextColor(sf::Color::White);
sf::Color Settings::KeyCountersTextColor(sf::Color::Black);
std::size_t Settings::StatisticsTextCharacterSize = 12;
std::size_t Settings::KeyCountersTextCharacterSize = 20;

float Settings::Distance = 5.f;
float Settings::SpaceBetweenButtonsAndStatistics = 5.f;

sf::Vector2u Settings::DefaultButtonTextureSize(50, 50);
sf::Color Settings::ButtonImageTransparency(sf::Color(0,0,0,0));
std::size_t Settings::AnimationVelocity = 15;
sf::Color Settings::AnimationColor(sf::Color::White);
sf::Color Settings::AnimationOnClickTransparency(sf::Color(0,0,0,150));

sf::Keyboard::Key Settings::KeyToIncrease(sf::Keyboard::Equal);
sf::Keyboard::Key Settings::KeyToDecrease(sf::Keyboard::Dash);

Settings::Settings(const std::string& FilePath)
: minimumKeys(1)
, maximumKeys(10)
, mIsChangeable(false)
, mIsChangeableAlert(5.f)
, mAlertColor(sf::Color::Red)
, mButtonToChange(sf::Keyboard::Unknown)
, mButtonToChangeIndex(-1)
{
    std::ifstream Config(FilePath);
    mIsChangeableAlert.setFillColor(mAlertColor);
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
            bool isThereSameKey = false;
            int sameKeyIndex = 0;
            for (size_t i = 0; i < KeyAmount; ++i)
            {
                if (event.key.code == Keys[i]
                &&  mButtonToChangeIndex != i)
                {
                    isThereSameKey = true;
                    sameKeyIndex = i;
                }
            }

            Keys[mButtonToChangeIndex] = event.key.code;
            if (isThereSameKey)
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

// size_t Settings::controlKeyAmount(std::string& Information)
// {
//     size_t i = 0;
//     for (; Information[i] != '\0' 
//         && Information[i] >= '0' && Information[i] <= '9'
//         ; i++)
//     { }
//     if (i != Information.length())
//     {
//         // Set default
//         return 0;
//     }
    
//     KeyAmount = std::stoi(Information);
// }

// void Settings::controlKeys(std::string& Information)
// {
// }

// void Settings::setTextFont(std::string& Information)
// {

// }

// void Settings::setColorText(std::string& Information)
// {

// }

// void Settings::setCharacterSize(std::string& Information)
// {

// }

// void Settings::setDistance(std::string& Information)
// {

// }

// void Settings::setButtonImage(std::string& Information)
// {

// }

// void Settings::setButtonImageTransparency(std::string& Information)
// {

// }

// void Settings::setAnimationVelocity(std::string& Information)
// {

// }

// void Settings::setAnimationColor(std::string& Information)
// {

// }

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