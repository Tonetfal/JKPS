#include "../Headers/Menu.hpp"
#include "../Headers/ConfigHelper.hpp"
#include "../Headers/Button.hpp"
#include "../Headers/Default media/Textures/vMark.hpp"
#include "../Headers/Default media/Textures/xMark.hpp"
#include "../Headers/Default media/Textures/RGB-Circle.hpp"
#include "../Headers/Default media/Textures/RefreshTexture.hpp"
#include "../Headers/Default media/Fonts/arial_monospaced.hpp"
#include "../Headers/Default media/Fonts/RobotoMono.hpp"

#include <SFML/Window/Event.hpp>
#include <SFML/System/Clock.hpp>

#include <limits.h>


std::string Menu::mProgramVersion("v0.1-hotfix");

Menu::Menu()
: mScrollSpeed(40.f)
, mSliderBar(sf::Vector2f(10.f, 200.f))
, mSliderBarDefaultColor(sf::Color(103,103,103))
, mSliderBarAimedColor(sf::Color(123,123,123))
, mSliderBarPressedColor(sf::Color(161,161,161))
, mSelectedTab(0u)
{
    loadFonts();
    loadTextures();

    initCollectionNames();
    buildParametersMap();
    auto read = ConfigHelper::readConfig(mParameters, mCollectionNames);
    buildParameterLines();
    buildMenuTabs();
    if (!read)
        ConfigHelper::saveConfig(mParameters, mParameterLines, nullptr, false);

    mSliderBar.setOrigin(mSliderBar.getSize() / 2.f);
    mSliderBar.setPosition(949.f, 100.f);
    mSliderBar.setFillColor(mSliderBarDefaultColor);

    mTabs.at(mSelectedTab)->mRect.setFillColor(GfxParameter::defaultSelectedRectColor);
}

void Menu::processInput()
{
    handleEvent();
    handleRealtimeInput();
}

void Menu::handleEvent()
{
    sf::Event event;
    static bool scrollCursorClicked;
    while (mWindow.pollEvent(event))
    {
        if (event.type == sf::Event::MouseButtonPressed 
        ||  event.type == sf::Event::KeyPressed
        ||  event.type == sf::Event::MouseWheelScrolled)
        {
            if (event.type == sf::Event::KeyPressed 
            &&  event.key.code == sf::Keyboard::Escape)
            {
                ParameterLine::deselectValue();
                continue;
            }

            bool isAnyLineSelected = false;
            for (auto &pair : mParameterLines)
            {
                bool isSelected = pair.second->handleEvent(event);
                if (isSelected)
                    isAnyLineSelected = true;
            }

            if (!isAnyLineSelected)
            {
                // If a key was pressed, but mouse aims at no box - deselect
                if (event.type != sf::Event::MouseWheelScrolled)
                    ParameterLine::deselectValue();

                // If the mouse wheel was scrolled and no value is selected - scroll
                if (event.type == sf::Event::MouseWheelScrolled || event.type == sf::Event::KeyPressed)
                {
                    float offset = 0.f;

                    if (event.type == sf::Event::MouseWheelScrolled)
                    {
                        offset = -mScrollSpeed * event.mouseWheelScroll.delta;
                    }
                    if (event.type == sf::Event::KeyPressed)
                    {
                        const sf::Keyboard::Key key = event.key.code;
                        const sf::Vector2f viewSize = mView.getSize();
                        if (key == sf::Keyboard::PageUp)
                            offset = -viewSize.y;
                        if (key == sf::Keyboard::PageDown)
                            offset = viewSize.y;
                    }

                    moveSliderBarButtons(offset);
                    returnViewInBounds();
                }
            }
        }

        if (event.type == sf::Event::MouseButtonPressed 
        ||  event.type == sf::Event::MouseButtonReleased
        ||  event.type == sf::Event::MouseMoved)
        {
            auto mousePos = sf::Mouse::getPosition(mWindow);
            auto colorToSet = mSliderBarDefaultColor;
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

        if (event.type == sf::Event::MouseButtonPressed 
        ||  event.type == sf::Event::MouseButtonReleased
        ||  event.type == sf::Event::MouseMoved)
        {
            const auto halfWindowSize = static_cast<sf::Vector2f>(mWindow.getSize()) / 2.f;
            const auto mousePos = static_cast<sf::Vector2f>(sf::Mouse::getPosition(mWindow));

            unsigned idx = 0u;
            for (auto &tab : mTabs)
            {
                auto color = GfxParameter::defaultRectColor;
                if (tab->contains(mousePos))
                {
                    color = GfxParameter::defaultAimedRectColor;
                    if (event.type == sf::Event::MouseButtonPressed)
                    {

                        // Reset slider
                        mSliderBar.setPosition(mSliderBar.getPosition().x, 100);

                        // Deselect old tab
                        mTabs[mSelectedTab]->mRect.setFillColor(GfxParameter::defaultRectColor);

                        // Select new tab
                        mSelectedTab = idx;
                        selectTab(idx);
                    }
                }

                if (mSelectedTab == idx)
                    color = GfxParameter::defaultSelectedRectColor;

                tab->mRect.setFillColor(color);
                ++idx;
            }
        }

        if (event.type == sf::Event::Closed
        || (event.type == sf::Event::KeyPressed
        && event.key.code == Settings::KeyExit
        && sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)))
        {
            closeWindow();
            return;
        }
    }
}

