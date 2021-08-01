#include "../Headers/Menu.hpp"
#include "../Headers/ConfigHelper.hpp"
#include "../Headers/Default media/Textures/vMark.hpp"
#include "../Headers/Default media/Textures/xMark.hpp"
#include "../Headers/Default media/Textures/RGB-Circle.hpp"
#include "../Headers/Default media/Textures/RefreshTexture.hpp"
#include "../Headers/Default media/Fonts/arial_monospaced.hpp"
#include "../Headers/Default media/Fonts/RobotoMono.hpp"

#include <SFML/Window/Event.hpp>
#include <SFML/System/Clock.hpp>


std::string Menu::mProgramVersion("v0.8-alpha");

Menu::Menu()
: mScrollSpeed(40.f)
, mSliderBar(sf::Vector2f(10, 200))
, mSliderBarDefaultColor(sf::Color(103,103,103))
, mSliderBarAimedColor(sf::Color(123,123,123))
, mSliderBarPressedColor(sf::Color(161,161,161))
{
    loadFonts();
    loadTextures();

    buildParametersMap();
    ConfigHelper::readConfig(mParameters, mParameterLines);
    buildParameterLines();
    saveConfig();

    mSettings.buildKeySelector();

    mSliderBar.setOrigin(mSliderBar.getSize() / 2.f);
    mSliderBar.setPosition(806, 100);
    mSliderBar.setFillColor(mSliderBarDefaultColor);
}

void Menu::handleEvent(sf::Event event)
{
    if (event.type == sf::Event::KeyPressed
    &&  event.key.code == Settings::KeyToOpenMenuWindow
    &&  sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
    {
        if (!mWindow.isOpen())
        {
            sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
            mWindow.create(sf::VideoMode(812, 600), "JKPS Menu", sf::Style::Close);
            mWindow.setPosition(sf::Vector2i(
                desktop.width  / 2 - mWindow.getSize().x / 2, 
                desktop.height / 2 - mWindow.getSize().y / 2));
            mView = mWindow.getView();
            mView.setCenter(mView.getCenter().x, 0);
        }
        else
        {
            mWindow.close();
            ParameterLine::deselectValue();
        }
    }
}

void Menu::handleOwnEvent()
{
    sf::Event event;
    static bool scrollCursorClicked;
    while (mWindow.pollEvent(event))
    { 
        for (auto &pair : mParameterLines)
            pair.second->handleEvent(event);

        if (event.type == sf::Event::MouseWheelScrolled || event.type == sf::Event::KeyPressed)
        {
            const sf::Vector2f viewCenter = mView.getCenter();
            float offset = 0.f;

            if (event.type == sf::Event::MouseWheelScrolled)
            {
                offset = -mScrollSpeed * event.mouseWheelScroll.delta;
            }
            if (event.type == sf::Event::KeyPressed)
            {
                sf::Keyboard::Key key = event.key.code;
                const sf::Vector2f viewSize = mView.getSize();
                if (key == sf::Keyboard::PageUp)
                    offset = -viewSize.y;
                if (key == sf::Keyboard::PageDown)
                    offset = viewSize.y;
            }

            moveSliderBarButtons(offset);
            returnViewInBounds();
        }
        if (event.type == sf::Event::MouseButtonPressed 
        ||  event.type == sf::Event::MouseButtonReleased
        ||  event.type == sf::Event::MouseMoved)
        {
            sf::Vector2i mousePos(sf::Mouse::getPosition(mWindow));
            sf::Color colorToSet = mSliderBarDefaultColor;
            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left)
                scrollCursorClicked = false;
            if (mSliderBar.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)))
            {
                colorToSet = mSliderBarAimedColor;
                scrollCursorClicked = sf::Mouse::isButtonPressed(sf::Mouse::Left);
            }
            if (scrollCursorClicked)
                colorToSet = mSliderBarPressedColor;

            mSliderBar.setFillColor(colorToSet);
            if (event.type == sf::Event::MouseMoved)
            {
                mousePos.x = event.mouseMove.x;
                mousePos.y = event.mouseMove.y;
            }
            if (scrollCursorClicked)
            {
                moveSliderBarMouse(mousePos);
            }
        }

        if (event.type == sf::Event::Closed
        || (event.type == sf::Event::KeyPressed
        && event.key.code == Settings::KeyExit
        && sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)))
        {
            mWindow.close();
        }
    }
    if (!ParameterLine::isValueSelected())
    {
        float offset = 0.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            offset = -mScrollSpeed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            offset = mScrollSpeed;
        if (offset != 0.f)
        {
            moveSliderBarButtons(offset);
            returnViewInBounds();
        }
    }

    for (auto &pair : mParameterLines)
        pair.second->processInput();

    for (auto elem : mParameters)
        if (elem.second->resetChangeState())
            mChangedParametersQueue.push(elem);
            
    if (!mChangedParametersQueue.isEmpty())
        saveConfig();
}

