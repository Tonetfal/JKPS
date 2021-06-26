#include "../Headers/Settings.hpp"

std::string getReadingErrMsg(const std::string &parName)
{
    return "Reading error - " + parName + ". Default value will be set.\n";
}

template <typename T>
std::string getOutOfBoundsErrMsg(const std::string &parName, T bot, T top)
{
    return "Out of bounds - " + parName + " cannot be less than " + std::to_string(bot) + 
        " or more than " + std::to_string(top) + ". Default value will be set.\n";
}


const std::size_t Settings::mFramesPerSecond = 60;

std::vector<sf::Keyboard::Key> Settings::Keys({ sf::Keyboard::Key::Z
                                              , sf::Keyboard::Key::X });
std::vector<sf::Mouse::Button> Settings::MouseButtons({ });
std::size_t Settings::KeyAmount = Settings::Keys.size();
std::size_t Settings::MouseButtonAmount = Settings::MouseButtons.size();
std::size_t Settings::ButtonAmount = Settings::KeyAmount + Settings::MouseButtonAmount;

std::string Settings::StatisticsFontPath = "Default";
std::string Settings::KeyCountersFontPath = "Default";
sf::Color Settings::StatisticsTextColor(sf::Color::White);
sf::Color Settings::KeyCountersTextColor(sf::Color::Black);
std::size_t Settings::StatisticsTextCharacterSize = 12;
std::size_t Settings::KeyCountersTextCharacterSize = 22;
bool Settings::ShowStatisticsText = true;
bool Settings::ShowKeyCountersText = true;
bool Settings::ShowSetKeysText = false;
bool Settings::ShowBPMText = true;

float Settings::ButtonDistance = 7.f;
float Settings::SpaceBetweenButtonsAndStatistics = 10.f;
float Settings::SpaceOnStatisticsRight = 0.f;

std::string Settings::ButtonTexturePath = "Default";
std::string Settings::AnimationTexturePath = "Default";
sf::Vector2u Settings::ButtonTextureSize(50, 50);
sf::Color Settings::ButtonTextureColor(sf::Color(255,255,255,255));
sf::Color Settings::AnimationTextureColor(sf::Color(0,0,0,0));
std::size_t Settings::AnimationVelocity = 7;
sf::Color Settings::AnimationColor(sf::Color(255,255,255,255));
sf::Vector2f Settings::AnimationScale(1.f, 1.f);
sf::Vector2f Settings::ScaledAnimationScale(AnimationScale);
sf::Color Settings::AnimationOnClickTransparency(sf::Color(0,0,0,150)); 

std::string Settings::BackgroundTexturePath = "Default";
sf::Color Settings::BackgroundColor(sf::Color(44,28,31,180));

bool Settings::IsChangeable = false;
sf::Color Settings::HighlightedKeyColor = sf::Color(255,0,0);
sf::Color Settings::AlertColor = sf::Color(255,0,0);

unsigned char* Settings::StatisticsDefaultFont = DefaultFont;
unsigned char* Settings::KeyCountersDefaultFont = DefaultFont;
unsigned char* Settings::DefaultButtonTexture = DefaultButtonTextureHeader;
unsigned char* Settings::DefaultAnimationTexture = DefaultAnimationTextureHeader;
unsigned char* Settings::DefaultBackgroundTexture = DefaultBackgroundTextureHeaderArray;

sf::Keyboard::Key Settings::KeyToIncrease(sf::Keyboard::Equal);
sf::Keyboard::Key Settings::AltKeyToIncrease(sf::Keyboard::Add);
sf::Keyboard::Key Settings::KeyToDecrease(sf::Keyboard::Dash);
sf::Keyboard::Key Settings::AltKeyToDecrease(sf::Keyboard::Subtract);