void Menu::handleRealtimeInput()
{
    if (mWindow.hasFocus() && !ParameterLine::isValueSelected())
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

    for (auto &elem : mParameters)
    {
        if (elem.second->resetChangeState())
            mChangedParametersQueue.push(elem);
    }
}

void Menu::update()
{

}

void Menu::render() 
{
    mWindow.setView(mView);

    mWindow.clear(sf::Color(45,45,45));

    for (const auto &pair : mParameterLines)
        mWindow.draw(*pair.second);

    auto transform = sf::Transform::Identity;
    mWindow.setView(mWindow.getDefaultView());

    mWindow.draw(mTabsBackground);
    for (const auto &elem : mTabs)
        mWindow.draw(*elem, transform);
    mWindow.draw(mSliderBar);

    mWindow.setView(mView);

    mWindow.display();
}

void Menu::openWindow()
{
    sf::Uint32 style;
#ifdef _WIN32
    style = sf::Style::Close;
#elif linux
    style = sf::Style::Default;
#else
#error Unsupported platform
#endif

    mWindow.create(sf::VideoMode(959, 700), "JKPS Menu", style);
    mView = mWindow.getView();
    selectTab(mSelectedTab);
    mSliderBar.setPosition(mSliderBar.getPosition().x, mSliderBar.getSize().y / 2);
}

void Menu::closeWindow()
{
    mWindow.close();
    ParameterLine::deselectValue();
}

