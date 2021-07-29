#include "../Headers/Settings.hpp"
#include "../Headers/ConfigHelper.hpp"
#include "../Headers/DefaultFiles.hpp"

#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderWindow.hpp>


const std::size_t Settings::mFramesPerSecond = 60;

// [Keys] [Mouse]
std::vector<sf::Keyboard::Key> Settings::Keys({ });
std::vector<sf::Mouse::Button> Settings::MouseButtons({ });

// Non config parameters
std::size_t Settings::ButtonAmount(0);


// [Statistics text]
float Settings::StatisticsDistance(-1.f);
float Settings::SpaceBetweenButtonsAndStatistics(-1.f);
std::string Settings::StatisticsFontPath("none");
sf::Color Settings::StatisticsTextColor(sf::Color::Transparent);
std::size_t Settings::StatisticsTextCharacterSize(0);
bool Settings::StatisticsBold(-1);
bool Settings::StatisticsItalic(-1);
bool Settings::ShowStatisticsText(-1);
bool Settings::ShowKPS(-1);
bool Settings::ShowMaxKPS(-1);
bool Settings::ShowTotal(-1);
bool Settings::ShowBPMText(-1);

// [Button text]
std::string Settings::KeyCountersFontPath("none");
sf::Color Settings::KeyCountersTextColor(sf::Color::Transparent);
std::size_t Settings::KeyCountersTextCharacterSize(0);
float Settings::KeyCounterWidth(-1.f);
float Settings::KeyCounterHeight(-1.f);
float Settings::KeyCountersHorizontalBounds(-1.f);
float Settings::KeyCountersVerticalBounds(-1.f);
bool Settings::KeyCountersBold(-1);
bool Settings::KeyCountersItalic(-1);
bool Settings::ShowSetKeysText(-1);
bool Settings::ShowKeyCountersText(-1);

// [Button graphics]
float Settings::ButtonDistance(-1.f);
std::string Settings::ButtonTexturePath("none");
sf::Vector2u Settings::ButtonTextureSize(0, 0);
sf::Color Settings::ButtonTextureColor(sf::Color::Transparent);

// [Animation graphics]
int Settings::AnimationStyle(-1);
std::string Settings::AnimationTexturePath("none");
std::size_t Settings::AnimationVelocity(0);
sf::Vector2f Settings::AnimationScale(-1.f, -1.f);
sf::Color Settings::AnimationColor(sf::Color::Transparent);
float Settings::AnimationOffset(-1.f);

// Non config parameters
sf::Color Settings::AnimationOnClickTransparency(sf::Color::Transparent); 
sf::Vector2f Settings::ScaledAnimationScale(AnimationScale);


// [Background]
std::string Settings::BackgroundTexturePath("none");
sf::Color Settings::BackgroundColor(sf::Color::Transparent);
bool Settings::ScaleBackground(-1);

// [Edit mode]
sf::Color Settings::HighlightedKeyColor(sf::Color::Transparent);

// [Main window]
bool Settings::WindowTitleBar(-1);
unsigned Settings::WindowBonusSizeTop(0);
unsigned Settings::WindowBonusSizeBottom(0);
unsigned Settings::WindowBonusSizeLeft(0);
unsigned Settings::WindowBonusSizeRight(0);

// [KPS window]
bool Settings::KPSWindowEnabledFromStart(-1);
sf::Vector2u Settings::KPSWindowSize(0, 0);
sf::Color Settings::KPSBackgroundColor(sf::Color::Transparent);
sf::Color Settings::KPSTextColor(sf::Color::Transparent);
sf::Color Settings::KPSNumberColor(sf::Color::Transparent);
std::string Settings::KPSWindowTextFontPath("none");
std::string Settings::KPSWindowNumberFontPath("none");
unsigned Settings::KPSTextSize(0);
unsigned Settings::KPSNumberSize(0);
float Settings::KPSWindowTopPadding(-1.f);
float Settings::KPSWindowDistanceBetween(-1.f);

// [Theme developer]
int Settings::ValueToMultiplyOnClick(-1);

unsigned char* Settings::StatisticsDefaultFont = MainProgramFont;
unsigned char* Settings::KeyCountersDefaultFont = MainProgramFont;
unsigned char* Settings::DefaultButtonTexture = ButtonTexture;
unsigned char* Settings::DefaultAnimationTexture = AnimationTexture;
unsigned char* Settings::DefaultBackgroundTexture = BackgroundTexture;
unsigned char* Settings::DefaultKPSWindowFont = KPSWindowDefaultFont;