Settings::Settings()
: configPath("KPS.cfg")
, errorLogPath("KPSErrorLog.txt")
, tmpConfigPath("tmpConfig.cfg")
, minimumKeys(0)
, maximumKeys(10)
, minimumMouseButtons(0)
, maximumMouseButtons(4)
, mWindow(nullptr)
, mIsChangeableAlert(5.f)
, mButtonAmountChanged(false)
, mButtonToChange(sf::Keyboard::Unknown)
, mButtonToChangeIndex(-1)
{
    mIsChangeableAlert.setFillColor(AlertColor);

    std::ifstream ifErrorLog(errorLogPath);
    if (ifErrorLog.is_open())
    {
        ifErrorLog.close();
        remove(errorLogPath.c_str());
    }

    remove(errorLogPath.c_str());

    std::ifstream Config(configPath);
    if (!Config.is_open())
    {
        std::cerr << "Config - Reading error. Default config will be generated.\n";
        createDefaultConfig();
        return;
    }

    std::ofstream ofErrorLog(errorLogPath);
    if (!ofErrorLog.is_open())
    {
        std::cerr << "Error log - Creating error. Error log cannot be created.\n";
        return;
    }

    // [Keys] [Mouse]
    setupKey(Keys, findParameter("Keys"), "Keys", ofErrorLog);
    setupMouseButton(MouseButtons, findParameter("Buttons"), "Buttons", ofErrorLog);

    // [Statistics]
    setupFilePathParameter(StatisticsFontPath, findParameter("Statistics font"), "Statistics font", ofErrorLog);
    setupFilePathParameter(KeyCountersFontPath, findParameter("Key counters font"), "Key counters font", ofErrorLog);
    setupColor(StatisticsTextColor, findParameter("Statistics text color"), "Statistics text color", ofErrorLog);
    setupColor(KeyCountersTextColor, findParameter("Key counters text color"), "Key counters text color", ofErrorLog);
    setupDigitParameter(StatisticsTextCharacterSize, 0, 100, findParameter("Statistics character size"), "Statistics character size", ofErrorLog);
    setupDigitParameter(KeyCountersTextCharacterSize, 0, 100, findParameter("Key counters character size"), "Key counters character size", ofErrorLog);
    setupBoolParameter(ShowStatisticsText, findParameter("Show statistics"), "Show statistics", ofErrorLog);
    setupBoolParameter(ShowSetKeysText, findParameter("Only show set keys"), "Only show set keys", ofErrorLog);
    setupBoolParameter(ShowKeyCountersText, findParameter("Show key counters"), "Show key counters", ofErrorLog);
    setupBoolParameter(ShowBPMText, findParameter("Show BPM"), "Show BPM", ofErrorLog);

    // [Spacing]
    setupDigitParameter(ButtonDistance, 0, 100, findParameter("Button distance"), "Button distance", ofErrorLog);
    setupDigitParameter(SpaceBetweenButtonsAndStatistics, 0, 200, findParameter("Space between buttons and statistics"), "Space between buttons and statistics", ofErrorLog);
    setupDigitParameter(SpaceOnStatisticsRight, 0, 200, findParameter("Space on the statistics right"), "Space on the statistics right", ofErrorLog);

    // [Button graphics]
    setupFilePathParameter(ButtonTexturePath, findParameter("Button texture"), "Button texture", ofErrorLog);
    setupFilePathParameter(AnimationTexturePath, findParameter("Animation texture"), "Animation texture", ofErrorLog);
    setupVector(ButtonTextureSize, 0, 250, findParameter("Button texture size"), "Button texture size", ofErrorLog);
    setupColor(ButtonTextureColor,findParameter("Button texture color"), "Button texture color", ofErrorLog);
    float tmp = 1.f;
    setupDigitParameter(tmp, 0, 2, findParameter("Animation scale on click"), "Animation scale on click", ofErrorLog);
    AnimationScale = sf::Vector2f(tmp, tmp);
    setupColor(AnimationColor, findParameter("Animation color"), "Animation color", ofErrorLog);
    setupDigitParameter(AnimationVelocity, 0, 120, findParameter("Animation velocity"), "Animation velocity", ofErrorLog);

    // [Background]
    setupFilePathParameter(BackgroundTexturePath, findParameter("Background texture"), "Background texture", ofErrorLog);
    setupColor(BackgroundColor, findParameter("Background color"), "Background color", ofErrorLog);

    // [Other]
    setupColor(AlertColor, findParameter("Changeability alert color"), "Changeability alert color", ofErrorLog);
    setupColor(HighlightedKeyColor, findParameter("Highlighted text button color"), "Highlighted text button color", ofErrorLog);
    mIsChangeableAlert.setFillColor(AlertColor);

    ofErrorLog.close();

    ifErrorLog.open(errorLogPath);
    if (!ifErrorLog.is_open())
    {
        std::cerr << "Error log test - Reading error. Error log cannot be read.\n";
        Config.close();
        return;
    }
    
    int isEmpty = ifErrorLog.peek();
    ifErrorLog.close();
    Config.close();

    if (isEmpty == -1)
        remove(errorLogPath.c_str());
}

