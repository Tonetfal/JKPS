#include "../Headers/Menu.hpp"
#include "../Headers/ConfigHelper.hpp"
#include "../Headers/Default media/Textures/vMark.hpp"
#include "../Headers/Default media/Textures/xMark.hpp"
#include "../Headers/Default media/Textures/RGB-Circle.hpp"
#include "../Headers/Default media/Fonts/arial_monospaced.hpp"
#include "../Headers/Default media/Fonts/RobotoMono.hpp"

#include <SFML/Window/Event.hpp>
#include <SFML/System/Clock.hpp>

#include <exception>

std::string Menu::mProgramVersion("v0.8-alpha");


Menu::Menu()
: mScrollSpeed(40.f)
{
    mWindow.setKeyRepeatEnabled(false);

    loadFonts();
    loadTextures();
    buildParametersMap();
    ConfigHelper::readConfig(mParameters, mParameterLines);
    buildParameterLines();
    saveConfig();

    mSettings.buildKeySelector();
}

void Menu::handleEvent(sf::Event event)
{
    if (event.type == sf::Event::KeyPressed
    &&  event.key.code == Settings::KeyToOpenMenuWindow
    &&  sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
    {
        if (!mWindow.isOpen())
        {
            mWindow.create(sf::VideoMode(800, 600), "Menu", sf::Style::Close);
            mView = mWindow.getView();
            mHighViewBounds = mWindow.getSize().y / 2;
        }
        else
            mWindow.close();
    }
}

void Menu::handleOwnEvent()
{
    sf::Event event;
    while (mWindow.pollEvent(event))
    { 
        for (auto &pair : mParameterLines)
            pair.second->handleEvent(event);

        if (event.type == sf::Event::MouseWheelScrolled)
        {
            mView.setCenter(mView.getCenter() - sf::Vector2f(0, 
                mScrollSpeed * event.mouseWheelScroll.delta));
            returnViewInBounds();
        }

        if (event.type == sf::Event::Closed
        || (event.type == sf::Event::KeyPressed
        && event.key.code == Settings::KeyExit
        && sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)))
        {
            mWindow.close();
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
}

void Menu::buildParametersMap()
{
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextDist, new LogicalParameter(LogicalParameter::Type::Float, &Settings::StatisticsDistance, "Statistics text distance", "5", -500, 500)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::SpaceBtwBtnAndStat, new LogicalParameter(LogicalParameter::Type::Float, &Settings::SpaceBetweenButtonsAndStatistics, "Space between buttons and statistics", "15", -500, 500)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextFont, new LogicalParameter(LogicalParameter::Type::String, &Settings::StatisticsFontPath, "Statistics text font", "Default")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextClr, new LogicalParameter(LogicalParameter::Type::Color, &Settings::StatisticsTextColor, "Statistics text color", "255,255,255,255")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextChSz, new LogicalParameter(LogicalParameter::Type::Unsigned, &Settings::StatisticsTextCharacterSize, "Statistics text character size", "14", 0, 500)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextBold, new LogicalParameter(LogicalParameter::Type::Bool, &Settings::StatisticsBold, "Statistics text bold", "False")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextItal, new LogicalParameter(LogicalParameter::Type::Bool, &Settings::StatisticsItalic, "Statistics text italic", "False")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextShow, new LogicalParameter(LogicalParameter::Type::Bool, &Settings::ShowStatisticsText, "Show statistics text", "True")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextShowKPS, new LogicalParameter(LogicalParameter::Type::Bool, &Settings::ShowKPS, "Show KPS", "True")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextShowMaxKPS, new LogicalParameter(LogicalParameter::Type::Bool, &Settings::ShowMaxKPS, "Show max KPS on separate line", "True")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextShowTotal, new LogicalParameter(LogicalParameter::Type::Bool, &Settings::ShowTotal, "Show total", "True")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextShowBPM, new LogicalParameter(LogicalParameter::Type::Bool, &Settings::ShowBPMText, "Show BPM", "True")));

    mParameters.emplace(std::make_pair(LogicalParameter::ID::BtnTextFont, new LogicalParameter(LogicalParameter::Type::String, &Settings::KeyCountersFontPath, "Buttons text font", "Default")));
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
    mParameters.emplace(std::make_pair(LogicalParameter::ID::BtnGfxTxtr, new LogicalParameter(LogicalParameter::Type::String, &Settings::ButtonTexturePath, "Button texture", "Default")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::BtnGfxTxtrSz, new LogicalParameter(LogicalParameter::Type::VectorU, &Settings::ButtonTextureSize, "Button texture size", "60,60", 0, 500)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::BtnGfxTxtrClr, new LogicalParameter(LogicalParameter::Type::Color, &Settings::ButtonTextureColor, "Button texture color", "30,30,30,255")));

    mParameters.emplace(std::make_pair(LogicalParameter::ID::AnimGfxStl, new LogicalParameter(LogicalParameter::Type::Unsigned, &Settings::AnimationStyle, "Animation style", "1", 1, 2)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::AnimGfxTxtr, new LogicalParameter(LogicalParameter::Type::String, &Settings::AnimationTexturePath, "Animation texture", "Default")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::AnimGfxVel, new LogicalParameter(LogicalParameter::Type::Unsigned, &Settings::AnimationVelocity, "Animation velocity", "5", 1, 120)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::AnimGfxScl, new LogicalParameter(LogicalParameter::Type::VectorF, &Settings::AnimationScale, "Animation scale on click (%)", "100,100", 0, 200)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::AnimGfxClr, new LogicalParameter(LogicalParameter::Type::Color, &Settings::AnimationColor, "Animation color", "255,180,0,255")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::AnimGfxOffset, new LogicalParameter(LogicalParameter::Type::Float, &Settings::AnimationOffset, "Animation offset", "3", -100, 100)));

    mParameters.emplace(std::make_pair(LogicalParameter::ID::BgTxtr, new LogicalParameter(LogicalParameter::Type::String, &Settings::BackgroundTexturePath, "Background texture", "Default")));
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
    mParameters.emplace(std::make_pair(LogicalParameter::ID::KPSWndwTxtFont, new LogicalParameter(LogicalParameter::Type::String, &Settings::KPSWindowTextFontPath, "KPS window text font", "Default")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::KPSWndwNumFont, new LogicalParameter(LogicalParameter::Type::String, &Settings::KPSWindowNumberFontPath, "KPS window number font", "Default", -50)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::KPSWndwTopPadding, new LogicalParameter(LogicalParameter::Type::Float, &Settings::KPSWindowTopPadding, "KPS top padding", "20", -500, 500)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::KPSWndwDistBtw, new LogicalParameter(LogicalParameter::Type::Float, &Settings::KPSWindowDistanceBetween, "KPS extra window distance between text", "50", -500, 500)));

    mParameters.emplace(std::make_pair(LogicalParameter::ID::OtherHighText, new LogicalParameter(LogicalParameter::Type::Color, &Settings::HighlightedKeyColor, "Highlighted text button color", "210,30,210,255")));

    mParameters.emplace(std::make_pair(LogicalParameter::ID::ThemeDevMultpl, new LogicalParameter(LogicalParameter::Type::Unsigned, &Settings::ValueToMultiplyOnClick, "Value to multiply on click", "1", 0, 1000000)));
}

void Menu::buildParameterLines()
{
    unsigned additionalLine = 0;
    for (auto &pair : mParameters)
    {
        if (pair.second->mType == LogicalParameter::Type::String)
        {
            ++additionalLine;
            continue;
        }

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

    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Collection, nullptr, "[KPS window]"));
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
    unsigned i = 0;
    for (auto &pair : mParameterLines)
    {
        pair.second->setPosition(0, distance * i);
        ++i;
    }

    mLowViewBounds = distance * mParameterLines.size() - mHighViewBounds - distance * (additionalLine - 1);
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