bool Menu::isOpen() const
{
    return mWindow.isOpen();
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

void Menu::selectTab(unsigned idx)
{
    mView.setCenter(1000.f * idx + mWindow.getSize().x / 2.f, 0.f);
    mWindow.setView(mView);
}

void Menu::initCollectionNames()
{
    mCollectionNames.emplace_back("[Statistics text]");
    mCollectionNames.emplace_back("[Statistics text advanced settings]");
    mCollectionNames.emplace_back("[Buttons text]");
    mCollectionNames.emplace_back("[Buttons text advanced settings]");
    mCollectionNames.emplace_back("[Button graphics]");
    mCollectionNames.emplace_back("[Button graphics advanced settings]");
    mCollectionNames.emplace_back("[Common parameters]");
    mCollectionNames.emplace_back("[Light animation]");
    mCollectionNames.emplace_back("[Press animation]");
    mCollectionNames.emplace_back("[Main window]");
    mCollectionNames.emplace_back("[Extra KPS window]");
    mCollectionNames.emplace_back("[Key press visualization]");
    mCollectionNames.emplace_back("[Other]");
    mCollectionNames.emplace_back("[Statistics save]");
}

void Menu::buildMenuTabs()
{
    // Font must be loaded before tabs can be constructed

    std::unique_ptr<GfxParameter> tabPtr = nullptr;
    const sf::Vector2f offset(5.f, 5.f);
    const sf::Vector2f distBtw(0.f, 5.f);
    const sf::Vector2f tabSize(140.f, 25.f);
    const sf::Vector2f highTabSize(140.f, 50.f);

    auto addTab = [&] (const auto &str, sf::Vector2f tabSize)
        {
            auto ptr = std::make_unique<GfxParameter>(str, tabSize);
            sf::Vector2f prevTabPos = offset + tabSize / 2.f;
            sf::Vector2f prevTabSize = -tabSize;
            if (!mTabs.empty())
            {
                prevTabPos = mTabs.back()->getPosition();
                prevTabSize = mTabs.back()->mRect.getSize();
            }

            ptr->setPosition(prevTabPos + sf::Vector2f(0.f, tabSize.y / 2.f + prevTabSize.y / 2.f) + distBtw);
            mTabs.push_back(std::move(ptr));
        };

    addTab("Statistics text", tabSize);
    addTab("Buttons text", tabSize);
    addTab(" Button\ngraphics", highTabSize);
    addTab("Animations", tabSize);
    addTab("Main window", tabSize);
    addTab("Extra KPS\n  window", highTabSize);
    addTab("  Key press\nvisualization", highTabSize);
    addTab("Other", tabSize);
    addTab("Main info", tabSize);

    mTabsBackground.setSize(sf::Vector2f(tabSize.x + offset.x * 2.f, 700.f));
    mTabsBackground.setFillColor(sf::Color(35, 35, 35));
}

void Menu::buildParametersMap()
{
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextDist,              new LogicalParameter(LogicalParameter::Type::Float,         &Settings::StatisticsTextDistance,                      "Distance between", "5", -500, 500)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatPos,                   new LogicalParameter(LogicalParameter::Type::VectorF,       &Settings::StatisticsTextPosition,                      "Position offset", "5, 0", -1000, 1000)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatValPos,                new LogicalParameter(LogicalParameter::Type::VectorF,       &Settings::StatisticsTextValuePosition,                 "Number position offset", "0, 0", -1000, 1000)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextFont,              new LogicalParameter(LogicalParameter::Type::StringPath,    &Settings::StatisticsTextFontPath,                      "Font filepath", "Default")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextClr,               new LogicalParameter(LogicalParameter::Type::Color,         &Settings::StatisticsTextColor,                         "Text color", "255,255,255,255")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextChSz,              new LogicalParameter(LogicalParameter::Type::Unsigned,      &Settings::StatisticsTextCharacterSize,                 "Character size", "15", 0, 500)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextOutThck,           new LogicalParameter(LogicalParameter::Type::Unsigned,      &Settings::StatisticsTextOutlineThickness,              "Outline thickness", "0", 0, 500)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextOutClr,            new LogicalParameter(LogicalParameter::Type::Color,         &Settings::StatisticsTextOutlineColor,                  "Outline color", "255,255,255,255")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextBold,              new LogicalParameter(LogicalParameter::Type::Bool,          &Settings::StatisticsTextBold,                          "Bold", "False")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextItal,              new LogicalParameter(LogicalParameter::Type::Bool,          &Settings::StatisticsTextItalic,                        "Italic", "False")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextShow,              new LogicalParameter(LogicalParameter::Type::Bool,          &Settings::ShowStatisticsText,                          "Enabled", "True")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextShowKPS,           new LogicalParameter(LogicalParameter::Type::Bool,          &Settings::ShowStatisticsKPS,                           "Show KPS", "True")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextShowTotal,         new LogicalParameter(LogicalParameter::Type::Bool,          &Settings::ShowStatisticsTotal,                         "Show total", "True")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextShowBPM,           new LogicalParameter(LogicalParameter::Type::Bool,          &Settings::ShowStatisticsBPM,                           "Show BPM", "True")));

    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextPosAdvMode,        new LogicalParameter(LogicalParameter::Type::Bool,          &Settings::StatisticsTextPositionsAdvancedMode,         "Enable advanced mode for text positions", "False")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextPos1,              new LogicalParameter(LogicalParameter::Type::VectorF,       &Settings::StatisticsTextPositions[0],                  "KPS text position offset", "0,0", -1000, 1000)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextPos2,              new LogicalParameter(LogicalParameter::Type::VectorF,       &Settings::StatisticsTextPositions[1],                  "Total text position offset", "0,0", -1000, 1000)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextPos3,              new LogicalParameter(LogicalParameter::Type::VectorF,       &Settings::StatisticsTextPositions[2],                  "BPM text position offset", "0,0", -1000, 1000)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextValPosAdvMode,     new LogicalParameter(LogicalParameter::Type::Bool,          &Settings::StatisticsTextValuePositionsAdvancedMode,    "Enable advanced mode for number text positions", "False")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextValPos1,           new LogicalParameter(LogicalParameter::Type::VectorF,       &Settings::StatisticsTextValuePositions[0],             "KPS number text position offset", "0,0", -1000, 1000)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextValPos2,           new LogicalParameter(LogicalParameter::Type::VectorF,       &Settings::StatisticsTextValuePositions[1],             "Total number text position offset", "0,0", -1000, 1000)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextValPos3,           new LogicalParameter(LogicalParameter::Type::VectorF,       &Settings::StatisticsTextValuePositions[2],             "BPM number text position offset", "0,0", -1000, 1000)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextClrAdvMode,        new LogicalParameter(LogicalParameter::Type::Bool,          &Settings::StatisticsTextColorsAdvancedMode,            "Enable advanced mode for text colors", "False")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextClr1,              new LogicalParameter(LogicalParameter::Type::Color,         &Settings::StatisticsTextColors[0],                     "KPS text color", "255,255,255,255")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextClr2,              new LogicalParameter(LogicalParameter::Type::Color,         &Settings::StatisticsTextColors[1],                     "Total text color", "255,255,255,255")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextClr3,              new LogicalParameter(LogicalParameter::Type::Color,         &Settings::StatisticsTextColors[2],                     "BPM text color", "255,255,255,255")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextChSzAdvMode,       new LogicalParameter(LogicalParameter::Type::Bool,          &Settings::StatisticsTextChSzssAdvancedMode,            "Enable advanced mode for character sizes", "False")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextChSz1,             new LogicalParameter(LogicalParameter::Type::Unsigned,      &Settings::StatisticsTextCharacterSizes[0],             "KPS text character size", "14", 0, 500)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextChSz2,             new LogicalParameter(LogicalParameter::Type::Unsigned,      &Settings::StatisticsTextCharacterSizes[1],             "Total text character size", "14", 0, 500)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextChSz3,             new LogicalParameter(LogicalParameter::Type::Unsigned,      &Settings::StatisticsTextCharacterSizes[2],             "BPM text character size", "14", 0, 500)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextBoldAdvMode,       new LogicalParameter(LogicalParameter::Type::Bool,          &Settings::StatisticsTextBoldAdvancedMode,              "Enable advanced mode for stats text bold", "False")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextBold1,             new LogicalParameter(LogicalParameter::Type::Bool,          &Settings::StatisticsTextBolds[0],                      "KPS bold", "False")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextBold2,             new LogicalParameter(LogicalParameter::Type::Bool,          &Settings::StatisticsTextBolds[1],                      "Total bold", "False")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextBold3,             new LogicalParameter(LogicalParameter::Type::Bool,          &Settings::StatisticsTextBolds[2],                      "BPM bold", "False")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextItalAdvMode,       new LogicalParameter(LogicalParameter::Type::Bool,          &Settings::StatisticsTextItalicAdvancedMode,            "Enable advanced mode for stats text italic", "False")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextItal1,             new LogicalParameter(LogicalParameter::Type::Bool,          &Settings::StatisticsTextItalics[0],                    "KPS italic", "False")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextItal2,             new LogicalParameter(LogicalParameter::Type::Bool,          &Settings::StatisticsTextItalics[1],                    "Total italic", "False")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextItal3,             new LogicalParameter(LogicalParameter::Type::Bool,          &Settings::StatisticsTextItalics[2],                    "BPM italic", "False")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextKPSText,           new LogicalParameter(LogicalParameter::Type::String,        &Settings::StatisticsKPSText,                           "KPS text", "KPS: ")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextKPS2Text,          new LogicalParameter(LogicalParameter::Type::String,        &Settings::StatisticsKPS2Text,                          "KPS text when it is 0", "Max: ")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextTotalText,         new LogicalParameter(LogicalParameter::Type::String,        &Settings::StatisticsTotalText,                         "Total text", "Total: ")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextBPMText,           new LogicalParameter(LogicalParameter::Type::String,        &Settings::StatisticsBPMText,                           "BPM text", "BPM: ")));

    mParameters.emplace(std::make_pair(LogicalParameter::ID::BtnTextFont,               new LogicalParameter(LogicalParameter::Type::StringPath,    &Settings::ButtonTextFontPath,                          "Font filepath", "Default")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::BtnTextClr,                new LogicalParameter(LogicalParameter::Type::Color,         &Settings::ButtonTextColor,                             "Text color", "255,255,255,255")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::BtnTextChSz,               new LogicalParameter(LogicalParameter::Type::Unsigned,      &Settings::ButtonTextCharacterSize,                     "Character size", "20", 0, 500)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::BtnTextOutThck,            new LogicalParameter(LogicalParameter::Type::Unsigned,      &Settings::ButtonTextOutlineThickness,                  "Outline thickness", "0", 0, 500)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::BtnTextOutClr,             new LogicalParameter(LogicalParameter::Type::Color,         &Settings::ButtonTextOutlineColor,                      "Outline color", "255,255,255,255")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::BtnTextPosition,           new LogicalParameter(LogicalParameter::Type::VectorF,       &Settings::ButtonTextPosition,                          "Position offset", "0,0", -500, 500)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::BtnTextBounds,             new LogicalParameter(LogicalParameter::Type::VectorF,       &Settings::ButtonTextBounds,                            "Text bounds", "8,8", -500, 500)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::BtnTextIgnoreBtnMovement,  new LogicalParameter(LogicalParameter::Type::Bool,          &Settings::BtnTextIgnoreBtnMovement,                    "Ignore button movement", "False")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::BtnTextBold,               new LogicalParameter(LogicalParameter::Type::Bool,          &Settings::ButtonTextBold,                              "Bold", "False")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::BtnTextItal,               new LogicalParameter(LogicalParameter::Type::Bool,          &Settings::ButtonTextItalic,                            "Italic", "False")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::BtnTextShowVisKeys,        new LogicalParameter(LogicalParameter::Type::Bool,          &Settings::ButtonTextShowVisualKeys,                    "Show key labels", "True")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::BtnTextShowTot,            new LogicalParameter(LogicalParameter::Type::Bool,          &Settings::ButtonTextShowTotal,                         "Show key counters", "False")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::BtnTextShowKps,            new LogicalParameter(LogicalParameter::Type::Bool,          &Settings::ButtonTextShowKPS,                           "Show key KPS", "False")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::BtnTextShowBpm,            new LogicalParameter(LogicalParameter::Type::Bool,          &Settings::ButtonTextShowBPM,                           "Show key BPM", "False")));

    mParameters.emplace(std::make_pair(LogicalParameter::ID::BtnTextSepPosAdvMode,      new LogicalParameter(LogicalParameter::Type::Bool,          &Settings::ButtonTextSepPosAdvancedMode,                "Enable advanced mode for separate button text positions", "False")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::BtnTextVisPosition,        new LogicalParameter(LogicalParameter::Type::VectorF,       &Settings::ButtonVisualKeysTextPosition,                "Visual keys text position offset", "0,0", -1000, 1000)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::BtnTextTotPosition,        new LogicalParameter(LogicalParameter::Type::VectorF,       &Settings::ButtonTotalTextPosition,                     "Key counters text position offset", "0,0", -1000, 1000)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::BtnTextKPSPosition,        new LogicalParameter(LogicalParameter::Type::VectorF,       &Settings::ButtonKPSTextPosition,                       "KPS text position offset", "0,0", -1000, 1000)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::BtnTextBPMPosition,        new LogicalParameter(LogicalParameter::Type::VectorF,       &Settings::ButtonBPMTextPosition,                       "BPM text position offset", "0,0", -1000, 1000)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::BtnTextPosAdvMode,         new LogicalParameter(LogicalParameter::Type::Bool,          &Settings::ButtonTextPosAdvancedMode,                   "Enable advanced mode for button text positions", "False")));
    for (unsigned idx = 0; idx < 15; ++idx)
    {
        auto textPosId =               LogicalParameter::ID(unsigned(LogicalParameter::ID::BtnTextPos1) + idx);
        mParameters.emplace(std::make_pair(textPosId,                                   new LogicalParameter(LogicalParameter::Type::VectorF,       &Settings::ButtonsTextPositions[idx],                  ("Button " + std::to_string(idx + 1) + " text position offset"), "0,0", -1000, 1000)));
    }

    mParameters.emplace(std::make_pair(LogicalParameter::ID::BtnGfxDist,                new LogicalParameter(LogicalParameter::Type::Float,         &Settings::GfxButtonDistance,                           "Distance", "4", -500, 500)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::BtnGfxTxtr,                new LogicalParameter(LogicalParameter::Type::StringPath,    &Settings::GfxButtonTexturePath,                        "Texture filepath", "Default")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::BtnGfxTxtrSz,              new LogicalParameter(LogicalParameter::Type::VectorU,       &Settings::GfxButtonTextureSize,                        "Texture size", "50,50", 0, 500)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::BtnGfxTxtrClr,             new LogicalParameter(LogicalParameter::Type::Color,         &Settings::GfxButtonTextureColor,                       "Texture color", "30,30,30,255")));

    mParameters.emplace(std::make_pair(LogicalParameter::ID::BtnGfxBtnPosAdvMode,       new LogicalParameter(LogicalParameter::Type::Bool,          &Settings::GfxButtonBtnPositionsAdvancedMode,           "Enable advanced mode for button positions", "False")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::BtnGfxSzAdvMode,           new LogicalParameter(LogicalParameter::Type::Bool,          &Settings::GfxButtonSizesAdvancedMode,                  "Enable advanced mode for button sizes", "False")));
    for (unsigned idx = 0; idx < 15; ++idx)
    {
        auto btnPosId =                LogicalParameter::ID(unsigned(LogicalParameter::ID::BtnGfxBtnPos1) + idx * 2);
        auto btnSzId =                 LogicalParameter::ID(unsigned(LogicalParameter::ID::BtnGfxSz1) + idx * 2);
        mParameters.emplace(std::make_pair(btnPosId,                                    new LogicalParameter(LogicalParameter::Type::VectorF,       &Settings::GfxButtonsBtnPositions[idx],                ("Button " + std::to_string(idx + 1) + " position offset"), "0,0", -1000, 1000)));
        mParameters.emplace(std::make_pair(btnSzId,                                     new LogicalParameter(LogicalParameter::Type::VectorF,       &Settings::GfxButtonsSizes[idx],                       ("Button " + std::to_string(idx + 1) + " size"), "60,60", -1000, 1000)));
    }
    
    mParameters.emplace(std::make_pair(LogicalParameter::ID::AnimGfxVel,                new LogicalParameter(LogicalParameter::Type::Unsigned,      &Settings::AnimationFrames,                             "Animation duration (frames)", "5", 1, 120)));

    mParameters.emplace(std::make_pair(LogicalParameter::ID::AnimGfxLight,              new LogicalParameter(LogicalParameter::Type::Bool,          &Settings::LightAnimation,                              "Enabled", "True")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::AnimGfxTxtr,               new LogicalParameter(LogicalParameter::Type::StringPath,    &Settings::AnimationTexturePath,                        "Texture filepath", "Default")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::AnimGfxScl,                new LogicalParameter(LogicalParameter::Type::VectorF,       &Settings::AnimationScale,                              "Scale on click (%)", "100,100", 0, 200)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::AnimGfxClr,                new LogicalParameter(LogicalParameter::Type::Color,         &Settings::AnimationColor,                              "Color", "255,180,0,255")));

    mParameters.emplace(std::make_pair(LogicalParameter::ID::AnimGfxPress,              new LogicalParameter(LogicalParameter::Type::Bool,          &Settings::PressAnimation,                              "Enabled", "False")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::AnimGfxOffset,             new LogicalParameter(LogicalParameter::Type::Float,         &Settings::AnimationOffset,                             "Offset", "3", -100, 100)));

    mParameters.emplace(std::make_pair(LogicalParameter::ID::BgTxtr,                    new LogicalParameter(LogicalParameter::Type::StringPath,    &Settings::BackgroundTexturePath,                       "Background texture filepath", "Default")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::BgClr,                     new LogicalParameter(LogicalParameter::Type::Color,         &Settings::BackgroundColor,                             "Background color", "140,140,140,255")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::BgScale,                   new LogicalParameter(LogicalParameter::Type::Bool,          &Settings::ScaleBackground,                             "Scale background texture if it does not fit", "True")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::MainWndwTitleBar,          new LogicalParameter(LogicalParameter::Type::Bool,          &Settings::WindowTitleBar,                              "Title bar", "False")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::MainWndwTop,               new LogicalParameter(LogicalParameter::Type::Int,           &Settings::WindowBonusSizeTop,                          "Bonus size top", "6", -1000, 1000)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::MainWndwBot,               new LogicalParameter(LogicalParameter::Type::Int,           &Settings::WindowBonusSizeBottom,                       "Bonus size bottom", "6", -1000, 1000)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::MainWndwLft,               new LogicalParameter(LogicalParameter::Type::Int,           &Settings::WindowBonusSizeLeft,                         "Bonus size left", "6", -1000, 1000)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::MainWndwRght,              new LogicalParameter(LogicalParameter::Type::Int,           &Settings::WindowBonusSizeRight,                        "Bonus size right", "105", -1000, 1000)));

    mParameters.emplace(std::make_pair(LogicalParameter::ID::KPSWndwEn,                 new LogicalParameter(LogicalParameter::Type::Bool,          &Settings::KPSWindowEnabledFromStart,                   "Enable from start", "False")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::KPSWndwSz,                 new LogicalParameter(LogicalParameter::Type::VectorU,       &Settings::KPSWindowSize,                               "Window size", "300,300", 0, 1000)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::KPSWndwTxtChSz,            new LogicalParameter(LogicalParameter::Type::Unsigned,      &Settings::KPSTextSize,                                 "Text character size:", "130", 0, 500)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::KPSWndwNumChSz,            new LogicalParameter(LogicalParameter::Type::Unsigned,      &Settings::KPSNumberSize,                               "Number character size", "100", 0, 500)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::KPSWndwBgClr,              new LogicalParameter(LogicalParameter::Type::Color,         &Settings::KPSBackgroundColor,                          "Background color", "0,177,64,255")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::KPSWndwTxtClr,             new LogicalParameter(LogicalParameter::Type::Color,         &Settings::KPSTextColor,                                "Text color", "255,255,255,255")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::KPSWndwNumClr,             new LogicalParameter(LogicalParameter::Type::Color,         &Settings::KPSNumberColor,                              "Number color", "255,255,255,255")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::KPSWndwTxtFont,            new LogicalParameter(LogicalParameter::Type::StringPath,    &Settings::KPSWindowTextFontPath,                       "Text font filepath", "Default")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::KPSWndwNumFont,            new LogicalParameter(LogicalParameter::Type::StringPath,    &Settings::KPSWindowNumberFontPath,                     "Number font filepaht", "Default", -50)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::KPSWndwTopPadding,         new LogicalParameter(LogicalParameter::Type::Float,         &Settings::KPSWindowTopPadding,                         "Top padding", "20", -500, 500)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::KPSWndwDistBtw,            new LogicalParameter(LogicalParameter::Type::Float,         &Settings::KPSWindowDistanceBetween,                    "Distance between text", "50", -500, 500)));

    mParameters.emplace(std::make_pair(LogicalParameter::ID::KeyPressVisToggle,         new LogicalParameter(LogicalParameter::Type::Bool,          &Settings::KeyPressVisToggle,                           "Enabled", "False")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::KeyPressVisSpeed,          new LogicalParameter(LogicalParameter::Type::Float,         &Settings::KeyPressVisSpeed,                            "Speed", "60", -500, 500)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::KeyPressVisRotation,       new LogicalParameter(LogicalParameter::Type::Float,         &Settings::KeyPressVisRotation,                         "Movement rotation", "0", -360, 360)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::KeyPressVisOrig,           new LogicalParameter(LogicalParameter::Type::VectorF,       &Settings::KeyPressVisOrig,                             "Spawn position offset", "0,-5", -1000, 1000)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::KeyPressVisFadeLineLen,    new LogicalParameter(LogicalParameter::Type::Float,         &Settings::KeyPressVisFadeLineLen,                      "Fade out distance", "500", -16384, 16384)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::KeyPressVisColor,          new LogicalParameter(LogicalParameter::Type::Color,         &Settings::KeyPressVisColor,                            "Color", "255,255,255,255")));

    mParameters.emplace(std::make_pair(LogicalParameter::ID::OtherSaveStats,            new LogicalParameter(LogicalParameter::Type::Bool,          &Settings::SaveStats,                                   "Update statistics on quit", "False")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::OtherMultpl,               new LogicalParameter(LogicalParameter::Type::Unsigned,      &Settings::ButtonPressMultiplier,                       "Value to multiply on click", "1", 0, 1000000)));

    mParameters.emplace(std::make_pair(LogicalParameter::ID::SaveStatMaxKPS,            new LogicalParameter(LogicalParameter::Type::Float,         &Settings::MaxKPS,                                      "Saved max KPS", "0", 0u, UINT_MAX)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::SaveStatTotal,             new LogicalParameter(LogicalParameter::Type::Unsigned,      &Settings::Total,                                       "Saved total", "0", 0u, UINT_MAX)));
    for (unsigned idx = 0; idx < Settings::KeysTotal.size(); ++idx)
    {
        auto id =                       LogicalParameter::ID(unsigned(LogicalParameter::ID::SaveStatTotal1) + idx);
        mParameters.emplace(std::make_pair(id,                                          new LogicalParameter(LogicalParameter::Type::Unsigned,      &Settings::KeysTotal[idx],                              "Saved total " + std::to_string(idx + 1), "0", 0, UINT_MAX)));
    }
}