// Hot keys
sf::Keyboard::Key Settings::KeyToIncrease(sf::Keyboard::Equal);
sf::Keyboard::Key Settings::AltKeyToIncrease(sf::Keyboard::Add);
sf::Keyboard::Key Settings::KeyToDecrease(sf::Keyboard::Dash);
sf::Keyboard::Key Settings::AltKeyToDecrease(sf::Keyboard::Subtract);
sf::Keyboard::Key Settings::KeyToClear(sf::Keyboard::X);
sf::Keyboard::Key Settings::KeyExit(sf::Keyboard::W);
sf::Keyboard::Key Settings::KeyToOpenKPSWindow(sf::Keyboard::K);
sf::Keyboard::Key Settings::KeyToOpenMenuWindow(sf::Keyboard::A);

bool Settings::mIsButtonSelected(false);
int Settings::mButtonToChangeIndex(-1);
sf::Keyboard::Key Settings::mButtonToChange(sf::Keyboard::Unknown);
bool Settings::mButtonAmountChanged(false);

Settings::Settings()
: mWindow(nullptr)
{ }

void Settings::handleEvent(sf::Event event)
{
    // Add/rm buttons and change one if it is selected
    if (event.type == sf::Event::KeyPressed)
    {
        sf::Keyboard::Key key = event.key.code;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
        {
            if ((key == KeyToIncrease || key == AltKeyToIncrease) 
            && Settings::Keys.size() < ConfigHelper::maxKeys)
            {
                addKey();
                ++ButtonAmount;
            }

            if ((key == KeyToDecrease || key == AltKeyToDecrease) 
            && Settings::Keys.size() > ConfigHelper::minKeys + 1)
            {
                removeKey();
                --ButtonAmount;
            }
        }

        if (mIsButtonSelected)
        {
            changeKey(key);
        }
    }

    // Select a button
    if (event.type == sf::Event::MouseButtonPressed
    && (event.mouseButton.button == sf::Mouse::Right))
    {
        bool isInRangeB = false;
        size_t i;
        for (i = 0; i < Settings::Keys.size(); ++i)
        {
            if (isInRange(i))
            {
                isInRangeB = true;
                break;
            }
        }

        // If mouse click was performed AND the cursor is not on a button
        // OR (it is on a button AND the selected button is the same one)
        if (!isInRangeB || (isInRangeB && mButtonToChangeIndex == i))
        {
            mIsButtonSelected = false;
            mButtonToChangeIndex = -1;
        }
        // If mouse click was performed AND the cursor is on a button 
        // AND (before there was no selected button OR it is a different button)
        else if (isInRangeB && (!mIsButtonSelected || mButtonToChangeIndex != i))
        {
            mIsButtonSelected = true;
            mButtonToChangeIndex = i;
        }
    }
    if (event.type == sf::Event::MouseButtonPressed
    && (event.mouseButton.button == sf::Mouse::Left))
    {
        mIsButtonSelected = false;
        mButtonToChangeIndex = -1;
    }
}

void Settings::update()
{
    mButtonAmountChanged = false;
}

void Settings::addKey()
{
    mButtonAmountChanged = true;
    Keys.emplace_back(ConfigHelper::getDefaultKey(Keys));
}

void Settings::removeKey()
{
    mButtonAmountChanged = true;
    Keys.pop_back();
}

void Settings::changeKey(sf::Keyboard::Key newKey)
{
    Keys[mButtonToChangeIndex] = newKey;
    if (newKey == sf::Keyboard::Unknown)
        Keys[mButtonToChangeIndex] = sf::Keyboard::A;

    unsigned sameKeyIndex = 0;
    while (ConfigHelper::isKeyAlreadyPresent(Keys, newKey, sameKeyIndex, mButtonToChangeIndex))
    {
        Keys[sameKeyIndex] = ConfigHelper::getDefaultKey(Keys);
    }

    mIsButtonSelected = false;
    mButtonToChangeIndex = -1;
}

bool Settings::isInRange(size_t index)
{
    sf::Vector2i mousePosition(sf::Mouse::getPosition(*mWindow));

    return  mousePosition.x > ButtonDistance * (index + 1) + ButtonTextureSize.x * index
        &&  mousePosition.x < ButtonDistance * (index + 1) + ButtonTextureSize.x * (index + 1)
        &&  mousePosition.y > ButtonDistance
        &&  mousePosition.y < ButtonDistance + ButtonTextureSize.y;
}

void Settings::setWindowReference(sf::RenderWindow& window)
{
    mWindow = &window;
}

bool Settings::wasButtonAmountChanged()
{
    return mButtonAmountChanged;
}

sf::Keyboard::Key Settings::getButtonToChange()
{
    return mButtonToChange;
}

int Settings::getButtonToChangeIndex()
{
    return mButtonToChangeIndex;
}