void Menu::update()
{    
    
}

void Menu::render() 
{
    mWindow.setView(mView);

    mWindow.clear(sf::Color(45,45,45));

    for (auto &pair : mParameterLines)
        mWindow.draw(*pair.second);

    mWindow.setView(mWindow.getDefaultView());
    mWindow.draw(mSliderBar);
    mWindow.setView(mView);


    mWindow.display();
}

LogicalParameter &Menu::getParameter(LogicalParameter::ID id)
{
    auto found = mParameters.find(id);
    assert(found != mParameters.end());

    return *found->second;
}

ChangedParametersQueue &Menu::getChangedParametersQueue()
{
    return mChangedParametersQueue;
}

Settings &Menu::getSettings()
{
    return mSettings;
}

void Menu::loadFonts()
{
    mFonts.loadFromMemory(Fonts::Parameter, RobotoMono, 1100000);
    mFonts.loadFromMemory(Fonts::Value, RobotoMono, 1100000);
}

void Menu::loadTextures()
{
    mTextures.loadFromMemory(Textures::rgbCircle, RGB_Circle, 10700);
    mTextures.loadFromMemory(Textures::vMark, vMark, 6000);
    mTextures.loadFromMemory(Textures::xMark, xMark, 6100);
    mTextures.loadFromMemory(Textures::Refresh, RefreshTexture, 6000);
}