void Settings::handleEvent(sf::Event event)
{
    if (event.type == sf::Event::KeyPressed)
    {
        sf::Keyboard::Key key = event.key.code;
        if ((key == KeyToIncrease || key == AltKeyToIncrease)
        &&  KeyAmount <= maximumKeys)
        {
            ++KeyAmount;
            ++ButtonAmount;
            mButtonAmountChanged = true;

            Keys.resize(KeyAmount);
            setDefaultKey(Keys.size() - 1);
            saveSettings();
        } 
        else if ((key == KeyToDecrease || key == AltKeyToDecrease)
        &&  KeyAmount > minimumKeys)
        {
            --KeyAmount;
            --ButtonAmount;
            mButtonAmountChanged = true;

            Keys.resize(KeyAmount);
            saveSettings();
        }
        // Change key step 1
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
        // Change key step 2
        else
        {
            Keys[mButtonToChangeIndex] = event.key.code;

            size_t sameKeyIndex = 0;
            while (isThereSameKey(event.key.code, sameKeyIndex, mButtonToChangeIndex))
                setDefaultKey(sameKeyIndex);

            saveSettings();
            mButtonToChange = sf::Keyboard::Unknown;
            mButtonToChangeIndex = -1;
        }
    }

    if (event.type == sf::Event::MouseButtonPressed)
    {
        bool isInRangeB = false;
        size_t i;
        for (i = 0; i < Settings::ButtonAmount; ++i)
        {
            if (isInRange(i))
            {
                isInRangeB = true;
                break;
            }
        }

        if (isInRangeB)
        {
            // Select
            if (event.mouseButton.button == sf::Mouse::Left
            &&  mButtonToChange == sf::Keyboard::Unknown)
            {
                mButtonToChange = Keys[i];
                mButtonToChangeIndex = i;
                return;
            }
            // Deselect
            if (event.mouseButton.button == sf::Mouse::Left)
            {
                mButtonToChange = sf::Keyboard::Unknown;
                mButtonToChangeIndex = -1;
            }
        }
    }
}

void Settings::update()
{
    mButtonAmountChanged = false;
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

    if (line.compare(0, parameterName.size(), parameterName))
        return "";
    if (line.length() <= parameterName.length() + 2)
        return "";

    // Remove parameter name, ':' and space after it
    return line.substr(parameterName.length() + 2, 81);
}

template <typename T>
void Settings::setupDigitParameter(  T& parameter
                        ,       int limitMin
                        ,       int limitMax
                        , const std::string information
                        , const std::string parameterName
                        ,       std::ofstream& errorLog )
                        
{
    if (information == "")
    {
        errorLog << getReadingErrMsg(parameterName);
        return;
    }

    T tmp = 0;
    // stof and not stoi because some values can be float, but int values
    // will be parsed automatically
    tmp = std::stof(information);

    if (tmp < limitMin || tmp > limitMax)
    {
        errorLog << getOutOfBoundsErrMsg(parameterName, limitMin, limitMax);
        return;
    }

    parameter = tmp;
}

