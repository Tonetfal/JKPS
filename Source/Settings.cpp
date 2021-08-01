#include "../Headers/Settings.hpp"
#include "../Headers/ConfigHelper.hpp"
#include "../Headers/DefaultFiles.hpp"

#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderWindow.hpp>


const std::size_t Settings::mFramesPerSecond = 60;

// [Keys] [Mouse]
std::vector<std::unique_ptr<LogicalKey>> Settings::LogicalKeys;
std::vector<std::unique_ptr<LogicalButton>> Settings::LogicalButtons;

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
std::string Settings::StatisticsKPSText("none");
std::string Settings::StatisticsKPS2Text("none");
std::string Settings::StatisticsMaxKPSText("none");
std::string Settings::StatisticsTotalText("none");
std::string Settings::StatisticsBPMText("none");

// [Button text]
std::string Settings::KeyCountersFontPath("none");
sf::Color Settings::KeyCountersTextColor(sf::Color::Transparent);
std::size_t Settings::KeyCountersTextCharacterSize(0);
sf::Vector2f Settings::KeyCounterPosition(-1.f, -1.f);
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

// Non config parameters

bool Settings::isGreenscreenSet(false);


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
unsigned char* Settings::DefaultGreenscreenBackgroundTexture = GreenscreenTexture;
unsigned char* Settings::DefaultKPSWindowFont = KPSWindowDefaultFont;


// Hot keys
sf::Keyboard::Key Settings::KeyToIncreaseKeys(sf::Keyboard::Equal);
sf::Keyboard::Key Settings::AltKeyToIncreaseKeys(sf::Keyboard::Add);
sf::Keyboard::Key Settings::KeyToDecreaseKeys(sf::Keyboard::Dash);
sf::Keyboard::Key Settings::AltKeyToDecreaseKeys(sf::Keyboard::Subtract);
sf::Keyboard::Key Settings::KeyToIncreaseButtons(sf::Keyboard::Period);
sf::Keyboard::Key Settings::KeyToDecreaseButtons(sf::Keyboard::Comma);
sf::Keyboard::Key Settings::KeyToClear(sf::Keyboard::X);
sf::Keyboard::Key Settings::KeyExit(sf::Keyboard::W);
sf::Keyboard::Key Settings::KeyToOpenKPSWindow(sf::Keyboard::K);
sf::Keyboard::Key Settings::KeyToOpenMenuWindow(sf::Keyboard::A);

bool Settings::mIsButtonSelected(false);
int Settings::mButtonToChangeIndex(-1);
sf::Keyboard::Key Settings::mButtonToChange(sf::Keyboard::Unknown);
bool Settings::mButtonAmountChanged(false);
bool Settings::mKeyWasChanged(false);
bool Settings::mReloadAssetsRequest(false);

Settings::Settings()
: mWindow(nullptr)
{ 
}

void Settings::handleEvent(sf::Event event)
{
    if (event.type == sf::Event::MouseButtonPressed
    && (event.mouseButton.button == sf::Mouse::Right))
        selectButton();

    // Add/rm buttons and change one if it is selected
    if (event.type == sf::Event::KeyPressed)
        changeKeysAmount(event.key.code);
}

void Settings::update()
{
    mButtonAmountChanged = mKeyWasChanged = false;
    static bool wasWindowOpenOnLastUpdate = false;
    if (mKeySelector->isOpen())
    {
        mKeySelector->handleOwnInput();
        mKeySelector->render();
    }
    if (wasWindowOpenOnLastUpdate && !mKeySelector->isOpen())
    {
        // deselect and save only after window closes
        mKeyWasChanged = wasWindowOpenOnLastUpdate;
        mIsButtonSelected = false;
        mButtonToChangeIndex = -1;
    }
    wasWindowOpenOnLastUpdate = mKeySelector->isOpen();
}

void Settings::changeKeysAmount(sf::Keyboard::Key clickedKey)
{
    sf::Keyboard::Key key = clickedKey;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
    {
        if ((key == KeyToIncreaseKeys || key == AltKeyToIncreaseKeys) 
        && Settings::LogicalKeys.size() < ConfigHelper::maxKeys)
        {
            addKeyboardKey();
            ++ButtonAmount;
        }

        if ((key == KeyToDecreaseKeys || key == AltKeyToDecreaseKeys) 
        && Settings::LogicalKeys.size() > ConfigHelper::minKeys)
        {
            removeKeyboardKey();
            --ButtonAmount;
        }

        if (key == KeyToIncreaseButtons && Settings::LogicalButtons.size() < ConfigHelper::maxButtons)
        {
            addMouseButton();
            ++ButtonAmount;
        }

        if (key == KeyToDecreaseButtons && Settings::LogicalButtons.size() > ConfigHelper::minButtons)
        {
            removeMouseButton();
            --ButtonAmount;
        }
    }
}

void Settings::selectButton()
{
    unsigned i;
    // If the cursor in on the button and there it isn't the selected one
    if (isPressPerformedOnButton(i) && (!mIsButtonSelected || mButtonToChangeIndex != i))
    {
        mIsButtonSelected = true;
        mButtonToChangeIndex = i;

        if (mButtonToChangeIndex < LogicalKeys.size())
        {
            mKeySelector->setKey(LogicalKeys[mButtonToChangeIndex].get(), nullptr);
            mKeySelector->openWindow();
        }
        else
        {
            mKeySelector->setKey(nullptr, LogicalButtons[mButtonToChangeIndex - LogicalKeys.size()].get());
            mKeySelector->openWindow();
        }
    }
}

void Settings::addKeyboardKey()
{
    mButtonAmountChanged = true;
    LogicalKeys.emplace_back(ConfigHelper::getDefaultLogicalKey(LogicalKeys));
}

void Settings::removeKeyboardKey()
{
    mButtonAmountChanged = true;
    LogicalKeys.pop_back();
}

void Settings::addMouseButton()
{
    mButtonAmountChanged = true;
    LogicalButtons.emplace_back(ConfigHelper::getDefaultLogicalButton(LogicalButtons));
}

void Settings::removeMouseButton()
{
    mButtonAmountChanged = true;
    LogicalButtons.pop_back();
}

bool Settings::isPressPerformedOnButton(unsigned &buttonIndex)
{
    for (buttonIndex = 0; buttonIndex < Settings::ButtonAmount; ++buttonIndex)
    {
        if (isMouseInRange(buttonIndex))
            return true;
    }
    return false;
}

bool Settings::isMouseInRange(unsigned index)
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
    mKeySelector->setMainWindowPointer(mWindow);
}

void Settings::buildKeySelector()
{
    std::unique_ptr<KeySelector> keySelector(new KeySelector);
    mKeySelector = std::move(keySelector);
}

bool Settings::wasButtonAmountChanged()
{
    return mButtonAmountChanged;
}

bool Settings::wasButtonChanged()
{
    return mKeyWasChanged;
}

sf::Keyboard::Key Settings::getButtonToChange()
{
    return mButtonToChange;
}

int Settings::getButtonToChangeIndex()
{
    return mButtonToChangeIndex;
}

void Settings::requestToReloadAssets()
{
    mReloadAssetsRequest = true;
}

bool Settings::resetReloadAssetsRequest() const
{
    return mReloadAssetsRequest && !(mReloadAssetsRequest = false);
}