void Menu::buildParametersMap()
{
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextDist, new LogicalParameter(LogicalParameter::Type::Float, &Settings::StatisticsDistance, "Statistics text distance", "5", -500, 500)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::SpaceBtwBtnAndStat, new LogicalParameter(LogicalParameter::Type::Float, &Settings::SpaceBetweenButtonsAndStatistics, "Space between buttons and statistics", "15", -500, 500)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextFont, new LogicalParameter(LogicalParameter::Type::StringPath, &Settings::StatisticsFontPath, "Statistics text font", "Default")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextClr, new LogicalParameter(LogicalParameter::Type::Color, &Settings::StatisticsTextColor, "Statistics text color", "255,255,255,255")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextChSz, new LogicalParameter(LogicalParameter::Type::Unsigned, &Settings::StatisticsTextCharacterSize, "Statistics text character size", "14", 0, 500)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextBold, new LogicalParameter(LogicalParameter::Type::Bool, &Settings::StatisticsBold, "Statistics text bold", "False")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextItal, new LogicalParameter(LogicalParameter::Type::Bool, &Settings::StatisticsItalic, "Statistics text italic", "False")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextShow, new LogicalParameter(LogicalParameter::Type::Bool, &Settings::ShowStatisticsText, "Show statistics text", "True")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextShowKPS, new LogicalParameter(LogicalParameter::Type::Bool, &Settings::ShowKPS, "Show KPS", "True")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextShowMaxKPS, new LogicalParameter(LogicalParameter::Type::Bool, &Settings::ShowMaxKPS, "Show max KPS on separate line", "True")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextShowTotal, new LogicalParameter(LogicalParameter::Type::Bool, &Settings::ShowTotal, "Show total", "True")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextShowBPM, new LogicalParameter(LogicalParameter::Type::Bool, &Settings::ShowBPMText, "Show BPM", "True")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextKPSText, new LogicalParameter(LogicalParameter::Type::String, &Settings::StatisticsKPSText, "KPS text", "KPS")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextKPS2Text, new LogicalParameter(LogicalParameter::Type::String, &Settings::StatisticsKPS2Text, "KPS text when it is 0 and Max KPS is disabled", "Max")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextMaxKPSText, new LogicalParameter(LogicalParameter::Type::String, &Settings::StatisticsMaxKPSText, "Max KPS text", "Max KPS")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextTotalText, new LogicalParameter(LogicalParameter::Type::String, &Settings::StatisticsTotalText, "Total text", "Total")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextBPMText, new LogicalParameter(LogicalParameter::Type::String, &Settings::StatisticsBPMText, "BPM text", "BPM")));

    mParameters.emplace(std::make_pair(LogicalParameter::ID::BtnTextFont, new LogicalParameter(LogicalParameter::Type::StringPath, &Settings::KeyCountersFontPath, "Buttons text font", "Default")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::BtnTextClr, new LogicalParameter(LogicalParameter::Type::Color, &Settings::KeyCountersTextColor, "Buttons text color", "255,255,255,255")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::BtnTextChSz, new LogicalParameter(LogicalParameter::Type::Unsigned, &Settings::KeyCountersTextCharacterSize, "Buttons text character size", "14", 0, 500)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::BtnTextWidth, new LogicalParameter(LogicalParameter::Type::Float, &Settings::KeyCounterWidth, "Buttons text width", "0", -500, 500)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::BtnTextHeight, new LogicalParameter(LogicalParameter::Type::Float, &Settings::KeyCounterHeight, "Buttons text height", "0", -500, 500)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::BtnTextHorzBounds, new LogicalParameter(LogicalParameter::Type::Float, &Settings::KeyCountersHorizontalBounds, "Buttons text horizontal bounds", "4", -500, 500)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::BtnTextVertBounds, new LogicalParameter(LogicalParameter::Type::Float, &Settings::KeyCountersVerticalBounds, "Buttons text vertical bounds", "4", -500, 500)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::BtnTextBold, new LogicalParameter(LogicalParameter::Type::Bool, &Settings::KeyCountersBold, "Buttons text bold", "False")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::BtnTextItal, new LogicalParameter(LogicalParameter::Type::Bool, &Settings::KeyCountersItalic, "Buttons text italic", "False")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::BtnTextShowKeys, new LogicalParameter(LogicalParameter::Type::Bool, &Settings::ShowSetKeysText, "Only show set keys", "True")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::BtnTextShowKeyCtrs, new LogicalParameter(LogicalParameter::Type::Bool, &Settings::ShowKeyCountersText, "Show key counters", "True")));

    mParameters.emplace(std::make_pair(LogicalParameter::ID::BtnGfxDist, new LogicalParameter(LogicalParameter::Type::Float, &Settings::ButtonDistance, "Button distance", "10", -500, 500)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::BtnGfxTxtr, new LogicalParameter(LogicalParameter::Type::StringPath, &Settings::ButtonTexturePath, "Button texture", "Default")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::BtnGfxTxtrSz, new LogicalParameter(LogicalParameter::Type::VectorU, &Settings::ButtonTextureSize, "Button texture size", "60,60", 0, 500)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::BtnGfxTxtrClr, new LogicalParameter(LogicalParameter::Type::Color, &Settings::ButtonTextureColor, "Button texture color", "30,30,30,255")));

    mParameters.emplace(std::make_pair(LogicalParameter::ID::AnimGfxStl, new LogicalParameter(LogicalParameter::Type::Unsigned, &Settings::AnimationStyle, "Animation style", "1", 1, 2)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::AnimGfxTxtr, new LogicalParameter(LogicalParameter::Type::StringPath, &Settings::AnimationTexturePath, "Animation texture", "Default")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::AnimGfxVel, new LogicalParameter(LogicalParameter::Type::Unsigned, &Settings::AnimationVelocity, "Animation velocity", "5", 1, 120)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::AnimGfxScl, new LogicalParameter(LogicalParameter::Type::VectorF, &Settings::AnimationScale, "Animation scale on click (%)", "100,100", 0, 200)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::AnimGfxClr, new LogicalParameter(LogicalParameter::Type::Color, &Settings::AnimationColor, "Animation color", "255,180,0,255")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::AnimGfxOffset, new LogicalParameter(LogicalParameter::Type::Float, &Settings::AnimationOffset, "Animation offset", "3", -100, 100)));

    mParameters.emplace(std::make_pair(LogicalParameter::ID::BgTxtr, new LogicalParameter(LogicalParameter::Type::StringPath, &Settings::BackgroundTexturePath, "Background texture", "Default")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::BgClr, new LogicalParameter(LogicalParameter::Type::Color, &Settings::BackgroundColor, "Background color", "170,170,170,255")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::BgScale, new LogicalParameter(LogicalParameter::Type::Bool, &Settings::ScaleBackground, "Scale background texture if it does not fit", "True")));

    mParameters.emplace(std::make_pair(LogicalParameter::ID::MainWndwTitleBar, new LogicalParameter(LogicalParameter::Type::Bool, &Settings::WindowTitleBar, "Window title bar", "False")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::MainWndwTop, new LogicalParameter(LogicalParameter::Type::Unsigned, &Settings::WindowBonusSizeTop, "Window bonus size top", "6", 0, 500)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::MainWndwBot, new LogicalParameter(LogicalParameter::Type::Unsigned, &Settings::WindowBonusSizeBottom, "Window bonus size bottom", "6", 0, 500)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::MainWndwLft, new LogicalParameter(LogicalParameter::Type::Unsigned, &Settings::WindowBonusSizeLeft, "Window bonus size left", "6", 0, 500)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::MainWndwRght, new LogicalParameter(LogicalParameter::Type::Unsigned, &Settings::WindowBonusSizeRight, "Window bonus size right", "130", 0, 500)));

    mParameters.emplace(std::make_pair(LogicalParameter::ID::KPSWndwEn, new LogicalParameter(LogicalParameter::Type::Bool, &Settings::KPSWindowEnabledFromStart, "Enable from start", "False")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::KPSWndwSz, new LogicalParameter(LogicalParameter::Type::VectorU, &Settings::KPSWindowSize, "Window size", "300,300", 0, 1000)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::KPSWndwTxtChSz, new LogicalParameter(LogicalParameter::Type::Unsigned, &Settings::KPSTextSize, "Text character size:", "130", 0, 500)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::KPSWndwNumChSz, new LogicalParameter(LogicalParameter::Type::Unsigned, &Settings::KPSNumberSize, "Number character size", "100", 0, 500)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::KPSWndwBgClr, new LogicalParameter(LogicalParameter::Type::Color, &Settings::KPSBackgroundColor, "KPS Background color", "0,177,64,255")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::KPSWndwTxtClr, new LogicalParameter(LogicalParameter::Type::Color, &Settings::KPSTextColor, "KPS text color", "255,255,255,255")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::KPSWndwNumClr, new LogicalParameter(LogicalParameter::Type::Color, &Settings::KPSNumberColor, "KPS number color", "255,255,255,255")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::KPSWndwTxtFont, new LogicalParameter(LogicalParameter::Type::StringPath, &Settings::KPSWindowTextFontPath, "KPS window text font", "Default")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::KPSWndwNumFont, new LogicalParameter(LogicalParameter::Type::StringPath, &Settings::KPSWindowNumberFontPath, "KPS window number font", "Default", -50)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::KPSWndwTopPadding, new LogicalParameter(LogicalParameter::Type::Float, &Settings::KPSWindowTopPadding, "KPS top padding", "20", -500, 500)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::KPSWndwDistBtw, new LogicalParameter(LogicalParameter::Type::Float, &Settings::KPSWindowDistanceBetween, "KPS extra window distance between text", "50", -500, 500)));

    mParameters.emplace(std::make_pair(LogicalParameter::ID::OtherHighText, new LogicalParameter(LogicalParameter::Type::Color, &Settings::HighlightedKeyColor, "Highlighted text button color", "210,30,210,255")));

    mParameters.emplace(std::make_pair(LogicalParameter::ID::ThemeDevMultpl, new LogicalParameter(LogicalParameter::Type::Unsigned, &Settings::ValueToMultiplyOnClick, "Value to multiply on click", "1", 0, 1000000)));
}