void Settings::setupColor(sf::Color& color
                    , const std::string information
                    , const std::string parameterName
                    ,       std::ofstream& errorLog)
{
    if (information == "")
    {
        errorLog << getReadingErrMsg(parameterName);
        return;
    }
    
    int rgba[4] = { color.r,color.g,color.b,color.a };
    size_t stringIndex = 0;
    for (size_t i = 0; i < 4; i++)
    {
        // Visual Studio doesn't want to compile w/o it :( VS Code does
        if (i > 0)
        {
            // If there is no transparency code
            if (information[stringIndex-1] != ',' && i == 3)
                break;
        }

        rgba[i] = std::stoi(information.substr(stringIndex, 81));

        if (rgba[i] < 0 || rgba[i] > 255)
        {
            errorLog << getOutOfBoundsErrMsg(parameterName, 0, 255);
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
                    , const std::string parameterName
                    ,       std::ofstream& errorLog)
{
    if (information == "")
    {
        errorLog << getReadingErrMsg(parameterName);
        return;
    }

    float tmp[2] = { 0 };
    size_t stringIndex = 0;
    for (size_t i = 0; i < 2; i++)
    {
        // stof and not stoi because some values can be float, but int values
        // will be parsed automatically
        tmp[i] = std::stof(information.substr(stringIndex, 81));

        if (tmp[i] < limitMin || tmp[i] > limitMax)
        {
            errorLog << getOutOfBoundsErrMsg(parameterName, limitMin, limitMax);
            return;
        }

        // Set index on next number
        stringIndex += std::to_string(int(tmp[i])).length() + 1;
    }
    
    vector = T(tmp[0], tmp[1]);
}

void Settings::setupFilePathParameter(  std::string& parameter
                                , const std::string information
                                , const std::string parameterName
                                ,       std::ofstream& errorLog )
{
    if (information == "Default")
        return;

    if (information == "")
    {
        errorLog << getReadingErrMsg(parameterName);
        return;
    }

    std::ifstream test(information);
    if (!test.is_open())
    {
        errorLog << getReadingErrMsg(parameterName);
        test.close();
        return;
    }
    test.close();

    parameter = information;
}

void Settings::setupKey(std::vector<sf::Keyboard::Key>& keys
                , const std::string information
                , const std::string parameterName
                ,       std::ofstream& errorLog )
{
    if (information == "")
    {
        errorLog<< getReadingErrMsg(parameterName);
        return;
    }

    if (information == "No" || information == "no" || information == "NO")
    {
        keys.resize(0);
        KeyAmount = keys.size();
        ButtonAmount = KeyAmount; // mouse buttons will be added in their setuper
        return;
    }

    size_t stringIndex = 0, nKeys = 0;
    for (size_t i = 0
        ; information.length() > stringIndex && nKeys < maximumKeys
        ; ++i, ++nKeys)
    {
        std::string keyStr;
        for (size_t j = stringIndex
            ; information[j] != ',' && information[j] != '\0'
            ; ++j)
        {
            keyStr += information[j];
        }
        
        stringIndex = information.find(',', stringIndex) + 1;

        sf::Keyboard::Key foundKey = convertStringToKey(keyStr);

        if (foundKey == sf::Keyboard::Unknown)
        {
            errorLog << getReadingErrMsg(parameterName);
            break;
        }

        keys.resize(i + 1);
        keys[i] = foundKey;
        size_t whichOne;
        while (isThereSameKey(foundKey, whichOne, i))
            setDefaultKey(whichOne);

        // find function returns 0 if the value was not found
        if (stringIndex == 0)
            break;
    }

    KeyAmount = keys.size();
    ButtonAmount = KeyAmount; // mouse buttons will be added in their setuper
}

void Settings::setupMouseButton(std::vector<sf::Mouse::Button>& mouseButtons
                        , const std::string information
                        , const std::string parameterName
                        ,       std::ofstream& errorLog)
{
    if (information == "")
    {
        errorLog<< getReadingErrMsg(parameterName);
        return;
    }

    if (information == "No" || information == "no" || information == "NO")
    {
        mouseButtons.resize(0);
        MouseButtonAmount = mouseButtons.size();
        ButtonAmount += MouseButtonAmount;
        return;
    }

    size_t stringIndex = 0, nButtons = 0;
    for (size_t i = 0
        ; information.length() > stringIndex && nButtons < maximumMouseButtons
        ; ++i, ++nButtons)
    {
        std::string buttonStr;
        for (size_t j = stringIndex
            ; information[j] != ',' && information[j] != '\0'
            ; ++j)
        {
            buttonStr += information[j];
        }
        
        stringIndex = information.find(',', stringIndex) + 1;

        sf::Mouse::Button foundMouseButton = convertStringToButton(buttonStr);

        if (foundMouseButton == sf::Mouse::ButtonCount)
        {
            errorLog << getReadingErrMsg(parameterName);
            break;
        }

        mouseButtons.resize(i + 1);
        mouseButtons[i] = foundMouseButton;

        // find function returns 0 if the value was not found
        if (stringIndex == 0)
            break;
    }

    MouseButtonAmount = mouseButtons.size();
    ButtonAmount += MouseButtonAmount;
}


void Settings::setupBoolParameter(  bool& parameter
                            , const std::string information
                            , const std::string parameterName
                            ,       std::ofstream& errorLog)
                        
{
    if (information == "true" || information == "false"
    ||  information == "True" || information == "False"
    ||  information == "TRUE" || information == "FALSE")
    {
        parameter = !(information == "false" || information == "FALSE" || information == "False");
    }
    else
        errorLog << getReadingErrMsg(parameterName);
}

bool Settings::isThereSameKey(sf::Keyboard::Key key, size_t& whichOne, size_t indexToIgnore)
{
    for (size_t i = 0; i < Keys.size(); ++i)
    {
        if (Keys[i] == key && i != indexToIgnore)
        {
            whichOne = i;
            return true;
        }
    }
    
    return false;
}

void Settings::setDefaultKey(size_t index)
{
    // sf::Keyboard::Key is an enum, that starts from 0
    size_t keyToAssign = 0;

    for (size_t i = 0; i < Settings::KeyAmount; i++)
    {
        if (keyToAssign == Keys[i] && i != index)
        {
            ++keyToAssign;
            i = -1;
            // -1 cuz I need to recheck to be sure that there is no same key
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

bool Settings::wasButtonAmountChanged()
{
    return mButtonAmountChanged;
}

void Settings::saveSettings()
{
    std::ofstream ofConfigTry(configPath, std::fstream::in);
    if (!ofConfigTry.is_open())
    {
        createDefaultConfig();
        std::cerr << "Config - Reading error. Default config will be generated.\n";
        return;
    }
    ofConfigTry.close();

    std::ofstream ofConfig(tmpConfigPath);
    writeKeys(ofConfig);

    ofConfig.close();

    remove(configPath.c_str());
    rename(tmpConfigPath.c_str(), configPath.c_str());
}

void Settings::createDefaultConfig()
{
    std::ofstream Config(configPath);
    if (!Config.is_open())
    {
        std::cerr << "Config - Creating error. Config cannot be created.\n";
        return;
    }

    Config << DefaultConfigString;

    Config.close();
}

void Settings::writeKeys(std::ofstream& ofConfig)
{
    std::ifstream ifConfig(configPath);

    std::string line, toFind = "[Keys]";
    bool found = false;
    size_t i = 0, lineN = 0;

    while (!ifConfig.eof())
    {
        getline(ifConfig, line);
        ++lineN;

        if (line.length() < toFind.length())
            continue;

        if(line.substr(0, toFind.length()) == toFind)
        {
            found = true;
            break;
        }
    }

    ifConfig.close();

    ifConfig.open(configPath);
    // Insert everything before Keys
    for (size_t i = 0; i < lineN; ++i)
    {
        getline(ifConfig, line);
        ofConfig << line << "\n";
    }

    // Skip line to replace
    getline(ifConfig, line);

    // If there is no Keys section
    if (!found)
        ofConfig << "\n" << toFind << "\n";

    ofConfig << "Keys: ";
    if (Keys.size() > 0)
    {
        for (size_t i = 0; i < Keys.size(); ++i)
        {
            ofConfig << convertKeyToString(Keys[i]) << (i+1 != Keys.size() ? "," : "\n");
        }
    }
    else
    {
        ofConfig << "No\n";
    }

    // Insert everything after Keys
    while (!ifConfig.eof())
    {
        getline(ifConfig, line);
        ofConfig << line << (ifConfig.peek() == EOF ? "" : "\n");
    }

    ifConfig.close();
}

bool Settings::isInRange(size_t index)
{
    sf::Vector2i mousePosition(sf::Mouse::getPosition(*mWindow));

    return  mousePosition.x > ButtonDistance * (index + 1) + ButtonTextureSize.x * index
        &&  mousePosition.x < ButtonDistance * (index + 1) + ButtonTextureSize.x * (index + 1)
        &&  mousePosition.y > ButtonDistance
        &&  mousePosition.y < ButtonDistance + ButtonTextureSize.y;
}

sf::Keyboard::Key Settings::getButtonToChange()
{
    return mButtonToChange;
}

int Settings::getButtonToChangeIndex()
{
    return mButtonToChangeIndex;
}