void Menu::buildParameterLines()
{
    for (auto &pair : mParameters)
    {
        mParameterLines.emplace(std::make_pair(ParameterLine::parIdToParLineId(pair.first), 
            new ParameterLine(pair.second, mFonts, mTextures, mWindow)));
    }
    
    using sPtr = std::shared_ptr<LogicalParameter>;
    sPtr parP = nullptr;
    sPtr emptyP(new LogicalParameter(LogicalParameter::Type::Empty, nullptr));
    size_t collectionNameIdx = 0u;

    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Collection, nullptr, mCollectionNames.at(collectionNameIdx++)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::StatTextColl, new ParameterLine(parP, mFonts, mTextures, mWindow)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::StatTextMty, new ParameterLine(emptyP, mFonts, mTextures, mWindow)));

    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Collection, nullptr, mCollectionNames.at(collectionNameIdx++)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::StatTextAdvColl, new ParameterLine(parP, mFonts, mTextures, mWindow)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::StatTextAdvMty, new ParameterLine(emptyP, mFonts, mTextures, mWindow)));

    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Collection, nullptr, mCollectionNames.at(collectionNameIdx++)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::BtnTextColl, new ParameterLine(parP, mFonts, mTextures, mWindow)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::BtnTextMty, new ParameterLine(emptyP, mFonts, mTextures, mWindow)));

    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Collection, nullptr, mCollectionNames.at(collectionNameIdx++)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::BtnTextAdvColl, new ParameterLine(parP, mFonts, mTextures, mWindow)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::BtnTextAdvMty, new ParameterLine(emptyP, mFonts, mTextures, mWindow)));

    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Collection, nullptr, mCollectionNames.at(collectionNameIdx++)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::BtnGfxColl, new ParameterLine(parP, mFonts, mTextures, mWindow)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::BtnGfxMty, new ParameterLine(emptyP, mFonts, mTextures, mWindow)));

    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Collection, nullptr, mCollectionNames.at(collectionNameIdx++)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::BtnGfxAdvColl, new ParameterLine(parP, mFonts, mTextures, mWindow)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::BtnGfxAdvMty, new ParameterLine(emptyP, mFonts, mTextures, mWindow)));

    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Collection, nullptr, mCollectionNames.at(collectionNameIdx++)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::AnimGfxColl, new ParameterLine(parP, mFonts, mTextures, mWindow)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::AnimGfxMty, new ParameterLine(emptyP, mFonts, mTextures, mWindow)));

    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Collection, nullptr, mCollectionNames.at(collectionNameIdx++)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::AnimGfxLightColl, new ParameterLine(parP, mFonts, mTextures, mWindow)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::AnimGfxLightMty, new ParameterLine(emptyP, mFonts, mTextures, mWindow)));

    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Collection, nullptr, mCollectionNames.at(collectionNameIdx++)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::AnimGfxPressColl, new ParameterLine(parP, mFonts, mTextures, mWindow)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::AnimGfxPressMty, new ParameterLine(emptyP, mFonts, mTextures, mWindow)));

    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Collection, nullptr, mCollectionNames.at(collectionNameIdx++)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::MainWndwColl, new ParameterLine(parP, mFonts, mTextures, mWindow)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::MainWndwMty, new ParameterLine(emptyP, mFonts, mTextures, mWindow)));

    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Collection, nullptr, mCollectionNames.at(collectionNameIdx++)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::KPSWndwColl, new ParameterLine(parP, mFonts, mTextures, mWindow)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::KPSWndwMty, new ParameterLine(emptyP, mFonts, mTextures, mWindow)));

    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Hint, nullptr, "Hint: Increase window size in \"Main window\" to see effect of this feature!"));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::KeyPressVisHint, new ParameterLine(parP, mFonts, mTextures, mWindow)));
    mParameterLines[ParameterLine::ID::KeyPressVisHint]->setCharacterSize(17u);

    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Collection, nullptr, mCollectionNames.at(collectionNameIdx++)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::KeyPressVisColl, new ParameterLine(parP, mFonts, mTextures, mWindow)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::KeyPressVisMty, new ParameterLine(emptyP, mFonts, mTextures, mWindow)));

    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Collection, nullptr, mCollectionNames.at(collectionNameIdx++)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::OtherColl, new ParameterLine(parP, mFonts, mTextures, mWindow)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::OtherMty, new ParameterLine(emptyP, mFonts, mTextures, mWindow)));

    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Collection, nullptr, mCollectionNames.at(collectionNameIdx++)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::SaveStatColl, new ParameterLine(parP, mFonts, mTextures, mWindow)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::SaveStatMty, new ParameterLine(emptyP, mFonts, mTextures, mWindow)));


    unsigned info = static_cast<unsigned>(ParameterLine::ID::InfoColl);
    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Collection, nullptr, "                            Quick Guide"));
    mParameterLines.emplace(std::make_pair(static_cast<ParameterLine::ID>(info++), new ParameterLine(parP, mFonts, mTextures, mWindow)));

    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Collection, nullptr, "Click with right mouse button on the key to edit it, then a menu"));
    mParameterLines.emplace(std::make_pair(static_cast<ParameterLine::ID>(info++), new ParameterLine(parP, mFonts, mTextures, mWindow)));

    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Collection, nullptr, "will pop out. It has 2 fields. First one means the logical key,"));
    mParameterLines.emplace(std::make_pair(static_cast<ParameterLine::ID>(info++), new ParameterLine(parP, mFonts, mTextures, mWindow)));

    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Collection, nullptr, "the second one is the visual one - how the key will be displayed."));
    mParameterLines.emplace(std::make_pair(static_cast<ParameterLine::ID>(info++), new ParameterLine(parP, mFonts, mTextures, mWindow)));

    mParameterLines.emplace(std::make_pair(ParameterLine::ID::InfoMty, new ParameterLine(emptyP, mFonts, mTextures, mWindow)));


    unsigned hotKey = static_cast<unsigned>(ParameterLine::ID::HotKey1);
    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Collection, nullptr, "[Hotkeys]"));
    mParameterLines.emplace(std::make_pair(static_cast<ParameterLine::ID>(hotKey++), new ParameterLine(parP, mFonts, mTextures, mWindow)));

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

    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Collection, nullptr, "Program version: " + mProgramVersion));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::ProgramVersion, new ParameterLine(parP, mFonts, mTextures, mWindow)));

    mParameterLines.emplace(std::make_pair(ParameterLine::ID::LastLine, new ParameterLine(emptyP, mFonts, mTextures, mWindow)));

    const float stepX = 1000.f, stepY = 50.f;
    const float halfWindowSize = 400.f;
    const float padding = 5.f;
    const sf::Vector2f offset(155.f, 0.f);
    unsigned row = 1u, column = 0u;
    for (auto &pair : mParameterLines)
    {
        if (pair.first >= ParameterLine::ID::SaveStatColl && pair.first <= ParameterLine::ID::SaveStatMty)
        {
            // Hide them
            pair.second->setPosition(-1000.f, -1000.f);
            continue;
        }

        if (ParameterLine::isEmpty(pair.first))
        {
            if (pair.first == ParameterLine::ID::LastLine)
            {
                mParameterLines.at(ParameterLine::ID::ProgramVersion)
                    ->move(0.f, halfWindowSize - stepY * 3.f + padding);
            }
            mBounds.push_back(stepY * (row - 2) - halfWindowSize + padding);
            row = 0;
            ++column;
        }
        pair.second->setPosition(stepX * column + offset.x, stepY * row - halfWindowSize + padding);
        ++row;
    }
}