void Menu::buildParameterLines()
{
    for (auto &pair : mParameters)
    {
        mParameterLines.emplace(std::make_pair(ParameterLine::parIdToParLineId(pair.first), 
            new ParameterLine(pair.second, mFonts, mTextures, mWindow)));
    }
    
    typedef std::shared_ptr<LogicalParameter> sPtr;
    sPtr parP = nullptr;
    sPtr emptyP(new LogicalParameter(LogicalParameter::Type::Empty, nullptr));

    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Collection, nullptr, "[Statistics text]"));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::StatTextColl, new ParameterLine(parP, mFonts, mTextures, mWindow)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::StatTextMty, new ParameterLine(emptyP, mFonts, mTextures, mWindow)));

    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Collection, nullptr, "[Buttons text]"));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::BtnTextColl, new ParameterLine(parP, mFonts, mTextures, mWindow)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::BtnTextMty, new ParameterLine(emptyP, mFonts, mTextures, mWindow)));

    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Collection, nullptr, "[Button graphics]"));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::BtnGfxColl, new ParameterLine(parP, mFonts, mTextures, mWindow)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::BtnGfxMty, new ParameterLine(emptyP, mFonts, mTextures, mWindow)));

    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Collection, nullptr, "[Animation graphics]"));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::AnimGfxColl, new ParameterLine(parP, mFonts, mTextures, mWindow)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::AnimGfxMty, new ParameterLine(emptyP, mFonts, mTextures, mWindow)));

    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Collection, nullptr, "[Background]"));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::BgColl, new ParameterLine(parP, mFonts, mTextures, mWindow)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::BgMty, new ParameterLine(emptyP, mFonts, mTextures, mWindow)));

    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Collection, nullptr, "[Main window]"));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::MainWndwColl, new ParameterLine(parP, mFonts, mTextures, mWindow)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::MainWndwMty, new ParameterLine(emptyP, mFonts, mTextures, mWindow)));

    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Collection, nullptr, "[Extra KPS window]"));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::KPSWndwColl, new ParameterLine(parP, mFonts, mTextures, mWindow)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::KPSWndwMty, new ParameterLine(emptyP, mFonts, mTextures, mWindow)));

    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Collection, nullptr, "[Other]"));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::OtherColl, new ParameterLine(parP, mFonts, mTextures, mWindow)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::OtherMty, new ParameterLine(emptyP, mFonts, mTextures, mWindow)));

    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Collection, nullptr, "[Theme developer]"));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::ThemeDevColl, new ParameterLine(parP, mFonts, mTextures, mWindow)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::ThemeDevMty, new ParameterLine(emptyP, mFonts, mTextures, mWindow)));


    unsigned info = static_cast<unsigned>(ParameterLine::ID::Info1);
    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Collection, nullptr, "                            Quick Guide"));
    mParameterLines.emplace(std::make_pair(static_cast<ParameterLine::ID>(info++), new ParameterLine(parP, mFonts, mTextures, mWindow)));

    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Collection, nullptr, "Click with right mouse button on the key to edit it, then a menu"));
    mParameterLines.emplace(std::make_pair(static_cast<ParameterLine::ID>(info++), new ParameterLine(parP, mFonts, mTextures, mWindow)));

    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Collection, nullptr, "will pop out. It has 2 fields. First one means the logical key,"));
    mParameterLines.emplace(std::make_pair(static_cast<ParameterLine::ID>(info++), new ParameterLine(parP, mFonts, mTextures, mWindow)));

    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Collection, nullptr, "the second one is the visual one - how the key will be displayed."));
    mParameterLines.emplace(std::make_pair(static_cast<ParameterLine::ID>(info++), new ParameterLine(parP, mFonts, mTextures, mWindow)));

    mParameterLines.emplace(std::make_pair(ParameterLine::ID::InfoMty, new ParameterLine(emptyP, mFonts, mTextures, mWindow)));

    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Collection, nullptr, "[Hotkeys]"));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::HotkeyColl, new ParameterLine(parP, mFonts, mTextures, mWindow)));

    unsigned hotKey = static_cast<unsigned>(ParameterLine::ID::HotKey1);
    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Collection, nullptr, "Ctrl + \"+/-\" - Add/remove keyboard keys"));
    mParameterLines.emplace(std::make_pair(static_cast<ParameterLine::ID>(hotKey++), new ParameterLine(parP, mFonts, mTextures, mWindow)));

    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Collection, nullptr, "Ctrl + \"</>\" - Add/remove mouse buttons"));
    mParameterLines.emplace(std::make_pair(static_cast<ParameterLine::ID>(hotKey++), new ParameterLine(parP, mFonts, mTextures, mWindow)));

    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Collection, nullptr, "Ctrl + W - Close the program"));
    mParameterLines.emplace(std::make_pair(static_cast<ParameterLine::ID>(hotKey++), new ParameterLine(parP, mFonts, mTextures, mWindow)));

    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Collection, nullptr, "Ctrl + X - Reset all the statistics"));
    mParameterLines.emplace(std::make_pair(static_cast<ParameterLine::ID>(hotKey++), new ParameterLine(parP, mFonts, mTextures, mWindow)));

    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Collection, nullptr, "Ctrl + A - Open graphical menu"));
    mParameterLines.emplace(std::make_pair(static_cast<ParameterLine::ID>(hotKey++), new ParameterLine(parP, mFonts, mTextures, mWindow)));

    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Collection, nullptr, "Ctrl + K - Open KPS extra window"));
    mParameterLines.emplace(std::make_pair(static_cast<ParameterLine::ID>(hotKey++), new ParameterLine(parP, mFonts, mTextures, mWindow)));

    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Collection, nullptr, "Alt (hold) - Show opposite buttons values"));
    mParameterLines.emplace(std::make_pair(static_cast<ParameterLine::ID>(hotKey++), new ParameterLine(parP, mFonts, mTextures, mWindow)));

    mParameterLines.emplace(std::make_pair(static_cast<ParameterLine::ID>(hotKey++), new ParameterLine(emptyP, mFonts, mTextures, mWindow)));

    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Collection, nullptr, "Program version " + mProgramVersion));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::LastLine, new ParameterLine(parP, mFonts, mTextures, mWindow)));

    float distance = 50;
    unsigned i = 0, halfWindowSize = 300;
    for (auto &pair : mParameterLines)
    {
        pair.second->setPosition(0, distance * i - halfWindowSize);
        ++i;
    }

    mLowViewBounds = distance * mParameterLines.size() - halfWindowSize * 2;
}

// The distance parameter contains offset of the view, 
// but the sliderbar is relative to the window,
// so it's neccessary to normilize both offset and cursor Y position,
// and then project the sum on the window
void Menu::moveSliderBarButtons(float offset)
{
    const sf::Vector2f sliderbarSize = mSliderBar.getSize();
    const sf::Vector2f sliberbarPosition = mSliderBar.getPosition();
    const sf::Vector2u windowSize = mWindow.getSize();
    const float highBounds = sliderbarSize.y / 2;
    const float lowBounds = mWindow.getSize().y - sliderbarSize.y / 2;

    const float normilizedOffset = offset / mLowViewBounds / 1.5f;
    const float normilizedCursorPosition = sliberbarPosition.y / windowSize.y;
    float projectedSliderbarPositionY = windowSize.y * (normilizedCursorPosition + normilizedOffset);

    if (projectedSliderbarPositionY < highBounds)
        projectedSliderbarPositionY = highBounds;
    if (projectedSliderbarPositionY > lowBounds)
        projectedSliderbarPositionY = lowBounds;

    const float sliderbarX = sliberbarPosition.x;
    const float sliderbarY = projectedSliderbarPositionY;

    mSliderBar.setPosition(sliderbarX, sliderbarY);
    mView.setCenter(mView.getCenter().x, mView.getCenter().y + offset);
}