// The distance parameter contains offset of the view, 
// but the sliderbar is relative to the window,
// so it's neccessary to normilize both offset and cursor Y position,
// and then project the sum on the window
void Menu::moveSliderBarButtons(float offset)
{
    if (mBounds[mSelectedTab] < mView.getSize().y / 4.f)
        return;

    const sf::Vector2f sliderbarSize = mSliderBar.getSize();
    const sf::Vector2f sliberbarPosition = mSliderBar.getPosition();
    const sf::Vector2u windowSize = mWindow.getSize();
    const float highBounds = sliderbarSize.y / 2;
    const float lowBounds = mWindow.getSize().y - sliderbarSize.y / 2.f;

    const float normilizedOffset = offset / mBounds[mSelectedTab] / 1.5f;
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
    if (mBounds[mSelectedTab] < mView.getSize().y / 4)
        return;
        
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
    mView.setCenter(mView.getCenter().x, mBounds[mSelectedTab] * normilizedViewHeight);
}

void Menu::returnViewInBounds()
{
    const bool tooHigh = mView.getCenter().y < mHighViewBounds;
    const bool tooLow = mView.getCenter().y > mBounds[mSelectedTab];

    if (mBounds[mSelectedTab] < mView.getSize().y / 4)
    {
        mView.setCenter(mView.getCenter().x, 0);
        return;
    }

    if (tooHigh)
        mView.setCenter(mView.getCenter().x, mHighViewBounds);
    if (tooLow)
        mView.setCenter(mView.getCenter().x, mBounds[mSelectedTab]);
}

void Menu::saveConfig(const std::vector<std::unique_ptr<Button>> &mKeys)
{
    if (Settings::SaveStats)
        updateSaveStatsStrings();
    ConfigHelper::saveConfig(mParameters, mParameterLines, &mKeys, true);
}

void Menu::requestFocus()
{
    mWindow.requestFocus();
}

void Menu::updateSaveStatsStrings()
{
    auto it = mParameters.find(LogicalParameter::ID::SaveStatMaxKPS);
    it->second->setValStr(std::to_string(it->second->getDigit<float>()));
    ++it;

    while (it->first <= LogicalParameter::ID::SaveStatTotal15 && it != mParameters.end())
    {
        it->second->setValStr(std::to_string(it->second->getDigit<unsigned>()));
        ++it;
    }
}