// Real window height - 600, sliderbar height - 200
// Virtual window should be 400, since the whole sliderbar must be in the window
// Quick scatch - https://i.imgur.com/xBXP6II.png
// Red - sliderbar, dark green - virtual window height, lime - real window 
// The sliderbar height has to be normilized and projected on the real window
void Menu::moveSliderBarMouse(sf::Vector2i mousePos)
{
    const sf::Vector2f sliderbarSize = mSliderBar.getSize();
    const float highBounds = sliderbarSize.y / 2;
    const float lowBounds = mWindow.getSize().y - sliderbarSize.y / 2;
    if (mousePos.y < highBounds)
        mousePos.y = highBounds;
    if (mousePos.y > lowBounds)
        mousePos.y = lowBounds;

    const float sliderbarX = mSliderBar.getPosition().x;
    const float sliderbarY = mousePos.y;
    const float virtualWindowHeight = mWindow.getSize().y - sliderbarSize.y;
    const float virtualCursorPositionY = sliderbarY - sliderbarSize.y / 2.f;
    const float normilizedViewHeight = virtualCursorPositionY / virtualWindowHeight;

    mSliderBar.setPosition(sliderbarX, sliderbarY);
    mView.setCenter(mView.getCenter().x, mLowViewBounds * normilizedViewHeight);
}

void Menu::returnViewInBounds()
{
    if (mView.getCenter().y < mHighViewBounds)
        mView.setCenter(mView.getCenter().x, mHighViewBounds);
    if (mView.getCenter().y > mLowViewBounds)
        mView.setCenter(mView.getCenter().x, mLowViewBounds);
}

void Menu::saveConfig()
{
    ConfigHelper::saveConfig(mParameters, mParameterLines);
}

void Menu::requestFocus()
{
    mWindow.requestFocus();
}